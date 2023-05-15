/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp, nrdp_platform, GIBBON_PULL, GIBBON_PUSH, GIBBON_PUSH_APPEND, GibbonSyncObject */

nrdp.gibbon.Widget = function(id) {
    this._object = new GibbonSyncObject(this, id, nrdp.gibbon._sync_Widget);
    this._id = id;
    this._animations = { pending: {} };
    this._name = nrdp.gibbon.Widget._createName(this._id);
    this._path = "nrdp.gibbon.widgets." + this._name;
    this._children = [];
    this._callbacks = {};
    this._syncData = {};
    this._defaults = nrdp.gibbon.Widget._default;
    this._properties = nrdp._classes[this.classname].properties.byName;
};

nrdp.gibbon.Widget._createName = function(id) { return "widget" + id; };

nrdp.gibbon.Widget.ALIGN_LEFT = 0x01;
nrdp.gibbon.Widget.ALIGN_RIGHT = 0x02;
nrdp.gibbon.Widget.ALIGN_CENTER_HORIZONTAL = 0x04;
nrdp.gibbon.Widget.ALIGN_TOP = 0x10;
nrdp.gibbon.Widget.ALIGN_BOTTOM = 0x20;
nrdp.gibbon.Widget.ALIGN_CENTER_VERTICAL = 0x40;

nrdp.gibbon.Widget._encodeAlignment = function(align) {
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
            if(a == "right")
                result |= nrdp.gibbon.Widget.ALIGN_RIGHT;
            else if(a == "left")
                result |= nrdp.gibbon.Widget.ALIGN_LEFT;
            else if(a == "top")
                result |= nrdp.gibbon.Widget.ALIGN_TOP;
            else if(a == "bottom")
                result |= nrdp.gibbon.Widget.ALIGN_BOTTOM;
            else if(a == "center" || a == "center-both")
                result |= nrdp.gibbon.Widget.ALIGN_CENTER_HORIZONTAL|nrdp.gibbon.Widget.ALIGN_CENTER_VERTICAL;
            else if(a == "center-horizontal")
                result |= nrdp.gibbon.Widget.ALIGN_CENTER_HORIZONTAL;
            else if(a == "center-vertical")
                result |= nrdp.gibbon.Widget.ALIGN_CENTER_VERTICAL;
        }
    } else {
        result = align;
    }
    return result;
};

nrdp.gibbon.Widget._default = {
    push_warnedPendingSync: false,
    effects: undefined,
    name: undefined,
    x: undefined,
    y: undefined,
    width: undefined,
    height: undefined,
    minWidth: undefined,
    minHeight: undefined,
    maxWidth: undefined,
    maxHeight: undefined,
    padding: { left: 0, top: 0, bottom: 0, right: 0, wrap: 0 },
    layout: undefined,
    parent: undefined,
    layoutStretch: 0,
    layoutSpacing: 0,
    opacity: 1.0,
    drawOrder: 0,
    scale: 1.0,
    clip: true,
    cache: undefined,
    opaque: false,
    erase: false,
    video: false,
    visible: true,
    loadImages: undefined,
    smoothScale: false,
    sendRenderProperties: false,
    sendAnimationFinished: false,
    backgroundColor: { r: 0, g: 0, b: 0, a: 0 },
    scrollX: 0,
    scrollY: 0,
    transformOriginX: 0,
    transformOriginY: 0
};


