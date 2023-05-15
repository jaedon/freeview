/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp */

nrdp.instrumentation = {
    classname: "InstrumentationBridge",
    _path: "instrumentation",
    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    setParams: function setParams(enabled, events) {
        nrdp._invoke("instrumentation", "setParams", { enabled:enabled, events: events });
    },

    get ON() { return 0; },
    get SWITCHED() { return 1; },
    get TEST() { return 2; },
    get DEBUG() { return 3; },
    get WATCHDOG() { return 4; },

    get verbose() { return this._syncData.verbose; },

    generateEvent: function generateEvent(category, name, value) {
        nrdp._invoke("instrumentation", "event", { category:category, name: name, value:value });
    },
    startInterval: function startInterval(category, name, value, reset){
        nrdp._invoke("instrumentation", "intervalStart", { category: category, name: name, value: value, reset: reset });
    },
    incIntervalCounter: function incIntervalCounter(category, name, counter, increment) {
        nrdp._invoke("instrumentation", "intervalCount",
                     { category: category, name: name, counter: counter, increment: increment});
    },
    tagInterval: function tagInterval(category, name, value) {
        nrdp._invoke("instrumentation", "intervalTag", { category: category, name: name, value: value });
    },
    endInterval: function endInterval(category, name, value) {
        nrdp._invoke("instrumentation", "intervalEnd", { category: category, name: name, value: value});
    },
    cancelInterval: function cancelInterval(category, name) {
        nrdp._invoke("instrumentation","intervalCancel", { category: category, name: name });
    },
    stashOn: function stashOn() {
        nrdp._invoke("instrumentation", "stash", { on: true });
    },
    stashOff: function stashOff() {
        nrdp._invoke("instrumentation", "stash", { on: false });
    },
    popStash: function popStash(cb)
    {
        var idx = this._nextIdx++;
        this._cbs[idx] = cb;
        nrdp._invoke("instrumentation", "popStash", { idx: idx });
    },

    _flush: function _flush(cb) {
        var idx = this._nextIdx++;
        this._cbs[idx] = cb;
        nrdp._invoke("instrumentation", "flush", { idx: idx });
    },

    _handleEvent: function _handleEvent(event) {
        if (event.data && event.data.idx) {
            if (this._cbs[event.data.idx] instanceof Function) {
                this._cbs[event.data.idx](event.data.events);
            }
            delete this._cbs[event.data.idx];
        } else if (event.name == "verboseChanged") {
            nrdp._callEventListeners(this, { type: event.name });
        } else {
            return false;
        }
        return true;
    },
    _nextIdx: 1,
    _cbs: {}
};
