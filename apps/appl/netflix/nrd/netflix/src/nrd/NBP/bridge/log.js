/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp, nrdp_platform */

nrdp.log = {
    classname: "LogBridge",
    _path: "log",
    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    get UIContext() { return this._syncData.UIContext; },
    set UIContext(s) { nrdp._setProperty("log","UIContext",s); },

    get appid() { return this._syncData.appid; },

    get areas() { return this._syncData.areas; },

    get aseCodes() { return this._syncData.aseCodes; },

    get errorCodes() { return this._syncData.errorCodes; },

    get haveTracing() { return this._syncData.haveTracing; },

    get level() { return this._syncData.level; },
    set level(e) { nrdp._setProperty("log", "level", e); },

    get levels() { return this._syncData.levels; },

    get sessionid() { return this._syncData.sessionid; },

    get traceAreas() { return this._syncData.traceAreas; },
    set traceAreas(e) { nrdp._setProperty("log", "traceAreas", e); },

    //get xid() { return this._syncData.xid; },

    flush: function flush() {
        nrdp._invoke("log", "flush");
    },

    _traceAreaMap: null,

    _log: function _log(level, args) {
        var msg = args[0],
            area = args[1],
            type = args[2],
            tags = args[3],
            critical = args[4],
            sendtoAppboot = args[5]?args[5]:false,
            logLevel;

        if (!nrdp._isError && this._syncData.levels) {
            if (level == "trace" && !nrdp.debug && !this.haveTracing)
                return;

            logLevel = this.levels[level];

            // compat with cadmium, could get an exception object passed in here
            if (typeof args[1] === "object" && args[1] !== null && /^\w*Error$/.test(args[1].name)) {
                area = undefined;
                if (!tags)
                    tags = {};
                tags["exception"] = args[1].message || ("" + args[1]);
                tags["stack"] = args[1].stack;
            }

            nrdp._invoke("log", "log", {
                logLevel: logLevel,
                msg: msg,
                traceArea: area,
                type: type,
                tags: tags,
                critical: critical,
                sendtoappboot: sendtoAppboot
            });

            if (!critical && ( type || (nrdp.log.level > nrdp.log.levels.trace && logLevel >= nrdp.log.level ))) {
                nrdp._callEventListeners(this, {
                    'type': 'logMsgsReady',
                    'data': [{
                        'monotime': nrdp.mono(),
                        'area': area,
                        'level': logLevel,
                        'msg': msg,
                        'tags': tags,
                        'logtype': type,
                        'threadname': "UI_THREAD",
                        'threadid': "1",
                        'critical': false
                    }]
                });
            }

        } else {
            if (args.length != 1)
                msg = args[1] + ": " + args[0];
            this.console(msg);
        }
    },

    milestone: function milestone(msg) { nrdp._invoke("log", "milestone", { msg: msg } ); },
    console: function console(msg) {
        if (nrdp._backchannel && nrdp._backchannel.console)
            nrdp._backchannel.console(msg);
        else if (typeof nrdp_platform !== "undefined" && nrdp_platform.console) //nrdp
            nrdp_platform.console(msg);
        else if (typeof window !== "undefined" && window.console.log) //browser
            window.console.log(msg);
    },
    debug: function debug() { nrdp.log._log("debug", arguments); },
    info: function info() { nrdp.log._log("info", arguments); },
    warn: function warn() { nrdp.log._log("warn", arguments); },
    error: function error() { nrdp.log._log("error", arguments); },
    fatal: function fatal() { nrdp.log._log("fatal", arguments); },
    trace: function trace() { nrdp.log._log("trace", arguments); },
    resetAppID: function resetAppID(newId) {
        nrdp.log.error("Resetting Appid:" + nrdp.log.appid );
        nrdp._invoke("log", "resetAppID", {appid: newId});
    },
    createArea: function createArea(area, groups) {
        nrdp._invoke("log", "createArea", {area: area, groups: groups});
        if (this._traceAreaMap) {
            this._traceAreaMap[area] = area;
        }
    },
    resetSessionID: function resetSessionID() { return nrdp._invoke("log", "resetSessionID"); },

    getLogMessages: function getLogMessages() { nrdp._invoke("log", "getLogMessages"); },

    getCriticalMessages: function getCriticalMessages() { nrdp._invoke("log", "getCriticalMessages"); },

    deleteCriticalMessages: function deleteCriticalMessages(criticalMessages) { nrdp._invoke("log", "deleteCriticalMessages", {criticalMessages: criticalMessages}); },

    _syncData: { },
    _updateProperty: function _updateProperty(property, value) {
        var evt;
        if (nrdp.isReady) {
            if (property == "sessionid") {
                evt = {
                    type: "sessionIDChanged",
                    data: value
                };
            }
        }

        this._syncData[property] = value;

        if (evt) {
            nrdp._callEventListeners(this, evt);
        }
    },

    _handleEvent: function _handleEvent(event) {
        var evt;
        if ( event.name == "logMsgsReady") {
            evt = { data : event.data };
            evt.type = event.name;
            nrdp._callEventListeners(this, evt);
        } else if ( event.name == "criticalMsgsReady") {
            evt = { data : event.data };
            evt.type = event.name;
            nrdp._callEventListeners(this, evt);
        } else if (event.name == "logflush") {
            evt = { name: "logflush", type: "logflush"};
            nrdp._callEventListeners(this, evt);
        } else if (event.name == "appIdResetComplete") {
            evt = { type: "appIdResetComplete", data : event.data};
            nrdp._callEventListeners(this, evt);
        } else {
            return false;
        }

        return true;
    }
};
