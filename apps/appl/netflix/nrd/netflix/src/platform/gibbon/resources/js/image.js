/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp, nrdp_platform, GIBBON_PULL, GIBBON_PUSH, GibbonSyncObject */

nrdp.gibbon.Image = function(widget, background, id) {
    if(!widget._syncData.images)
        widget._syncData.images = {};

    this._object = new GibbonSyncObject(this, widget._id, nrdp.gibbon._sync_Image, id, background);
    this._id = id;
    this._widget = widget;
    this._background = background;
    this._name = nrdp.gibbon.Image._createName(this._id);
    this._path = widget._path + ".images." + this._name;
    if(!this._syncData)
        this._syncData = widget._syncData.images[this._name] = {};
    this._defaults = nrdp.gibbon.Image._default;
    this._properties = nrdp._classes[this.classname].properties.byName;
};
nrdp.gibbon.Image._createName = function(id) { return "image" + id; };

nrdp.gibbon.Image.LAZY_DECODE = 0x01;
nrdp.gibbon.Image.LAZY_DOWNLOAD = 0x02;
nrdp.gibbon.Image.ALIGN_NORMAL = 0x00;
nrdp.gibbon.Image.ALIGN_LEFT = nrdp.gibbon.Image.ALIGN_NORMAL;
nrdp.gibbon.Image.ALIGN_TOP = nrdp.gibbon.Image.ALIGN_NORMAL;
nrdp.gibbon.Image.ALIGN_CENTER = 0x01;
nrdp.gibbon.Image.ALIGN_RIGHT = 0x02;
nrdp.gibbon.Image.ALIGN_BOTTOM = nrdp.gibbon.Image.ALIGN_RIGHT;
nrdp.gibbon.Image.ALIGN_TILE = 0x04;
nrdp.gibbon.Image.ALIGN_STRETCH = 0x08;
nrdp.gibbon.Image.ALIGN_ASPECT = 0x18;
nrdp.gibbon.Image._default = {
    src: "",
    purge: true,
    player: undefined,
    sendImageLoaded: false,
    sourceRect: { x: 0, y: 0, width: undefined, height: undefined },
    verticalAlignment: nrdp.gibbon.Image.ALIGN_NORMAL,
    horizontalAlignment: nrdp.gibbon.Image.ALIGN_NORMAL
};