nrdp.gibbon.Widget.prototype = {
    classname: "WidgetBridge",
    constructor: nrdp.gibbon.Widget,

    _describe: function _describe() {
        var result;
        if(this.name)
            result = this.name;
        else
            result = "UNNAMED";
        result += "(" + this._id + ")";
        return result;
    },

    get effects() {  return this._effects; },
    addEffect: function addEffect(type, params) {
        if(!this._effects)
            this._effects = {};
        var effectid = nrdp.gibbon._nextEffectId++;
        if(effectid > nrdp.gibbon.INT_MAX)
            effectid = 1;
        var effect = new nrdp.gibbon.Effect(this, type, effectid);
        effect.params = params ? params : {};
        this._effects[effect._name] = effect;
        return effect;
    },
    removeEffect: function removeEffect(effect) {
        var effectID = effect;
        if(effect instanceof Object)
            effectID = effect._id;
        for(var e in this._effects) {
            if(this._effects[e]._id == effectID) {
                nrdp.gibbon._invoke(this._path, "removeEffect", { effect: effectID });
                delete this._effects[e];
                break;
            }
        }
    },

    findWidget: function(id) {
        if(typeof id === "number") {
            if(this._id == id)
                return this;
        } else if(this.name == id) {
            return this;
        }
        for(var i = 0; i < this._children.length; ++i) {
            var widget = this._children[i].findWidget(id);
            if(widget)
                return widget;
        }
        return undefined;
    },

    addEventListener: function addEventListener(evt, listener) {
        if(nrdp._addEventListener(this, evt, listener)) {
            if(evt == "renderpropertychange") {
                this.sendRenderProperties = true;
            } else if(evt == "animationFinished") {
                this.sendAnimationFinished = true;
            } else if(evt == "imageLoaded" || evt == "backgroundImageLoaded") {
                var image = (evt == "imageLoaded") ? this.image : this.backgroundImage;
                if(!image._widgetImageLoadedHandler) {
                    image._widgetImageLoadedHandler = function(event) {
                        var loadEvent = { type: evt, data: event.data };
                        nrdp._callEventListeners(image._widget, loadEvent);
                    };
                    image.addEventListener("imageLoaded", image._widgetImageLoadedHandler);
                }
            }
        }
    },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    _sendRenderProperty: function _sendRenderProperty(property, renderProperty, value) {
        //nrdp.log.error("RENDEREVENT: " + " - p=" + property + ", rP=" + renderProperty + ", v=" + value);
        nrdp._callEventListeners(this, { type: "renderpropertychange",
                                         property: property,
                                         renderProperty: renderProperty,
                                         value: value });
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },
    _renderProperty: function _renderProperty(property) {
        if(!this.sendRenderProperties) {
            nrdp.log.error("RenderProperty(" + property + "): Used but WIDGET(" + this._id + ") has not set sendRenderProperties!");
            this.sendRenderProperties = true;
        }
        return GIBBON_PULL(property);
    },
    get sendRenderProperties() { return GIBBON_PULL("sendRenderProperties"); },
    set sendRenderProperties(_sendRenderProperties) {
        if(_sendRenderProperties == this.sendRenderProperties)
            return;
        if(this._text)
            this._text.sendRenderProperties = _sendRenderProperties;
        GIBBON_PUSH("sendRenderProperties", _sendRenderProperties, _sendRenderProperties);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get sendAnimationFinished() { return GIBBON_PULL("sendAnimationFinished"); },
    set sendAnimationFinished(_sendAnimationFinished) {
        if(_sendAnimationFinished == this.sendAnimationFinished)
            return;
        GIBBON_PUSH("sendAnimationFinished", _sendAnimationFinished, _sendAnimationFinished);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get color() { return this.backgroundColor; },
    set color(_color) { this.backgroundColor = _color; },
    get backgroundColor() { return GIBBON_PROXY_PULL("backgroundColor"); },
    set backgroundColor(_color) {
        if(typeof _color === "string") {
            if(_color[0] == '#') {
                var r = parseInt(_color.substr(1, 2), 16); if(isNaN(r)) r = 0;
                var g = parseInt(_color.substr(3, 2), 16); if(isNaN(g)) g = 0;
                var b = parseInt(_color.substr(5, 2), 16); if(isNaN(b)) b = 0;
                var a = (_color.length == 9 ? parseInt(_color.substr(7, 2), 16) : 255); if(isNaN(a)) a = 255;
                _color = { r:r, g:g, b:b, a:a };
            } else {
                _color = undefined;
            }
        }
        if(_color === undefined)
            _color = { r:0, g:0, b:0, a:0 };
        else if(!(_color instanceof Object))
            _color = { r:0, g:0, b:0, a:255 };
        if(nrdp.gibbon._compareColor(_color, GIBBON_PULL("backgroundColor")))
            return;
        var value = { r: _color.r, g: _color.g, b: _color.b, a: _color.a };
        GIBBON_PUSH("backgroundColor", value, nrdp.gibbon._encodeColor(value));
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get padding() { return GIBBON_PROXY_PULL("padding"); },
    set padding(_padding) {
        var encodedPadding = _padding;
        var padding = GIBBON_PULL("padding");
        if(_padding instanceof Object) {
            if(padding instanceof Object && _padding.top == padding.top && _padding.left == padding.left &&
               _padding.bottom == padding.bottom && _padding.right == padding.right && _padding.wrap == padding.wrap)
                return;
            _padding = { top: _padding.top, left: _padding.left, bottom: _padding.bottom, right: _padding.right, wrap: _padding.wrap };
            encodedPadding = [_padding.top, _padding.left, _padding.bottom, _padding.right, _padding.wrap];
        } else if(typeof _padding == "number") {
            if(_padding == padding)
                return;
        } else {
            return;
        }
        GIBBON_PUSH("padding", _padding, encodedPadding);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get name() { return GIBBON_PULL("name"); },
    set name(_name) {
        if(typeof _name == "number")
            _name += "";
        if(_name && !_name.length)
            _name = undefined;
        if(_name == this.name)
            return;
        GIBBON_PUSH("name", _name, _name);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get image() {
        if(this._image === undefined) {
            var widget = this;
            this._image = this.addForegroundImage();
        }
        return this._image;
    },
    get backgroundImage() {
        if(this._backgroundImage === undefined) {
            var widget = this;
            this._backgroundImage = this.addBackgroundImage();
        }
        return this._backgroundImage;
    },
    get images() {  return this._images; },
    addForegroundImage: function addForegroundImage(obj) { return this.addImage(false, obj); },
    addBackgroundImage: function addBackgroundImage(obj) { return this.addImage(true, obj); },
    addImage: function addImage(background, obj) {
        if(!this._images)
            this._images = {};
        var imageid = nrdp.gibbon._nextImageId++;
        if(imageid > nrdp.gibbon.INT_MAX)
            imageid = 1;
        var image = new nrdp.gibbon.Image(this, background, imageid);
        if(obj) {
            for(var i in obj)
                image[i] = obj[i];
        }
        this._images[image._name] = image;
        if(background) {
            if(!this._backgroundImage)
                this._backgroundImage = image;
        } else if(!this._image) {
            this._image = image;
        }
        return image;
    },
    removeImage: function removeImage(image) {
        var imageID = image;
        if(image instanceof Object)
            imageID = image._id;
        if(image._background) {
            if(this._backgroundImage && this._backgroundImage._id == imageID)
                this._backgroundImage = undefined;
        }  else if(this._image && this._image._id == imageID) {
            this._image = undefined;
        }
        for(var i in this._images) {
            if(this._images[i]._id == imageID) {
                nrdp.gibbon._invoke(this._path, "removeImage", { image: imageID });
                delete this._images[i];
                break;
            }
        }
    },

    get renderX() { return this._renderProperty("renderX"); },
    get renderY() { return this._renderProperty("renderY"); },
    get renderWidth() { return this._renderProperty("renderWidth"); },
    get renderHeight() { return this._renderProperty("renderHeight"); },
    get renderTextLines() { return this.text.renderLines; },
    get renderTextFirstLine() { return this.text.renderFirstLine; },
    get renderTextCursorPosition() { return this.text.renderCursorPosition; },

    get drawOrder() { return GIBBON_PULL("drawOrder"); },
    set drawOrder(_drawOrder) {
        if(_drawOrder == this.drawOrder)
            return;
        GIBBON_PUSH("drawOrder", _drawOrder, _drawOrder);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get minHeight() { return GIBBON_PULL("minHeight"); },
    set minHeight(_minHeight) {
        if(_minHeight == this.minHeight)
            return;
        GIBBON_PUSH("minHeight", _minHeight, _minHeight);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get minWidth() { return GIBBON_PULL("minWidth"); },
    set minWidth(_minWidth) {
        if(_minWidth == this.minWidth)
            return;
        GIBBON_PUSH("minWidth", _minWidth, _minWidth);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get maxHeight() { return GIBBON_PULL("maxHeight"); },
    set maxHeight(_maxHeight) {
        if(_maxHeight == this.maxHeight)
            return;
        GIBBON_PUSH("maxHeight", _maxHeight, _maxHeight);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get maxWidth() { return GIBBON_PULL("maxWidth"); },
    set maxWidth(_maxWidth) {
        if(_maxWidth == this.maxWidth)
            return;
        GIBBON_PUSH("maxWidth", _maxWidth, _maxWidth);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get loadImages() { return GIBBON_PULL("loadImages"); },
    set loadImages(_loadImages) {
        if(_loadImages == this.loadImages)
            return;
        GIBBON_PUSH("loadImages", _loadImages, _loadImages);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get visible() { return GIBBON_PULL("visible"); },
    set visible(_visible) {
        if(_visible == this.visible)
            return;
        GIBBON_PUSH("visible", _visible, _visible);
        if(nrdp.gibbon.scene.root && nrdp.gibbon.scene.root._id == this._id)
            nrdp.gibbon.scene._rootChanged();
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get smoothScale() { return GIBBON_PULL("smoothScale"); },
    set smoothScale(_smoothScale) {
        if(_smoothScale == this.smoothScale)
            return;
        GIBBON_PUSH("smoothScale", _smoothScale, _smoothScale);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get hidden() { return !this.visible; },
    set hidden(_hidden) { this.visible = !_hidden; },

    get opacity() {
        var value = GIBBON_PULL("opacity");
        if(value && value instanceof Object)
            return value.value;
        return value;
    },
    set opacity(_opacity) {
        if(isNaN(_opacity))
            _opacity = undefined;
        if(_opacity == this.opacity)
            return;
        this._setAnimatedValue("opacity", _opacity);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get clone() {
        var c = GIBBON_PULL("clone");
        if(c)
            return nrdp.gibbon.findWidget(c);
        return undefined;
    },
    set clone(_clone) {
        if(!_clone && !this.clone)
            return;
        if(_clone && this.clone && _clone._id == this.clone._id)
            return;
        var clone_id = _clone ? _clone._id : undefined;
        GIBBON_PUSH("clone", clone_id, clone_id);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    _addChild: function _addChild(child) {
        this._children.push(child);
    },
    _removeChild: function _removeChild(child) {
        for(var ch in this._children) {
            if(this._children[ch]._id == child._id) {
                this._children.splice(ch, 1);
                break;
            }
        }
    },
    get _isRoot() {
        if(nrdp.gibbon.scene.root && nrdp.gibbon.scene.root._id == this._id)
            return true;
        else if(nrdp.gibbon.scene.overlay && nrdp.gibbon.scene.overlay._id == this._id)
            return true;
        return false;
    },
    get children() { return this._children; },
    get parent() { return GIBBON_PULL("parent"); },
    set parent(_parent) {
        if(typeof _parent === "number")
            _parent = nrdp.gibbon.findWidget(_parent);
        if(!_parent && !this.parent)
            return;
        if(_parent && this.parent && _parent._id == this.parent._id)
            return;
        if(this.parent)
            this.parent._removeChild(this);
        if(_parent) {
            GIBBON_PUSH("parent", _parent, ([ _parent._id, nrdp.gibbon._nextChildId++ ]));
            _parent._addChild(this);
        } else {
            GIBBON_PUSH("parent", _parent, _parent);
        }
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreakSubtree(_parent);
    },

    get scale() {
        var value = GIBBON_PULL("scale");
        if(value && value instanceof Object)
            return value.value;
        return value;
    },
    set scale(_scale) {
        if(_scale == this.scale)
            return;
        this._setAnimatedValue("scale", _scale);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get text() {
        if(!this._text) {
            this._text = new nrdp.gibbon.Text(this);
            this._text.sendRenderProperties = this.sendRenderProperties;
        }
        return this._text;
    },
    set text(_text) {
        if(_text && _text instanceof Object) {
            for(var property in _text) {
                if(_text[property] !== undefined)
                    this.text[property] = _text[property];
            }
        } else {
            this.text.contents = _text;
        }
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get clip() { return GIBBON_PULL("clip"); },
    set clip(_clip) {
        if(_clip == this.clip)
            return;
        GIBBON_PUSH("clip", _clip, _clip);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get cache() { return GIBBON_PULL("cache"); },
    set cache(_cache) {
        if(_cache == this.cache)
            return;
        GIBBON_PUSH("cache", _cache, _cache);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get textFirstLine() {
        if(!this._text)
            return nrdp.gibbon.Text._default.firstLine;
        return this._text.firstLine;
    },
    set textFirstLine(_textFirstLine) { this.text.firstLine = _textFirstLine; },

    get textCursorPosition() {
        if(!this._text)
            return nrdp.gibbon.Text._default.cursorPosition;
        return this._text.cursorPosition;
    },
    set textCursorPosition(_textCursorPosition) { this.text.cursorPosition = _textCursorPosition; },

    get textStyle() { return this.text; },
    set textStyle(_textStyle) {
        if(this.textStyle == _textStyle)
            return;
        if(_textStyle._widget !== undefined && _textStyle._widget !== this)
            nrdp.log.error("CopyTextStyle(" + this._id + "): From different WIDGET(" + _textStyle._widget._id + ")!");
        for(var property in _textStyle) {
            if(_textStyle[property] !== undefined)
                this.text[property] = _textStyle[property];
        }
    },

    get transformOriginX() { return GIBBON_PULL("transformOriginX"); },
    set transformOriginX(_transformOriginX) {
        if(_transformOriginX == this.transformOriginX)
            return;
        GIBBON_PUSH("transformOriginX", _transformOriginX, _transformOriginX);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get transformOriginY() { return GIBBON_PULL("transformOriginY"); },
    set transformOriginY(_transformOriginY) {
        if(_transformOriginY == this.transformOriginY)
            return;
        GIBBON_PUSH("transformOriginY", _transformOriginY, _transformOriginY);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get scrollX() {
        var value = GIBBON_PULL("scrollX");
        if(value && value instanceof Object)
            return value.value;
        return value;
    },
    set scrollX(_scrollX) {
        if(_scrollX == this.scrollX)
            return;
        this._setAnimatedValue("scrollX", _scrollX);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get scrollY() {
        var value = GIBBON_PULL("scrollY");
        if(value && value instanceof Object)
            return value.value;
        return value;
    },
    set scrollY(_scrollY) {
        if(_scrollY == this.scrollY)
            return;
        this._setAnimatedValue("scrollY", _scrollY);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get erase() { return GIBBON_PULL("erase"); },
    set erase(_erase) {
        if(_erase == this.erase)
            return;
        GIBBON_PUSH("erase", _erase, _erase);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get video() { return GIBBON_PULL("video"); },
    set video(_video) {
        if(_video == this.video)
            return;
        GIBBON_PUSH("video", _video, _video);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get opaque() { return GIBBON_PULL("opaque"); },
    set opaque(_opaque) {
        if(_opaque == this.opaque)
            return;
        GIBBON_PUSH("opaque", _opaque, _opaque);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get layout() { return GIBBON_PULL("layout"); },
    set layout(_layout) {
        var layout = this.layout;
        if(_layout instanceof Object) {
            _layout = { layout: _layout.layout, align: nrdp.gibbon.Widget._encodeAlignment(_layout.align) };
            if(layout instanceof Object && _layout.align == layout.align && _layout.layout == layout.layout)
                return;
        } else if(_layout == layout) {
            return;
        }
        GIBBON_PUSH("layout", _layout, _layout);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get layoutSpacing() { return GIBBON_PULL("layoutSpacing"); },
    set layoutSpacing(_layoutSpacing) {
        if(_layoutSpacing == this.layoutSpacing)
            return;
        GIBBON_PUSH("layoutSpacing", _layoutSpacing, _layoutSpacing);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get layoutStretch() { return GIBBON_PULL("layoutStretch"); },
    set layoutStretch(_layoutStretch) {
        if(_layoutStretch == this.layoutStretch)
            return;
        GIBBON_PUSH("layoutStretch", _layoutStretch, _layoutStretch);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get rect() { return { x: this.x, y: this.y, width: this.width, height: this.height,
                          minWidth: this.minWidth, minHeight: this.minHeight, maxWidth: this.maxWidth, maxHeight: this.maxHeight  }; },
    set rect(_rect_in) {
        var _rect;
        var animate;
        if(_rect_in.hasOwnProperty("animate")) {
            animate = _rect_in.animate;
        } else if(!this._canAnimate()) {
            animate = false;
        }
        var rect_props = { x: 1, y: 1, width: 1, height: 1, minWidth: 0, minHeight: 0, maxWidth : 0, maxHeight: 0 };
        for(var d in rect_props) {
            if(d in _rect_in && _rect_in[d] != this[d]) {
                var value = _rect_in[d];
                if(rect_props[d] && _rect_in[d] !== undefined) {
                    var valueAnimation = animate ? animate : this._findAnimation(d);
                    if(valueAnimation) {
                        this.startAnimation(d, undefined, value, valueAnimation.duration, valueAnimation.ease, false, valueAnimation.flags);
                        continue;
                    }
                }
                GIBBON_PUSH(d, value, value);
            }
        }
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get x() {
        var value = GIBBON_PULL("x");
        if(value && value instanceof Object)
            return value.value;
        return value;
    },
    set x(_x) {
        if(isNaN(_x))
            _x = undefined;
        if(_x == this.x)
            return;
        this._setAnimatedValue("x", _x);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get y() {
        var value = GIBBON_PULL("y");
        if(value && value instanceof Object)
            return value.value;
        return value;
    },
    set y(_y) {
        if(isNaN(_y))
            _y = undefined;
        if(_y == this.y)
            return;
        this._setAnimatedValue("y", _y);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get width() {
        var value = GIBBON_PULL("width");
        if(value && value instanceof Object)
            return value.value;
        return value;
    },
    set width(_width) {
        if(isNaN(_width))
            _width = undefined;
        if(_width == this.width)
            return;
        this._setAnimatedValue("width", _width);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get height() {
        var value = GIBBON_PULL("height");
        if(value && value instanceof Object)
            return value.value;
        return value;
    },
    set height(_height) {
        if(isNaN(_height))
            _height = undefined;
        if(_height == this.height)
            return;
        this._setAnimatedValue("height", _height);
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    dump: function dump(cb) {
        if(cb !== undefined) {
            var id = this._registerCallback(cb);
            nrdp.gibbon._invoke(this._path, "dump", { id: id });
        } else {
            nrdp.gibbon._invoke(this._path, "dump");
        }
    },
    _exec: function _exec(args) {
        nrdp.gibbon._invoke(this._path, "exec", args);
    },
    grab: function grab(cb) {
        var id = this._registerCallback(cb);
        nrdp.gibbon._invoke(this._path, "grab", { id: id });
    },
    setDisplayFlags: function setDisplayFlags(_flags) { //compat
        var opaque = false;
        var erase = false;
        var alignment = nrdp.gibbon.Image.ALIGN_NORMAL;
        var flags = _flags.split(" ");
        for (var f in flags) {
            var flag = flags[f];
            if(flag == "opaque")
                opaque = true;
            else if(flag == "erase")
                erase = true;
            else if(flag == "scale")
                alignment = nrdp.gibbon.Image.ALIGN_STRETCH;
            else if(flag == "tile")
                alignment = nrdp.gibbon.Image.ALIGN_TILE;
            else if(flag == "center")
                alignment = nrdp.gibbon.Image.ALIGN_CENTER;
        }
        this.erase = erase;
        this.opaque = opaque;
        this.image.halign = alignment;
        this.image.valign = alignment;
    },

    _setAnimatedValue: function _setAnimatedValue(property, value) {
        if(!(value instanceof Object) && value !== undefined) {
            var implicitAnimation = this._findAnimation(property);
            if(implicitAnimation && this._canAnimate())
                return this.startAnimation(property, undefined, value, implicitAnimation.duration, implicitAnimation.ease, false, implicitAnimation.flags);
        }
        if(nrdp.gibbon._debugAnimations)
            nrdp.log.error("SetAnimatedValue: " + this._id + " -> " + JSON.stringify(value));
        this._animations.pending[property] = undefined;
        GIBBON_PUSH_APPEND(property, value, value);
        return undefined;
    },
    _canAnimate: function _canAnimate() {
        if(!this.parent && !this._isRoot)
            return false;
        for(var p = this; p; p = p.parent) {
            if(!p.visible)
                return false;
        }
        return true;
    },
    _findAnimation: function _findAnimation(property) {
        var animations = this._animations;
        if(animations[property] && animations[property].duration)
            return animations[property];
        return false;
    },
    stopAnimation: function stopAnimation(property, end) {
        if(end === undefined)
            end = true;
        nrdp.gibbon._invoke(this._path, "stopAnimation", { property: property, end: end });
    },
    startAnimation: function startAnimation(property, start, end, duration, ease, append, flags) {
        if(!duration && !append) { //Nonsense, duration 0 is non-animated
 #ifdef BUILD_DEBUG
            nrdp.log.warn("StartAnimation(" + property + "): With 0 duration for WIDGET(" + this._id + ")!");
 #endif
            this._setAnimatedValue(property, { value: end });
            return undefined;
        }

        var value = nrdp.gibbon._createAnimation(property, start, end, duration, ease, append, flags);
        if(start === undefined)
            value.animate.previous = this[property];
        if(nrdp.gibbon._debugAnimations)
            nrdp.log.error('AnimationStarted:' + this._id + ':' + JSON.stringify(start) + ":" + value.animate.id + ":" + property + " == " + JSON.stringify(value));
        this._animations.pending[property] = value.animate.id;
        GIBBON_PUSH_APPEND(property, value, value);
        return value.animate.id;
    },
    animate: function animate(property, duration, ease, flags) {
        if(!this._animations[property]) {
            if(!duration)
                return;
        } else if(this._animations[property].duration == duration &&
                  this._animations[property].ease == ease) {
            return;
        }
        this._animations[property] = { duration: duration, ease: ease, flags: flags };
    },
    _updateProperty: function _updateProperty(property, value) {
        //nrdp.log.error("Updated " + this._path + "::" + property);
        this._syncData[property] = value;

        if(property.lastIndexOf("render", 0) == 0 && nrdp._hasEventListener(this, "renderpropertychange")) {
            var renderProperty;
            if(property == "renderX")
                renderProperty = "x";
            else if(property == "renderY")
                renderProperty = "y";
            else if(property == "renderWidth")
                renderProperty = "width";
            else if(property == "renderHeight")
                renderProperty = "height";
            if(renderProperty) {
                this._sendRenderProperty(property, renderProperty, value);
                return;
            }
            nrdp.log.error("Unhandled widget.renderProperty: " + property);
        }
        if (nrdp.gibbon._breaks !== undefined)
            this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    _maybeBreakSubtree: function _maybeBreakSubtree(obj) {
        if (obj === undefined)
            return;
        _maybeBreakSubtree(obj.parent);
        if (nrdp.gibbon._breaks[obj._id] && nrdp.gibbon._breaks[obj._id][nrdp.gibbon.DEBUGGER_SUBTREE_MODIFIED]) {
            nrdp.gibbon._breaks[obj._id][nrdp.gibbon.DEBUGGER_SUBTREE_MODIFIED]();
        }
    },

    _maybeBreak: function _maybeBreak(type) {
        if (nrdp.gibbon._breaks[this._id] && nrdp.gibbon._breaks[this._id][type]) {
            nrdp.gibbon._breaks[this._id][type]();
        }
        this._maybeBreakSubtree(this.parent);
    },

    _handleEvent: function _handleEvent(event) {
        var handled = true;
        if(event.name == "animationFinished") {
            event.data.property = nrdp._classes[this.classname].properties.byIndex[event.data.propertyID];
            var property = event.data.property;
            if(nrdp.gibbon._debugAnimations)
                nrdp.log.error((event.data.aborted ? "AnimationAborted:" : "AnimationFinished:") +
                               this._id + ':' + event.data.id + ":" + property + " == " + event.data.value +
                               (this._animations.pending[property] == event.data.id ? "*" : ""));
            if(event.data.id && this._animations.pending[property] == event.data.id) {
                if (nrdp.gibbon._breaks !== undefined)
                    this._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
                this._syncData[property] = event.data.value;
            }
            var evt = { type: event.name, data: event.data };
            nrdp._callEventListeners(this, evt);
        } else if(event.name == "grab" || event.name == "dump") {
            this._callCallback(event);
        } else {
            handled = false;
        }
        return handled;
    },
    _registerCallback: function _registerCallback(cb) {
        var id = nrdp.gibbon._setValue(cb);
        return id;
    },
    _callCallback: function _callCallback(event) {
        if(event.data.id === undefined)
            return;
        var cb = nrdp.gibbon._getValue(event.data.id);
        nrdp.gibbon._deleteValue(event.data.id);
        if(cb)
            cb(event.data.data);
    }
};
