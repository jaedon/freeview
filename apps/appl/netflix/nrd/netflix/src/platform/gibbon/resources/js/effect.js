/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* global nrdp, GibbonSyncObject */

nrdp.gibbon.Effect = function(widget, type, id) {
    if(!widget._syncData.effects)
        widget._syncData.effects = {};

    this._object = new GibbonSyncObject(this, widget._id, nrdp.gibbon._sync_Effect, id, type);
    this._id = id;
    this._type = type;
    this._widget = widget;
    this._name = nrdp.gibbon.Effect._createName(this._id);
    this._path = widget._path + ".effects." + this._name;
    this._syncData = widget._syncData.effects[this._name];
    if(!this._syncData)
        this._syncData = widget._syncData.effects[this._name] = {};
    this._defaults = nrdp.gibbon.Effect._default;
    this._properties = nrdp._classes[this.classname].properties.byName;
};
nrdp.gibbon.Effect._createName = function(id) { return "effect" + id; };

nrdp.gibbon.Effect._default = {
    visible: true,
    accelerated: undefined,
    params: undefined
};

nrdp.gibbon.Effect.prototype = {
    classname: "EffectBridge",
    constructor: nrdp.gibbon.Effect,

    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    stopAnimation: function stopAnimation(property, end) {
        if(end === undefined)
            end = true;
        nrdp.gibbon._invoke(this._path, "stopAnimation", { property: property, end: end });
    },
    startAnimation: function startAnimation(property, start, end, duration, ease, append, flags) {
        var value = nrdp.gibbon._createAnimation(property, start, end, duration, ease, append, flags);

        var params = GIBBON_PULL("params");
        if(start === undefined) {
            var previous = params[property];
            if(previous && previous instanceof Object)
                previous = previous.value;
            value.animate.previous = previous;
        }
        params[property] = value;

        var _encodedParams = {};
        _encodedParams[property] = value;
        GIBBON_PUSH_APPEND("params", params, _encodedParams);
        return value.animate.id;
    },

    get type() { return this._type; },
    get accelerated() { return GIBBON_PULL("accelerated"); },

    get visible() { return GIBBON_PULL("visible"); },
    set visible(_visible) {
        GIBBON_PUSH("visible", _visible, _visible);
    },

    get params() { return GIBBON_PROXY_PULL("params"); },
    set params(_params) {
        if((this.type == "mask" || this.type == "diff") && _params.image) //fixup
            _params.image.url = nrdp.gibbon._resolveUrl(_params.image.url);
        GIBBON_PUSH_APPEND("params", _params, _params);
    },

    _handleEvent: function _handleEvent(event) {
        var handled = true;
        if (event.name == "animationFinished") {
            this._syncData[event.data.property] = event.data.value;
            nrdp._callEventListeners(this, {type: event.name, data: event.data});
        } else if (event.name == "effectChanged") {
            nrdp._callEventListeners(this, {type: event.name, data: event.data});
        } else {
            handled = false;
        }
        return handled;
    }
};