nrdp.gibbon.Image.prototype = {
    classname: "ImageBridge",
    constructor: nrdp.gibbon.Image,

    get sendImageLoaded() { return GIBBON_PULL("sendImageLoaded"); },
    set sendImageLoaded(_sendImageLoaded) {
        if(_sendImageLoaded == this.sendImageLoaded)
            return;
        GIBBON_PUSH("sendImageLoaded", _sendImageLoaded, _sendImageLoaded);
    },

    addEventListener: function addEventListener(evt, listener) {
        if(nrdp._addEventListener(this, evt, listener)) {
            if(evt == "imageLoaded")
                this.sendImageLoaded = true;
        }
    },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    get height() { return GIBBON_PULL("height");  },
    get width() { return GIBBON_PULL("width");  },

    get visible() { return GIBBON_PULL("visible"); },
    set visible(_visible) {
        if(_visible == this.visible)
            return;
        GIBBON_PUSH("visible", _visible, _visible);
    },

    get sourceRect() { return GIBBON_PROXY_PULL("sourceRect"); },
    set sourceRect(_sourceRect_in) {
        var _sourceRect = GIBBON_PULL("sourceRect");
        if(_sourceRect &&
           _sourceRect_in.x == _sourceRect.x &&
           _sourceRect_in.y == _sourceRect.y &&
           _sourceRect_in.width == _sourceRect.width &&
           _sourceRect_in.height == _sourceRect.height)
            return;
        GIBBON_PUSH("sourceRect", _sourceRect_in, ([_sourceRect_in.x, _sourceRect_in.y, _sourceRect_in.width, _sourceRect_in.height]));
    },

    get purge() { return GIBBON_PULL("purge"); },
    set purge(_purge) {
        if(_purge == this.purge)
            return;
        GIBBON_PUSH("purge", _purge, _purge);
    },

    _encodeAlignment: function _encodeAlignment(align) {
        var result;
        if(align instanceof Array) {
            result = 0;
            for(var i = 0; i < align.length; ++i)
                result |= this._encodeAlignment(align[i]);
        } else if(typeof align === "string") {
            result = 0;
            var aligns = align.split(" ");
            for(var j = 0; j < aligns.length; ++j) {
                var a = aligns[j];
                if(a === "normal")
                    result |= nrdp.gibbon.Image.ALIGN_NORMAL;
                else if(a === "left")
                    result |= nrdp.gibbon.Image.ALIGN_LEFT;
                else if(a === "top")
                    result |= nrdp.gibbon.Image.ALIGN_TOP;
                else if(a === "center")
                    result |= nrdp.gibbon.Image.ALIGN_CENTER;
                else if(a === "right")
                    result |= nrdp.gibbon.Image.ALIGN_RIGHT;
                else if(a === "bottom")
                    result |= nrdp.gibbon.Image.ALIGN_BOTTOM;
                else if(a === "tile")
                    result |= nrdp.gibbon.Image.ALIGN_TILE;
                else if(a === "stretch")
                    result |= nrdp.gibbon.Image.ALIGN_STRETCH;
                else if(a === "aspect")
                    result |= nrdp.gibbon.Image.ALIGN_ASPECT;
            }
        } else {
            result = align;
        }
        return result;
    },

    get halign() { return this.horizontalAlignment; },
    set halign(_halign) { this.horizontalAlignment = _halign; },
    get horizontalAlignment() { return GIBBON_PULL("horizontalAlignment"); },
    set horizontalAlignment(_halign) {
        _halign = this._encodeAlignment(_halign);
        if(_halign == this.halign)
            return;
        GIBBON_PUSH("horizontalAlignment", _halign, _halign);
    },

    get valign() { return this.verticalAlignment; },
    set valign(_valign) { this.verticalAlignment = _valign; },
    get verticalAlignment() { return GIBBON_PULL("verticalAlignment"); },
    set verticalAlignment(_valign) {
        _valign = this._encodeAlignment(_valign);
        if(_valign == this.valign)
            return;
        GIBBON_PUSH("verticalAlignment", _valign, this._encodeAlignment(_valign));
    },

    reload: function reload() {
        var url = this.url;
        if(url instanceof Object) {
            if(!url.url || !url.url.length)
                return;
            url.lazy = false;
        } else {
            if(!url || !url.length)
                return;
        }
        GIBBON_PUSH("src", url, url);
    },

    get group() { return GIBBON_PULL("group"); },
    set group(_group) {
        if(_group == this.group)
            return;
        GIBBON_PUSH("group", _group, _group);
    },

    get player() { return GIBBON_PULL("player"); },

    get url() { return this.src; },
    set url(_url) { this.src = _url; },
    get src() { return GIBBON_PULL("src"); },
    set src(_url) {
        var url = GIBBON_PULL("src");
        if(_url instanceof Object) {
            if(_url.url)
                _url.url = nrdp.gibbon._resolveUrl(_url.url);
            if(typeof _url.lazy == "boolean")
                _url.lazy = _url.lazy ? nrdp.gibbon.Image.LAZY_DOWNLOAD : undefined;
            // not comparing typed arrays by content, only by reference
            if(url instanceof Object
               && ((_url.url && _url.url == url.url) || (_url.data && _url.data == url.data))
               && _url.lazy == url.lazy
               && _url.async == url.async
               && _url.scale == url.scale
               && _url.required == url.required
               && _url.timeout == url.timeout
               && _url.highPriority == url.highPriority
               && _url.connectTimeout == url.connectTimeout
               && _url.stallTimeout == url.stallTimeout) {
                if(_url.headers instanceof Object && url.headers instanceof Object) {
                    var same = true;
                    var prop;
                    for(prop in _url.headers) {
                        if(!_url.headers.hasOwnProperty(prop))
                            continue;
                        if(url.headers[prop] != _url.headers[prop]) {
                            same = false;
                            break;
                        }
                        if(same) {
                            for(prop in url.headers) {
                                if(!url.headers.hasOwnProperty(prop))
                                    continue;
                                if(_url.headers[prop] != url.headers[prop]) {
                                    same = false;
                                    break;
                                }
                            }
                            if(same)
                                return;
                        }
                    }
                } else if(!_url.headers && !url.headers) {
                    return;
                }
            } else if(!url && (!_url.url || !_url.url.length) && (!_url.data || !_url.data.length)) {
                return;
            }
            var tmp = _url;
            _url = { };
            var properties = 0;
            if(tmp.fast === true) {
                ++properties;
                _url.fast = tmp.fast;
            }
            if(tmp.lazy !== undefined) {
                ++properties;
                _url.lazy = tmp.lazy;
            }
            if(tmp.async === false) {
                ++properties;
                _url.async = tmp.async;
            }
            if(tmp.scale !== undefined) {
                ++properties;
                _url.scale = tmp.scale;
            }
            if(tmp.required === false) {
                ++properties;
                _url.required = tmp.required;
            }
            if(tmp.highPriority === true) {
                ++properties;
                _url.highPriority = tmp.highPriority;
            }
            if(tmp.timeout) {
                ++properties;
                _url.timeout = tmp.timeout;
            }
            if(tmp.connectTimeout) {
                ++properties;
                _url.connectTimeout = tmp.connectTimeout;
            }
            if(tmp.stallTimeout) {
                ++properties;
                _url.stallTimeout = tmp.stallTimeout;
            }
            if(tmp.url) {
                ++properties;
                _url.url = tmp.url;
            }
            if(tmp.data) {
                ++properties;
                _url.data = tmp.data;
            }
            if(tmp.headers) {
                var headers = {};
                var hasHeaders = false;
                for(var header in tmp.headers) {
                    if(!tmp.headers.hasOwnProperty(header))
                        continue;
                    var val = tmp.headers[header];
                    if(typeof val === "string") {
                        hasHeaders = true;
                        headers[header] = val;
                    }
                }
                if(hasHeaders) {
                    ++properties;
                    _url.headers = headers;
                }
            }
            if((properties <= 1 || !_url.url) && !_url.data) {
                _url = _url.url ? _url.url : "";
                if(url == _url)
                    return;
            }
        } else {
            _url = nrdp.gibbon._resolveUrl(_url);
            if(_url == url)
                return;
        }
        GIBBON_PUSH("src", _url, _url);
        // temporarily delete width/height; when the image comes back it will be populated again
        delete this._syncData.width;
        delete this._syncData.height;
    },

    _handleEvent: function _handleEvent(event) {
        var handled = true;
        if(event.name == "imageLoaded") {
            if(event.data.success) {
                if(nrdp.gibbon._breaks !== undefined)
                    this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
                if(event.data.state == "player")
                    this._syncData.player = nrdp.player;
                this._syncData.width = event.data.width;
                this._syncData.height = event.data.height;
            }
            var evt = { type: event.name, data: event.data };
            nrdp._callEventListeners(this, evt);
        } else {
            handled = false;
        }
        return handled;
    }
};
