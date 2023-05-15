/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp, GibbonSyncObject*/

nrdp.gibbon.Text = function(widget) {
    this._object = new GibbonSyncObject(this, widget._id, nrdp.gibbon._sync_Text);
    this._widget = widget;
    this._path = widget._path + ".text";
    if(!this._syncData)
        this._syncData = widget._syncData.text = {};
    this._defaults = nrdp.gibbon.Text._default;
    this._properties = nrdp._classes[this.classname].properties.byName;
};

nrdp.gibbon.Text._default = {
    size: 30,
    lineHeight: undefined,
    sendTextLoaded: false,
    sendRenderProperties: false,
    weight: "bold",
    variant: "normal",
    align: nrdp.gibbon.ALIGN_LEFT,
    color: { r: 0, g: 0, b: 0, a: 255 },
    backgroundColor: { r: 0, g: 0, b: 0, a: 0 },
    padding: 0,
    indent: 0,
    shadow: {
        offsetX: 0,
        offsetY: 0,
        color: { r: 0, g: 0, b: 0, a: 255 }
    },
    truncation: {
        position: "none",
        ellipsis: "…"
    },
    edgeEffect: {
        type: "none",
        width: 1,
        lightColor: { r: 0, g: 0, b: 0, a: 255 },
        darkColor: { r: 0, g: 0, b: 0, a: 255 },
        outlineColor: { r: 0, g: 0, b: 0, a: 255 }
    },
    cursor: {
        visible: false,
        style: "line",
        color: {r: 0, g: 0, b: 0, a: 255 },
        interval: 500,
        width: 1
    },
    typography: {
        kerning: true,
        tracking: 0,
        fontFeatures: ""
    },
    wrap: false,
    underline: false,
    strikethrough: false,
    richText: false,

    text: undefined,

    firstLine: 0,
    cursorPosition: undefined,

    renderFirstLine: 0
};

