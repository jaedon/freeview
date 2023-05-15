/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp */

nrdp.gibbon.tilemanager = {
    _path: "gibbon.tilemanager",
    classname: "TileManagerBridge",
    _syncData: {},

    get STATE_Ok() { return 0; },
    get STATE_ReadError() { return 1; },
    get STATE_IntegrityError() { return 2; },
    get STATE_VersionError() { return 3; },
    get STATE_ExpiryError() { return 4; },

    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    get tiles() { return nrdp.gibbon.tilemanager._syncData.tiles; },

    init: function init() { nrdp._invoke("gibbon.tilemanager", "init"); },

    splash: function splash(callback, width, height) {
        nrdp._invoke("gibbon.tilemanager", "splash", { id: nrdp.gibbon._setValue(callback), width: width, height: height });
    },

    _handleEvent: function _handleEvent(event) {
        if(event.name == "splash") {
            var cb = nrdp.gibbon._getValue(event.data.id);
            if(typeof cb == "function")
                cb(event.data);
            return true;
        }
        return false;
    },

    _updateProperty: function _updateProperty(property, value) {
        this._syncData[property] = value;
        nrdp._callEventListeners(this, { type: "tileschanged" } );
    }
};
