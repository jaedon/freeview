/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp, nrdpPartner, nrdp_platform, NTRACE */

nrdp.system = {
    get bootURL() { return this._syncData.bootUrl; },
    get bootURLTime() { return this._syncData.bootUrlTime; },

    get uiQueryString() { return this._syncData.uiQueryString; },
    set uiQueryString(query) { nrdp._setProperty("system", "uiQueryString", query); },

    _handleEvent: function(event) {
        if (event.name == "keypress") {
            var keyCode = -1;
            for (var key in nrdpPartner.Keys) {
                if (nrdpPartner.Keys[key] == event.data) {
                    keyCode = key;
                    break;
                }
            }

            if (keyCode == -1) {
                nrdp.log.warn("Unknown key: " + event.data);
                return false;
            } else {
                NTRACE("Dispatching key " + event.data + ", keycode " + keyCode);
                if (window.nrdp_platform != null && window.nrdp_platform.sendKey != null) {
                    nrdp_platform.sendKey(keyCode);
                } else {
                    var downEvent = document.createEvent("Event");
                    downEvent.initEvent("keydown", true, true);
                    downEvent.which = keyCode;
                    document.dispatchEvent(downEvent);
                    var upEvent = document.createEvent("Event");
                    upEvent.initEvent("keyup", true, true);
                    upEvent.which = keyCode;
                    document.dispatchEvent(upEvent);
                }
            }
            return true;
        } else {
            return false;
        }
    },
    _syncData: {}
};
