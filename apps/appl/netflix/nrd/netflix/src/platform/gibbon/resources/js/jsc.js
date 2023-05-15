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
/*global nrdp, nrdp_platform*/

(function() {
function _gotEvent(arg) {
    nrdp._sendSyncdEvent(nrdp._gotEvent, nrdp, arg);
}

function _setupBackchannel() {
    if (typeof nrdp_platform === "undefined" || typeof this.platform.jscBridgeEnabled === "undefined" || !this.platform.jscBridgeEnabled() )
        return false;
    return this.platform.jscBridgeInit(_gotEvent);
}

function _setProperty(subobj, prop, val) {
    var obj = subobj ? "nrdp." + subobj : "nrdp";
    return this.platform.jscBridgeSetProperty(obj, prop, val);
}

function _invoke(obj, method, args) {
    obj = obj ? "nrdp." + obj : "nrdp";
    return this.platform.jscBridgeInvoke(obj, method, args);
}

function _console(msg) {
    this.platform.log(msg);
}

function _atob(s, returnTypedArray) {
    return this.platform.atob(s, returnTypedArray);
}

function _btoa(s, urlSafe, returnTypedArray) {
    return this.platform.btoa(s, urlSafe, returnTypedArray);
}

function _parseJSON(json) {
    return this.platform.parseJSON(json);
}

function _mono() {
    return this.platform.mono();
}

function _pmono() {
    return this.platform.pmono();
}

function _parseXML(xml) {
    var obj = this.platform.parseXML(xml);
    if (obj) nrdp._fixXml(obj);
    return obj;
}

function _compress(data, type, binary) {
    return this.platform.compress(data, type, binary);
}

function _uncompress(data, type, returntypedarray) {
    return this.platform.uncompress(data, type, returntypedarray);
}

function _random(typed_array) {
    return this.platform.random(typed_array);
}

function _gctag(s) {
    return this.platform.gctag(s);
}

var backchannel = {
    name: "GibbonJavaScriptCore",
    isNative: true,
    init: _setupBackchannel,
    console: _console,
    setProperty: _setProperty,
    invoke: _invoke,
    atob: _atob,
    btoa: _btoa,
    mono: _mono,
    pmono: _pmono,
    atoutf8: function atoutf8(s) {
        return this.platform.atoutf8(s);
    },
    utf8toa: function utf8toa(s) {
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