nrdp.gibbon.Text.prototype = {
    classname: "TextBridge",
    constructor: nrdp.gibbon.Text,
    toString: function toString() {
        var contents = GIBBON_PULL("contents");
        if(contents === undefined)
            return "";
        return contents;
    },

    addEventListener: function addEventListener(evt, listener) {
        if(nrdp._addEventListener(this, evt, listener)) {
            if(evt == "renderpropertychange")
                this.sendRenderProperties = true;
            else if(evt == "textLoaded")
                this.sendTextLoaded = true;
        }
    },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    _mergeValues: function _mergeValues(newObject, oldObject, values) {
        var mergeObject = {};
        for(var v in values) {
            var valueName = values[v];
            var newValue;
            if(newObject.hasOwnProperty(valueName)) {
                newValue = newObject[valueName];
                if(valueName === "color")
                    newValue = { r: newValue.r, g: newValue.g, b: newValue.b, a: (newValue.a === undefined ? 255 : newValue.a) };
                if(valueName === "backgroundColor")
                    newValue = { r: newValue.r, g: newValue.g, b: newValue.b, a: (newValue.a === undefined ? 255 : newValue.a) };
            } else {
                newValue = oldObject[valueName];
            }
            mergeObject[valueName] = newValue;

        }
        return mergeObject;
    },

    _sendRenderProperty: function _sendRenderProperty(property, renderProperty, value) {
        //nrdp.log.error("EVENT: " + evt.type + " - p=" + evt.property + ", rP=" + evt.renderProperty + ", v=" + evt.value);
        nrdp._callEventListeners(this, { type: "renderpropertychange",
                                         property: property,
                                         renderProperty: renderProperty,
                                         value: value });
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },
    _renderProperty: function _renderProperty(property) {
        if(!this.sendRenderProperties) {
            nrdp.log.error("RenderProperty(" + property + "): Used but TEXT(" + this._widget._id + ") has not set sendRenderProperties!");
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

    get sendTextLoaded() { return GIBBON_PULL("sendTextLoaded"); },
    set sendTextLoaded(_sendTextLoaded) {
        if(_sendTextLoaded == this.sendTextLoaded)
            return;
        GIBBON_PUSH("sendTextLoaded", _sendTextLoaded, _sendTextLoaded);
    },

    get renderLines() { return this._renderProperty("renderLines"); },
    get renderTextLines() { return this.renderLines; }, //compat
    get renderFirstLine() { return this._renderProperty("renderFirstLine"); },
    get renderPadding() { return this._renderProperty("renderPadding"); },
    get renderTextFirstLine() { return this.renderFirstLine; }, //compat
    get renderCursorPosition() { return this._renderProperty("renderCursorPosition"); },
    get renderTextCursorPosition() { return this.renderCursorPosition; }, //compat

    get contents() { return GIBBON_PULL("contents"); },
    set contents(_contents) {
        if(_contents === null || typeof _contents === "undefined")
            _contents = "";
        if(_contents == this.contents)
            return;
        GIBBON_PUSH("contents", _contents, _contents);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get firstLine() { return GIBBON_PULL("firstLine"); },
    set firstLine(_firstLine) {
        if(_firstLine == this.firstLine)
            return;
        GIBBON_PUSH("firstLine", _firstLine, _firstLine);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get cursorPosition() { return GIBBON_PULL("cursorPosition"); },
    set cursorPosition(_cursorPosition) {
        if(_cursorPosition == this.cursorPosition)
            return;
        GIBBON_PUSH("cursorPosition", _cursorPosition, _cursorPosition);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get family() { return GIBBON_PULL("family"); },
    set family(_family) {
        if(_family == this.family)
            return;
        GIBBON_PUSH("family", _family, _family);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get locale() { return GIBBON_PULL("locale"); },
    set locale(_locale) {
        if(_locale == this.locale)
            return;
        GIBBON_PUSH("locale", _locale, _locale);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get direction() { return GIBBON_PULL("direction"); },
    set direction(_direction) {
        if(_direction == this.direction)
            return;
        GIBBON_PUSH("direction", _direction, _direction);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get script() { return GIBBON_PULL("script"); },
    set script(_script) {
        if(_script == this.script)
            return;
        GIBBON_PUSH("script", _script, _script);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get size() { return GIBBON_PULL("size"); },
    set size(_size) {
        if(typeof _size != "number")
            _size = this._defaults.size;
        if(_size == this.size && nrdp.gibbon.scene.scale == 1.0)
            return;
        GIBBON_PUSH("size", _size, _size);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get padding() { return GIBBON_PULL("padding"); },
    set padding(_padding) {
        if (_padding instanceof Array) {
            if (_padding.length > 2) {
                _padding = this._defaults.padding;
            } else {
                for (var i = 0; i < _padding.length; ++i) {
                    if (typeof _padding[i] != "number" && typeof _padding[i] != "undefined") {
                        _padding = this._defaults.padding;
                        break;
                    }
                }
            }
        } else if (typeof _padding != "number" && typeof _padding != "undefined") {
            if (typeof _padding != "object")
                _padding = this._defaults.padding;
        }
        if (_padding == this.padding)
            return;
        GIBBON_PUSH("padding", _padding, _padding);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get indent() { return GIBBON_PULL("indent"); },
    set indent(_indent) {
        if(_indent == this.indent)
            return;
        GIBBON_PUSH("indent", _indent, _indent);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get lineHeight() { return GIBBON_PULL("lineHeight"); },
    set lineHeight(_lineHeight) {
        if(_lineHeight == this.lineHeight)
            return;
        GIBBON_PUSH("lineHeight", _lineHeight, _lineHeight);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get wrap() { return GIBBON_PULL("wrap"); },
    set wrap(_wrap) {
        if(_wrap == this.wrap)
            return;
        GIBBON_PUSH("wrap", _wrap, _wrap);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get richText() { return GIBBON_PULL("richText"); },
    set richText(_richText) {
        if(_richText == this.richText)
            return;
        GIBBON_PUSH("richText", _richText, _richText);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get maxLines() { return GIBBON_PULL("maxLines"); },
    set maxLines(_maxLines) {
        if(_maxLines == this.maxLines)
            return;
        GIBBON_PUSH("maxLines", _maxLines, _maxLines);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get underline() { return GIBBON_PULL("underline"); },
    set underline(_underline) {
        if(_underline == this.underline)
            return;
        GIBBON_PUSH("underline", _underline, _underline);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get strikethrough() { return GIBBON_PULL("strikethrough"); },
    set strikethrough(_strikethrough) {
        if(_strikethrough == this.strikethrough)
            return;
        GIBBON_PUSH("strikethrough", _strikethrough, _strikethrough);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get bold() { return this.weight == "bold"; },
    set bold(_bold) { this.weight = _bold ? "bold" : "normal"; },
    get weight() { return GIBBON_PULL("weight"); },
    set weight(_weight) {
        if(_weight == this.weight)
            return;
        GIBBON_PUSH("weight", _weight, _weight);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get align() { return GIBBON_PULL("align"); },
    set align(_align) {
        _align = nrdp.gibbon.Widget._encodeAlignment(_align);
        if(_align == this.align)
            return;
        GIBBON_PUSH("align", _align, _align);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get italic() { return this.variant == "italic"; },
    set italic(_italic) { this.variant = _italic ? "italic" : "normal"; },
    get variant() { return GIBBON_PULL("variant"); },
    set variant(_variant) {
        if(_variant == this.variant)
            return;
        GIBBON_PUSH("variant", _variant, _variant);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get color() { return GIBBON_PROXY_PULL("color"); },
    set color(_color_in) {
        var _color = this.color;
        if(nrdp.gibbon._compareColor(_color_in, _color))
            return;
        var value = { r: _color_in.r, g: _color_in.g, b: _color_in.b, a: _color_in.a };
        GIBBON_PUSH("color", value, nrdp.gibbon._encodeColor(_color_in));
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get backgroundColor() { return GIBBON_PROXY_PULL("backgroundColor"); },
    set backgroundColor(_color_in) {
        var _color = this.backgroundColor;
        if(nrdp.gibbon._compareColor(_color_in, _color))
            return;
        var value = { r: _color_in.r, g: _color_in.g, b: _color_in.b, a: _color_in.a };
        GIBBON_PUSH("backgroundColor", value, nrdp.gibbon._encodeColor(_color_in));
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get cursor() { return GIBBON_PROXY_PULL("cursor"); },
    set cursor(_cursor_in) {
        var _cursor = GIBBON_PULL("cursor");
        if(!_cursor_in && !_cursor)
            return;
        if(_cursor && _cursor_in &&
           _cursor.style == _cursor_in.style &&
           _cursor.width == _cursor_in.width &&
           _cursor.interval == _cursor_in.interval &&
           nrdp.gibbon._compareColor(_cursor.color, _cursor_in.color) &&
           _cursor.visible == _cursor_in.visible)
            return;
        var value = this._mergeValues(_cursor_in ? _cursor_in : this._defaults.cursor, _cursor,
                                      ["style", "width", "visible", "interval", "color"]);
        GIBBON_PUSH("cursor", value, ([value.style, value.visible, value.interval, value.width, nrdp.gibbon._encodeColor(value.color)]));
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get edgeEffect() { return GIBBON_PROXY_PULL("edgeEffect"); },
    set edgeEffect(_edgeEffect_in) {
        var _edgeEffect = GIBBON_PULL("edgeEffect");
        if(!_edgeEffect_in && !_edgeEffect)
            return;
        var value = this._mergeValues(_edgeEffect_in ? _edgeEffect_in : this._defaults.edgeEffect, _edgeEffect,
                                      ["type", "width", "color1", "color2", "lightColor", "darkColor", "outlineColor"]);
        if(value.type == "raised" || value.type == "depressed") {
            if(_edgeEffect_in.lightColor instanceof Object) {
                value.lightColor = _edgeEffect_in.lightColor;
                value.color1 = {
                    r: _edgeEffect_in.lightColor.r,
                    g: _edgeEffect_in.lightColor.g,
                    b: _edgeEffect_in.lightColor.b,
                    a: _edgeEffect_in.lightColor.a
                };
            }
            if(_edgeEffect_in.darkColor instanceof Object) {
                value.darkColor = _edgeEffect_in.darkColor;
                value.color2 = {
                    r: _edgeEffect_in.darkColor.r,
                    g: _edgeEffect_in.darkColor.g,
                    b: _edgeEffect_in.darkColor.b,
                    a: _edgeEffect_in.darkColor.a
                };
            }
        } else if(value.type == "outline") {
            if(_edgeEffect_in.outlineColor instanceof Object) {
                value.outlineColor = _edgeEffect_in.outlineColor;
                value.color1 = {
                    r: _edgeEffect_in.outlineColor.r,
                    g: _edgeEffect_in.outlineColor.g,
                    b: _edgeEffect_in.outlineColor.b,
                    a: _edgeEffect_in.outlineColor.a
                };
            }
        }

        if(_edgeEffect && _edgeEffect.type == value.type && _edgeEffect.width == value.width &&
           nrdp.gibbon._compareColor(_edgeEffect.color1, value.color1) &&
           nrdp.gibbon._compareColor(_edgeEffect.color2, value.color2))
            return;

        GIBBON_PUSH("edgeEffect", value, ([value.type, value.width, nrdp.gibbon._encodeColor(value.color1), nrdp.gibbon._encodeColor(value.color2)]));
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get shadow() { return GIBBON_PROXY_PULL("shadow"); },
    set shadow(_shadow_in) {
        var _shadow = GIBBON_PULL("shadow");
        if(!_shadow_in && !_shadow)
            return;
        if(_shadow && _shadow_in && _shadow.color &&
           _shadow.offsetX == _shadow_in.offsetX &&
           _shadow.offsetY == _shadow_in.offsetY &&
           nrdp.gibbon._compareColor(_shadow.color, _shadow_in.color))
            return;
        var value = this._mergeValues(_shadow_in ? _shadow_in : this._defaults.shadow, _shadow,
                                      ["offsetX", "offsetY", "color"]);
        GIBBON_PUSH("shadow", value, ([value.offsetX, value.offsetY, nrdp.gibbon._encodeColor(value.color)]));
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get truncation() { return GIBBON_PROXY_PULL("truncation"); },
    set truncation(_truncation_in) {
        var _truncation = GIBBON_PULL("truncation");
        if(!_truncation_in && !_truncation)
            return;
        if(_truncation && _truncation_in &&
           _truncation.position == _truncation_in.position && _truncation.ellipsis == _truncation_in.ellipsis)
            return;
        var value = this._mergeValues(_truncation_in ? _truncation_in : this._defaults.truncation, _truncation,
                                      ["position", "ellipsis"]);
        GIBBON_PUSH("truncation", value, ([value.position, value.ellipsis]));
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    get typography() { return GIBBON_PROXY_PULL("typography"); },
    set typography(_typography_in) {
        var _typography = GIBBON_PULL("typography");
        if(!_typography_in && !_typography)
            return;
        if(_typography && _typography_in &&
           _typography.kerning == _typography_in.kerning && _typography.tracking == _typography_in.tracking &&
           _typography.fontFeatures == _typography_in.fontFeatures)
            return;
        var value = this._mergeValues(_typography_in ? _typography_in : this._defaults.typography, _typography,
                                      ["kerning", "tracking", "fontFeatures"]);
        GIBBON_PUSH("typography", value, value);
        if (nrdp.gibbon._breaks !== undefined)
            this._widget._maybeBreak(nrdp.gibbon.DEBUGGER_ATTRIBUTE_MODIFIED);
    },

    info: function info(cb) {
        var id = nrdp.gibbon._setValue(cb);
        nrdp.gibbon._invoke(this._path, "textInfo", {id: id});
    },

    _updateProperty: function _updateProperty(property, value) {
        //nrdp.log.error("Updated " + this._path + "::" + property + " " + nrdp._hasEventListener(this._widget, "renderpropertychange") + " " + nrdp._hasEventListener(this, "renderpropertychange"));
        this._syncData[property] = value;

        if(property.lastIndexOf("render", 0) == 0 &&
           (nrdp._hasEventListener(this._widget, "renderpropertychange") ||
            nrdp._hasEventListener(this, "renderpropertychange"))) {
            var renderProperty, widgetProperty;
            if(property == "renderCursorPosition") {
                renderProperty = "cursorPosition";
                widgetProperty = { property: "renderTextCursorPosition", renderProperty: "textCursorPosition" };
            } else if(property == "renderLines") {
                renderProperty = property;
                widgetProperty = { property: "renderTextLines", renderProperty: "renderTextLines" };
            } else if(property == "renderFirstLine") {
                renderProperty = "firstLine";
                widgetProperty = { property: "renderTextFirstLine", renderProperty: "textFirstLine" };
            } else if(property == "renderPadding") {
                renderProperty = property;
                widgetProperty = { property: "renderPadding", renderProperty: "renderPadding" };
            }
            if(renderProperty) {
                this._sendRenderProperty(property, renderProperty, value);
                if(widgetProperty && this._widget.sendRenderProperties)
                    this._widget._sendRenderProperty(widgetProperty.property, widgetProperty.renderProperty, value);
                return;
            } else {
                nrdp.log.error("Unhandled text.renderProperty: " + property);
            }
        }
    },

    _handleEvent: function _handleEvent(event) {
        var handled = true;
        if(event.name == "textLoaded") {
            var evt = { type: event.name, data: event.data };
            nrdp._callEventListeners(this, evt);
        } else if(event.name == "textInfo") {
            var cb = nrdp.gibbon._getValue(event.data.id);
            if (cb)
                cb(event.data.info);
        } else {
            handled = false;
        }
        return handled;
    }
};
