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

function _gotEvents(events) {
    for (var i = 0; i < events.length; i++)
        nrdp._sendSyncdEvent(nrdp._gotEvent, nrdp, [events[i]]);
};

function _setupBackchannel(url) {
    if (typeof nrdp_platform === "undefined" || typeof nrdp_platform.mozjsBridgeEnabled === "undefined" || !nrdp_platform.mozjsBridgeEnabled() )
        return false;
    nrdp_platform.mozjsBridgeInit(_gotEvents);
    return true;
};

function _setProperty(subobj, prop, val) {
    var obj = subobj ? "nrdp." + subobj : "nrdp";
    var events = nrdp_platform.mozjsBridgeSetProperty(obj, prop, val);
    if(events)
        _gotEvents(events);
}

function _invoke(obj, method, args) {
    obj = obj ? "nrdp." + obj : "nrdp";
    var events = nrdp_platform.mozjsBridgeInvoke(obj, method, args);
    if(events)
        _gotEvents(events);
}

function _console(msg) {
    nrdp_platform.console(msg);
}

var backchannel = {
    name: "GibbonSpiderMonkey",
    isNative: true,
    init: _setupBackchannel,
    console: _console,
    setProperty: _setProperty,
    invoke: _invoke
};

nrdp._backchannels.unshift(backchannel);

})();
