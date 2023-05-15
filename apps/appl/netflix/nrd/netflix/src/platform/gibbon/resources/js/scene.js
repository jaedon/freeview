/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp */

nrdp.gibbon.scene = {
    classname: "ScreenBridge",

    _callbacks: {},
    _nextCallbackId: 1,

    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    get suspend() { return (this._suspendPending === undefined) ? this._syncData.suspend : this._suspendPending; },
    set suspend(_suspend) {
        if(_suspend == this.suspend)
            return;

        if(this._suspendPending === undefined) {
            this._syncData.suspend = _suspend;
            nrdp._setProperty("gibbon.scene", "suspend", _suspend);
        }
        this._suspendPending = _suspend;
    },

    get widget() { return this.root; },
    set widget(_widget) { this.root = _widget; },
    get root() {
        var wid = this._syncData.root;
        return (typeof wid === "object") ? wid : null;
    },
    set root(_widget) {
        if(_widget && this.root && _widget._id == this.root._id)
            return;
        var oldRoot = this.root;
        this._syncData.root = _widget;
        nrdp._setProperty("gibbon.scene", "root", _widget ? _widget._id : undefined);
        this._rootChanged();
    },

    get overlay() {
        var wid = this._syncData.overlay;
        return (typeof wid === "object") ? wid : null;
    },
    set overlay(_widget) {
        if(_widget && this.overlay && _widget._id == this.overlay._id)
            return;
        var oldOverlay = this.overlay;
        this._syncData.overlay = _widget;
        nrdp._setProperty("gibbon.scene", "overlay", _widget ? _widget._id : undefined);
    },

    get scale() { return this._syncData.scale; },

    get width() { return this._syncData.width; },
    get height() { return this._syncData.height; },

    dump: function dump(cb) {
        this.widget.dump(cb);
    },
    grab: function grab(cb) {
        var id = this._registerCallback(cb);
        nrdp._invoke("gibbon.scene", "grab", { id: id });
    },
    update: function update(widget) {
        nrdp._invoke("gibbon.scene", "update", { id: widget ? widget._id : this.widget._id });
    },

    _rootChanged: function _rootChanged() {
        if(nrdp._hasEventListener(this, "rootChanged"))
            nrdp._callEventListeners(this, { type: "rootChanged" });
    },

    _handleEvent: function _handleEvent(event) {
        if(event.name == "grab") {
            this._callCallback(event);
            return true;
        } else if(event.name == "suspendChanged") {
            this._syncData.suspend = event.data.suspended;
            var evt = { type: event.name, data: event.data };
            nrdp._callEventListeners(this, evt);
            if(this._suspendPending !== undefined) {
                var suspendPending = this._suspendPending;
                this._suspendPending = undefined;
                if(suspendPending !== event.data.suspended) {
                    this.suspend = suspendPending;
                }
            }
            return true;
        }
        return false;
    },
    _registerCallback: function _registerCallback(cb) {
        var id = this._nextCallbackId++;
        this._callbacks[id] = cb;
        return id;
    },
    _callCallback: function _callCallback(event) {
        if (event.data.id === undefined)
            return;
        var cb = this._callbacks[event.data.id];
        delete this._callbacks[event.data.id];
        if (cb)
            cb(event.data.data);
    }
};
