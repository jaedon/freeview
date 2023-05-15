/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
 * all nrdp communication helpers are required to do four things:
 *
 * 1. add object to nrdp._backchannels for initialization
 * 2. provide nrdp._setProperty()
 * 3. provide nrdp._invoke()
 * 4. call nrdp._gotEvent() when an event comes in.
 */

(function() {

function _gotEvent(arg) {
    nrdp._sendSyncdEvent(nrdp._gotEvent, nrdp, arg);
};

function _setupBackchannel() {
    if (typeof nrdp_platform === "undefined" || typeof nrdp_platform.v8BridgeEnabled === "undefined" || !nrdp_platform.v8BridgeEnabled() )
        return false;
    return this.platform.v8BridgeInit(_gotEvent);
};

function _setProperty(subobj, prop, val) {
    var obj = subobj ? "nrdp." + subobj : "nrdp";
    this.platform.v8BridgeSetProperty(obj, prop, val);
}

function _invoke(obj, method, args) {
    obj = obj ? "nrdp." + obj : "nrdp";
    return this.platform.v8BridgeInvoke(obj, method, args);
}

function _console(msg) {
    this.platform.console(msg);
}

function _mono() {
    return this.platform.mono();
}

function _now() {
    return this.platform.now();
}

function _atob(s, returnTypedArray) {
    this.platform.console("atob");
    return this.platform.atob(s, returnTypedArray);
}

function _btoa(s, urlSafe, returnTypedArray) {
    this.platform.console("btoa");
    return this.platform.btoa(s, urlSafe, returnTypedArray);
}

function _parseJSON(json) {
    this.platform.console("parseJSON");
    return this.platform.parseJSON(json);
}

function _parseXML(xml) {
    this.platform.console("parseXML");
    var obj = this.platform.parseXML(xml);
    if (obj) nrdp._fixXml(obj);
    return obj;
}

function _compress(data, type, binary) {
    this.platform.console("compress");
    return this.platform.compress(data, type, binary);
}

function _uncompress(data, type, returntypedarray) {
    this.platform.console("uncompress");
    return this.platform.uncompress(data, type, returntypedarray);
}

function _random(typed_array) {
    this.platform.console("random");
    return this.platform.random(typed_array);
}

function _gctag(s) {
    this.platform.console("gctag");
    return this.platform.gctag(s);
}

var backchannel = {
    name: "GibbonV8",
    isNative: true,
    init: _setupBackchannel,
    console: _console,
    setProperty: _setProperty,
    invoke: _invoke,
    mono: _mono,
    now: _now,
    atob: _atob,
    btoa: _btoa,
    atoutf8: function atoutf8(s) {
        this.platform.console("atoutf8");
        return this.platform.atoutf8(s);
    },
    utf8toa: function utf8toa(s) {
        this.platform.console("utf8toa");
        return this.platform.utf8toa(s);
    },
    parseJSON: _parseJSON,
    parseXML: _parseXML,
    compress: _compress,
    uncompress: _uncompress,
    random: _random,
    gctag: _gctag,
    // I know this looks weird, but running in Chrome has a problem otherwise
    platform: typeof nrdp_platform === "undefined" ? undefined : nrdp_platform
};

nrdp._backchannels.unshift(backchannel);

})();
