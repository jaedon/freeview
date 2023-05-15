
// @depend ../preamble.js

// For those JavaScript engines (Qt 4.6) that lack support for
// Array.indexOf, we use this implementation from
// https://developer.mozilla.org/En/Core_JavaScript_1.5_Reference/Objects/Array/IndexOf#Compatibility
if (!Array.prototype.indexOf)
{
    Array.prototype.indexOf = function(elt /*, from*/)
    {
        var len = this.length >>> 0;
    
        var from = Number(arguments[1]) || 0;
        from = (from < 0) ? Math.ceil(from) : Math.floor(from);
        if (from < 0)
            from += len;

        for (; from < len; from++)
        {
            if (from in this &&
                this[from] === elt)
            return from;
        }
        return -1;
    };
}



gibbon.defaultTextStyle = function () {
    var t = this;

//    t.family = "Arial";
    t.size = 30;
    t.align = "left";
    t.weight = "bold";
    t.color = new gibbon.color();

    t.merge = function (newStyle) {
        for (var k in newStyle) {
            if (newStyle.hasOwnProperty(k)) {
                this[k] = newStyle[k];
            }
        }
        // special case color, since we need to construct a new object
        if (newStyle.hasOwnProperty("color")) {
            this.color = new gibbon.color(newStyle.color);
        }
    };
};

gibbon.rect = function (x, y, w, h) {
    var t = this;

    if (typeof(x) == "object") {
        t.x = x.x;
        t.y = x.y;
        t.width = x.width;
        t.height = x.height;
    } else {
        t.x = x;
        t.y = y;
        t.width = w;
        t.height = h;
    }
};

gibbon.rect.prototype = {
    constructor: gibbon.rect,

    get left() { return this.x; },
    set left(tx) {
        var t = this;
        t.width += t.x - tx;
        t.x = tx;
    },

    get top() { return this.y; },
    set top(ty) {
        var t = this;
        t.height += t.y - ty;
        t.y = ty;
    },

    get right() {
        return this.x + this.width;
    },
    set right(rt) {
        this.width = rt - this.x;
    },

    get bottom() {
        return this.y + this.height;
    },
    set bottom(bt) {
        this.height = bt - this.y;
    },

    clear: function () {
        var t = this;
        t.x = 0;
        t.y = 0;
        t.width = 0;
        t.height = 0;
    },

    empty: function () {
        return this.width <= 0 || this.height <= 0;
    },

    overlaps: function (r) {
        var t = this;
        return !(t.x > r.right || t.y > r.bottom || t.right < r.x || t.bottom < r.y);
    },

    unite: function (r) {
        var t = this;

        if (t.x > r.x) t.left = r.x;
        if (t.y > r.y) t.top = r.y;
        if (t.right < r.right) t.right = r.right;
        if (t.bottom < r.bottom) t.bottom = r.bottom;
    },

    intersect: function (r) {
        var t = this, sectRect;

        if (t.overlaps(r)) {
            sectRect = new gibbon.rect(t.x > r.x ? t.x : r.x, t.y > r.y ? t.y : r.y, 0, 0);
            sectRect.right = t.right < r.right ? t.right : r.right;
            sectRect.bottom = t.bottom < r.bottom ? t.bottom : r.bottom;
            return sectRect;
        }

        return new gibbon.rect;
    },
};

gibbon.color = function(color) {
    var t = this;

    t.a = 255;

    if (color === undefined) {
        t.r = 0;
        t.g = 0;
        t.b = 0;
    } else if (typeof color === "object") {
        t.r = color.r;
        t.g = color.g;
        t.b = color.b;
        if (color.a !== undefined) {
            t.a = color.a;
        }
    } else if (typeof color === "string") {
        t.parseString(color);
    }
};

// Color parsing extracted and adapted from:
//      TinyColor.js - <https://github.com/bgrins/TinyColor> - 2011 Brian Grinstead (http://briangrinstead.com) - v0.5
//      License: https://github.com/bgrins/TinyColor/blob/master/LICENSE

gibbon.color.prototype = {
    constructor: gibbon.color,

    parseString: function (color) {
        var t = this,
            regex = gibbon.color.regex,
            match;

        color = color.replace(regex.trimLeft,'').replace(regex.trimRight, '').toLowerCase();
        if ((match = regex.hex6.exec(color))) {
            t.a = 255;
            t.r = parseInt(match[1], 16);
            t.g = parseInt(match[2], 16);
            t.b = parseInt(match[3], 16);
            return true;
        }
        if ((match = regex.hex3.exec(color))) {
            t.a = 255;
            t.r = parseInt(match[1] + '' + match[1], 16);
            t.g = parseInt(match[2] + '' + match[2], 16);
            t.b = parseInt(match[3] + '' + match[3], 16);
            return true;
        }
        if ((match = regex.rgb.exec(color))) {
            t.a = 255;
            t.r = match[1];
            t.g = match[2];
            t.b = match[3];
            return true;
        }
        if ((match = regex.rgba.exec(color))) {
            t.r = match[1];
            t.g = match[2];
            t.b = match[3];
            t.a = match[4];
            return true;
        }
        if (color == 'transparent') {
            t.r = 0;
            t.g = 0;
            t.b = 0;
            return true;
        }

        return false;
    },
};

gibbon.color.regex = (function() {

    // <http://www.w3.org/TR/css3-values/#integers>
    var CSS_INTEGER = "[-\\+]?\\d+%?"; 
    
    // <http://www.w3.org/TR/css3-values/#number-value>
    var CSS_NUMBER = "[-\\+]?\\d*\\.\\d+%?"; 
    
    // Allow positive/negative integer/number.  Don't capture the either/or, just the entire outcome.
    var CSS_UNIT = "(?:" + CSS_NUMBER + ")|(?:" + CSS_INTEGER + ")"; 
    
    // Actual matching.  
    // Parentheses and commas are optional, but not required.  
    // Whitespace can take the place of commas or opening paren
    var PERMISSIVE_MATCH3 = "[\\s|\\(]+(" + CSS_UNIT + ")[,|\\s]+(" + CSS_UNIT + ")[,|\\s]+(" + CSS_UNIT + ")\\s*\\)?";
    var PERMISSIVE_MATCH4 = "[\\s|\\(]+(" + CSS_UNIT + ")[,|\\s]+(" + CSS_UNIT + ")[,|\\s]+(" + CSS_UNIT + ")[,|\\s]+(" + CSS_UNIT + ")\\s*\\)?";
    
    return {
        rgb: new RegExp("rgb" + PERMISSIVE_MATCH3),
        rgba: new RegExp("rgba" + PERMISSIVE_MATCH4),
        hex3: /^([0-9a-fA-F]{1})([0-9a-fA-F]{1})([0-9a-fA-F]{1})$/,
        hex6: /^([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})$/,
        trimLeft: /^[\s,#]+/, 
        trimRight: /\s+$/,
    };
})();







(function () {

    var styleSheetEntry = function () {
        this._styles = [];
        this._parentSelectorMap = {};
    };

    styleSheetEntry.prototype = {
        constructor: styleSheetEntry,

        addProperties: function (selector, parentSelector, props) {
            var style = { _parentSelector: undefined },
                p;

            if (parentSelector !== undefined) {
                style._parentSelector = parentSelector;
                this._parentSelectorMap[parentSelector] = true;
            }

            // make a copy
            for (p in props) {
                if (props.hasOwnProperty(p)) {
                    style[p] = props[p];
                }
            }

            this._styles.push(style);
        },

        select: function (destStyles, styleObject) {
            var styles = this._styles, s, i, n;

            // walk through the styles and copy properties from those that match
            for (i = 0, n = styles.length; i < n; i++) {
                s = styles[i];

                if (s._parentSelector !== undefined) {
                    styleObject._hasParentStyleReference = true;
                    if (!styleObject._target.parentHasClass(s._parentSelector)) {
                        // not a match, skip it
                        continue;
                    }
                }

                // match! copy the props to the dest
                for (k in s) {
                    if (s.hasOwnProperty(k)) {
                        destStyles[k] = s[k];
                    }
                }
            }

            return s;
        },
    };

    var styleSheetClass = function () {
        this._styleSheet = {};
    };

    styleSheetClass.prototype = {
        constructor: styleSheetClass,

        addStyles: function (markup) {
            var t = this,
                ss = t._styleSheet,
                styleKey;

            for (styleKey in markup) {
                if (markup.hasOwnProperty(styleKey)) {
                    t._addStyleEntry(styleKey, markup[styleKey]);
                }
            }
        },

        _addStyleEntry: function (selector, markup) {
            var ss = this._styleSheet,
                styleEntry,
                prop,
                offset,
                parentSelector, rootSelector;

            // parse selector
            offset = selector.indexOf(">");
            if (offset > 0) {
                rootSelector = selector.substr(offset + 1);
                parentSelector = selector.substr(0, offset);
            } else {
                rootSelector = selector
            }

            styleEntry = ss[rootSelector];
            if (styleEntry === undefined) {
                styleEntry = new styleSheetEntry();
            }

            styleEntry.addProperties(rootSelector, parentSelector, markup);
            ss[rootSelector] = styleEntry;
        },

        _selectStyle: function (destStyles, className, styleObject) {
            if (className.length > 0 ) {
                var style = this._styleSheet[className];
                if (style !== undefined) {
                    return style.select(destStyles, styleObject);
                }
            }
        },

        // for each class
            // select appropriate style
                // if just simple selector style, we're done
                // otherwise call selector (could mean walk up the tree to find parent class)
            // apply style's values
        computeStyles: function (styleObject) {
            var t = this,
                newStyles = styleObject.computedStyles,
                classArray = styleObject.className.split(" "),
                className, i, n,
                changed = false
                ;

            for (i = 0, n = classArray.length; i < n; i++) {
                className = classArray[i];
                if (className.length > 0) {
                    if (t._selectStyle(newStyles, className, styleObject)) {
                        changed = true;
                    }
                }
            }

            styleObject.computedStyles = newStyles;

            return changed;
        },
    };


    var idCounter = 982734;
    var styleClass = function (target) {
        var t = this;
        t._id = "style: " + (idCounter++);
        t._classString = "";
        t._computedStyles = {};
        t._classNameMap = {};
        t._target = target;

        // does this style refer to any style sheet entries
        // that 'parent' selectors
        t._hasParentStyleReference = false;
    };

    styleClass.prototype = {
        constructor: styleClass,

        hasClass: function (classString) {
            return this._classNameMap.hasOwnProperty(classString);
        },

        addClass: function (classString) {
            var t = this;
            if (!t.hasClass(classString)) {
                t.className = t._classString.length ? t._classString + " " + classString : classString;
            }
        },

        addClasses: function (classStringArray) {
            var t = this, i, n,
                newClassName = t._classString,
                newClassArray, newClass;

            if (newClassName.length) {
                newClassArray = newClassName.split(" ");
                for (i = 0, n = classStringArray.length; i < n; i++) {
                    newClass = classStringArray[i];
                    if (!t.hasClass(newClass)) {
                        newClassArray.push(newClass);
                    }
                }

                t.className = newClassArray.join(" ");
            }
            else {
                t.className = classStringArray.join(" ");
            }
        },

        removeClass: function (classString) {
            var origName = this._classString,
                name = origName.replace(this._regExp(classString), "$1"),
                last = name.length - 1;
            this.className = name[last] === " " ? name.substr(0, last) : name;
        },

        removeClasses: function (classArray) {
            var newClassName = this._classString, classString,
                name, last,
                i, n;

            for (i = 0, n = classArray.length; i < n; i++) {
                classString = classArray[i];
                name = newClassName.replace(this._regExp(classString), "$1");
                last = name.length - 1;
                newClassName = name[last] === " " ? name.substr(0, last) : name;
            }

            this.className = newClassName;
        },

        get className() {
            return this._classString;
        },

        set className(classString) {
            var t = this;

            if (classString === t._classString) {
                return;
            }

            t._classString = classString;

            t.computeStyles();
        },

        computeStyles: function () {
            var t = this,
                classString = t._classString,
                classArray = classString.split(" "),
                classMap = {},
//                 changedClasses = [],
                i, n, c, k
                ;

            for (i = 0, n = classArray.length; i < n; i++) {
                c = classArray[i];
                classMap[c] = 1;

                // if new class, mark as changed
//                 if (!t.hasClass(c)) {
//                     changedClasses.push(c);
//                 }
            }

            // mark removed classes as changed
//             for (k in t._classNameMap) {
//                 if (t._classNameMap.hasOwnProperty(k) && !classMap.hasOwnProperty(k)) {
//                     changedClasses.push(k);
//                 }
//             }

            t._classNameMap = classMap;

            t.resetComputedStyles();
            gibbon.style.computeStyles(t);
            t._target.onStyleChange(t);
        },

        resetComputedStyles: function () {
            this._computedStyles = {};
        },

        get computedStyles() {
            return this._computedStyles;
        },

        set computedStyles(cs) {
            this._computedStyles = cs;
        },

        _regExp: function(className) {
            return new RegExp("(^|\\s+)" + className + "(?:$|\\s+)", "g");
        },
    };


    gibbon.style = {
        _styleClass: styleClass,
        _styleSheetClass: styleSheetClass,
        _styleSheets: [],

        createStyle: function (target) {
            return new this._styleClass(target);
        },

        createStyleSheet: function () {
            return new this._styleSheetClass();
        },

        addStyleSheet: function (ss) {
            this._styleSheets.push(ss);
        },
        
        computeStyles: function (styleObject) {
            var ss = this._styleSheets,
                i, n,
                changed = false
                ;

            for (i = 0, n = ss.length; i < n; i++) {
                changed = ss[i].computeStyles(styleObject) || changed;
            }

            return changed;
        },
    };

})();


// @depend style.js

gibbon.scene.start = function (markup) {
    if (!markup.hasOwnProperty("scene")) {
        throw new Error("gibbon.scene.start: no scene specified in markup");
    }

    if (markup.hasOwnProperty("styles")) {
        this._styleSheet.addStyles(markup.styles);
    }

    gibbon.scene.widgets = gibbon.createWidgets(markup.scene);

    gibbon.setScene(gibbon.scene.widgets.proxy);

    if (gibbon.inspector !== undefined) {
        gibbon.inspector.reset();
        gibbon.inspector.update();
    }
};

gibbon.scene._styleSheet = gibbon.style.createStyleSheet();
gibbon.style.addStyleSheet(gibbon.scene._styleSheet);

gibbon.getElementById = function (id) {
    if (gibbon.scene && gibbon.scene.widgets) {
        return gibbon.scene.widgets.getElementById(id);
    }

    return undefined;
};

gibbon.getElementsByClassName = function (className) {
    if (gibbon.scene && gibbon.scene.widgets) {
        return gibbon.scene.widgets.getElementsByClassName(className);
    }

    return [];
};


(function () {

    gibbon.layout = {};
    var defaultMargin = { left: 0, top: 0, right: 0, bottom: 0 };
    
    gibbon.layout.basic = function (markup) {
        this.layout = function (widget) {
            var wa = widget.children;
            var pm = defaultMargin;
    
            for (var i = 0, n = wa.length; i < n; ++i)
            {
                var w = wa[i];
                var m = w.margin === undefined ? defaultMargin : w.margin;
    
                w.x = m.left;
                w.y =  m.top;
            }
        };
    };
    
    // layout the child widgets flowing like text down the page
    // alignH: justify the lines of widgets to the left, center, or right
    // alignV: align the widgets top, bottom, or center of the line in which they reside
    // the line height is determined by the tallest widget in that line.
    // FIXME: alignV should affect the entire block of widgets as a whole, not each
    // individual line.
    gibbon.layout.flow = function (markup)
    {
        var val = markup.alignH;
        this.alignH = val === undefined ? "left" : val; // "left", "center", or "right"
    
        val = markup.alignV;
        this.alignV = val === undefined ? "bottom" : val; // "top", "center", or "bottom"
    
        val = markup.lineSpacing;
        this.lineSpacing = val === undefined ? val : 0;
    
        this.layoutVertical = function (widget)
        {
            var wa = widget.children;
            var maxLineHeight = widget.height;
            var lineWidth = 0;
            var lineHeight = 0;
            var lineStartIndex = 0;
            var wx = 0;
            var i = 0;
            var pm = defaultMargin;
    
            for (var n = wa.length; i < n; )
            {
                var w = wa[i];
    
                var m = w.margin === undefined ? defaultMargin : w.margin;
                var height = m.top + w.getPropertyFinal("height") + m.bottom;
    
                if ((lineHeight + height) <= maxLineHeight)
                {
                    lineHeight += height;
                    var width = w.getPropertyFinal("width") + m.left + m.right;
                    if (width > lineWidth)
                    {
                        lineWidth = width;
                    }
                    ++i;
                }
                else
                {
                    this.layoutLineVertical(lineStartIndex, i, widget, wx, lineWidth, lineHeight, maxLineHeight);
    
                    wx += lineWidth;
    
                    lineWidth = 0;
                    lineHeight = 0;
                    lineStartIndex = i;
                }
                pm = m;
            }
    
            this.layoutLineVertical(lineStartIndex, i, widget, wx, lineWidth, lineHeight, maxLineHeight);
        };
    
        this.layoutLineVertical = function (lineStartIndex, n, widget, lineLeft, lineWidth, lineHeight, maxLineHeight)
        {
            var wa = widget.children;
            var wy = 0;
            var widgetWidth = widget.width;
            var pm = defaultMargin;
    
            switch (this.alignV)
            {
                case "center":
                    wy += (maxLineHeight - lineHeight) / 2;
                    break;
                case "bottom":
                    wy += maxLineHeight - lineHeight;
                    break;
                default:
                    break;
            }
    
            var alignH = this.alignH;
            for (var j = lineStartIndex; j < n; ++j)
            {
                var w = wa[j];
                var m = w.margin === undefined ? defaultMargin : w.margin;
                w.y = pm.bottom + m.top + wy;
    
                switch (alignH)
                {
                    case "center":
                        w.x = lineLeft + (lineWidth - w.getPropertyFinal("width") + m.left - m.right) / 2;
                        break;
                    case "right":
                        w.x = (lineLeft + lineWidth) - w.getPropertyFinal("width") - m.right;
                        break;
                    default:
                        w.x = lineLeft + m.left;
                        break;
                }
    
    //  gibbon.log("\t["+w.id+"] h="+w.getPropertyFinal("height") + "  w.y=" + (w.y));
    
                wy += pm.bottom + m.top + w.getPropertyFinal("height");
                pm = m;
            }
        };
    
        this.layoutHorizontal = function (widget)
        {
            var wa = widget.children;
            var maxLineWidth = widget.width;
            var lineWidth = 0;
            var lineHeight = 0;
            var lineStartIndex = 0;
            var wy = 0;
            var i = 0;
            var pm = defaultMargin;
    
            for (var n = wa.length; i < n; )
            {
                var w = wa[i];
    
                var m = w.margin === undefined ? defaultMargin : w.margin;
                var width = m.left + w.getPropertyFinal("width") + m.right;
    
                if ((lineWidth + width) <= maxLineWidth)
                {
                    lineWidth += width;
                    var h = w.getPropertyFinal("height") + m.top + m.bottom;
                    if (h > lineHeight)
                    {
                        lineHeight = h;
                    }
                    ++i;
                }
                else
                {
                    this.layoutLineHorizontal(lineStartIndex, i, widget, wy, lineHeight, lineWidth, maxLineWidth);
    
                    wy += lineHeight;
    
                    lineWidth = 0;
                    lineHeight = 0;
                    lineStartIndex = i;
                }
                pm = m;
            }
    
            this.layoutLineHorizontal(lineStartIndex, i, widget, wy, lineHeight, lineWidth, maxLineWidth);
        };
    
        this.layoutLineHorizontal = function (lineStartIndex, n, widget, lineTop, lineHeight, lineWidth, maxLineWidth)
        {
            var wa = widget.children;
            var wx = 0;
            var widgetHeight = widget.height;
            var pm = defaultMargin;
    
            switch (this.alignH)
            {
                case "center":
                    wx += (maxLineWidth - lineWidth) / 2;
                    break;
                case "right":
                    wx += maxLineWidth - lineWidth;
                    break;
                default:
                    break;
            }
    
            var alignV = this.alignV;
            for (var j = lineStartIndex; j < n; ++j)
            {
                var w = wa[j];
                var m = w.margin === undefined ? defaultMargin : w.margin;
                w.x = pm.right + m.left + wx;
    
                switch (alignV)
                {
                    case "center":
                        w.y = lineTop + (lineHeight - w.getPropertyFinal("height") + m.top - m.bottom) / 2;
                        break;
                    case "top":
                        w.y = lineTop + m.top;
                        break;
                    default:
                        w.y = (lineTop + lineHeight) - w.getPropertyFinal("height") - m.bottom;
                        break;
                }
    
    //  gibbon.log("\t["+w.id+"] h="+w.getPropertyFinal("height") + "  w.y=" + (w.y));
    
                wx += pm.right + m.left + w.getPropertyFinal("width");
                pm = m;
            }
        };
    
        this.layout = markup.direction == "vertical" ? this.layoutVertical : this.layoutHorizontal;
    };
    
    // layout the child widgets vertically,
    // setting the width to that of the parent
    // minus the child's margins
    gibbon.layout.vertical = function (markup)
    {
        var defaultMargin = { left: 0, top: 0, right: 0, bottom: 0 };
    
        this.layout = function (widget)
        {
            var wa = widget.children;
            var parentWidth = widget.width;
            var y = 0;
            var pm = defaultMargin;
    
            for (var i = 0, n = wa.length; i < n; ++i)
            {
                var w = wa[i];
                var m = w.margin === undefined ? defaultMargin : w.margin;
    
//                w.x = m.left;
                y += pm.bottom + m.top;
                w.y =  y;
                if (w.width === 0)
                {
                    w.width = parentWidth - (m.left + m.right);
                }
                y += w.getPropertyFinal("height");
                pm = m;
            }
        };
    };
    
    // layout the child widgets horizontally,
    // setting the height to that of the parent
    // minus the child's margins
    gibbon.layout.horizontal = function (markup)
    {
        var defaultMargin = { left: 0, top: 0, right: 0, bottom: 0 };
    
        this.layout = function (widget)
        {
            var wa = widget.children;
            var parentHeight = widget.height;
            var x = 0;
            var pm = defaultMargin;
    
            for (var i = 0, n = wa.length; i < n; ++i)
            {
                var w = wa[i];
                var m = w.margin === undefined ? defaultMargin : w.margin;
    
                w.y = m.top;
                x += pm.right + m.left;
                w.x = x;
                w.height = parentHeight - (m.top + m.bottom);
                x += w.getPropertyFinal("width");
                pm = m;
            }
        };
    };

    gibbon.createLayout = function (layoutType, markup) {
        if (gibbon.layout.hasOwnProperty(layoutType)) {
            return new gibbon.layout[layoutType](markup);
        }
        // return undefined;
    };

})();

/* Notes:
    - Move functionality from markup.calcWidgetProperty into widget so we can use semantic
      values in widget properties outside of markup context.
    - (?) Defer proxy updates by tracking changed properties and adding widgets to a dirty list.
*/


(function () {

    function parsePercentageValue(val, parentValue) {
        if (typeof(val) == "string") {
            if (val.indexOf("%") != -1) {
                var va = val.split("%", 1);
                val = parseInt(va[0], 10);
                if (parentValue === undefined) {
                    parentValue = 0;
                }
                val = (val * parentValue) / 100;
            }
            else {
                val = parseInt(val, 10);
            }
        }

        return val;
    };
    
    function parseImageAlign(newAlign) {
        align = "";//nrdp.gibbon.Image.ALIGN_NORMAL;

        if (newAlign.indexOf("tile") != -1) {
            align = "tile";//nrdp.gibbon.Image.ALIGN_TILE;
        }

        if (newAlign.indexOf("center") != -1) {
            align = "center";//nrdp.gibbon.Image.ALIGN_CENTER;
        }

        if (newAlign.indexOf("stretch") != -1) {
            align = "stretch";//nrdp.gibbon.Image.ALIGN_STRETCH;
        }

        return align;
    };

    gibbon.widget = function () {
        var t = this;

        t.proxy = gibbon.makeWidget(this);
        // set the local values and assume the proxy matches up
        t._x = 0;
        t._y = 0;
        t._width = 0;
        t._height = 0;
        t._drawOrder = 0;
        t._hidden = false;
        t._opacity = 1.0;
        t._scale = 1.0;
        t._textStyle = new gibbon.defaultTextStyle();
        t._text = "";
        t._transformOriginX = 0;
        t._transformOriginY = 0;
        t._display = "";
        t._clip = false;

        // properties with NO getter/setter
        t.children = [];
        t.style = gibbon.style.createStyle(this);
//        t.image = ""; // this property is entirely shadowed by the proxy, no need to init here
    };

    var cloneFunc = function (deep) {
        var t = this, w = new gibbon.widget(),
            wc, i, n, k,
            image, sourceRect, r;

        if (t.image !== undefined) {
            if (typeof(t.image) === "string") {
                if (t.image.length > 0) {
                    w.image = t.image;
                }
            } else {
                image = {
                    url: t.image.url,
                    halign: t.image.halign,
                    valign: t.image.valign,
                };

                if (t.image.sourceRect !== undefined) {
                    image.sourceRect = {
                        x: t.image.sourceRect.x,
                        y: t.image.sourceRect.y,
                        width: t.image.sourceRect.width,
                        height: t.image.sourceRect.height,
                    };
                }

                t.image = image;
            }
        }

        for (k in t) {
            switch (k) {

                // do not copy these properties
                case "proxy":
                case "children":
                case "_parent":
                    break;

                case "style":
                    w.className = t.className;
                    break;

                default: {
                    if (t.hasOwnProperty(k)) {
                        if (k.charAt(0) === '_') {
                            w[k.substr(1)] = t[k.substr(1)];
                        } else {
                            w[k] = t[k];
                        }
                    }
                    break;
                }
            }
        }

        if (deep) {
            for (i = 0, n = t.children.length; i < n; i++) {
                wc = t.children[i].clone(deep);
                wc.parent = w;
            }
        }

        return w;
    };

    gibbon.widget.prototype = {

        constructor: gibbon.widget,

        get id() {
            return this._id;
        },

        set id(id) {
            this._id = id;
            this.proxy.name = id;
        },

        get rect() {
            return { x: this.x, y: this.y, width: this.width, height: this.height };
        },

        set rect(v) {
            var t = this,
                parentHeight = t._parent !== undefined ? t._parent._height : gibbon.scene.height,
                parentWidth = t._parent !== undefined ? t._parent._width : gibbon.scene.width,
                rect = {
                    x: v.x,
                    y: v.y,
                    right: v.right,
                    bottom: v.bottom,
                };

            if (v.width !== undefined) {
                rect.width = parsePercentageValue(v.width, parentWidth);
            }
            if (v.height !== undefined) {
                rect.height = parsePercentageValue(v.height, parentHeight);
            }

            if (rect.width !== undefined) {
                t._width = rect.width;
            } 
            if (rect.height !== undefined) {
                t._height = rect.height;
            } 
            if (rect.x !== undefined) {
                t._x = rect.x;
            } else if(rect.right !== undefined) {
                rect.x = t._x = parentWidth - t._width - rect.right;
                delete rect.right;
            }
            if (rect.y !== undefined) {
                t._y = rect.y;
            } else if (rect.bottom !== undefined) {
                rect.y = t._y = parentHeight - t._height - rect.bottom;
                delete rect.bottom;
            }
            t.proxy.rect = rect;
        },

        get x() {
            return this._x;
        },

        set x(x) {
            var t = this;
            t._x = x;
            t.proxy.x = x;
        },

        get y() {
            return this._y;
        },

        set y(y) {
            this._y = y;
            this.proxy.y = y;
        },

        get right() {
            var t = this, parent = t._parent || gibbon.scene;
            return parent._width - t._x + t._width;
        },

        set right(r) {
            var t = this, parent = t._parent || gibbon.scene;
            t.x = parent._width - t._width - r;
        },

        get bottom() {
            var t = this, parent = t._parent || gibbon.scene;
            return parent._height - t._y + t._height;
        },

        set bottom(b) {
            var t = this, parent = t._parent || gibbon.scene;
            t.y = parent._height - t._height - b;
        },

        get width() {
            return this._width;
        },

        set width(w) {
            var t = this,
                parentWidth = t._parent !== undefined ? t._parent._width : gibbon.scene.width,
                v = parsePercentageValue(w, parentWidth);

            t._width = v;
            t.proxy.width = v;
        },

        get height() {
            return this._height;
        },

        set height(h) {
            var t = this,
                parentHeight = t._parent !== undefined ? t._parent._height : gibbon.scene.height,
                v = parsePercentageValue(h, parentHeight);

            t._height = v;
            t.proxy.height = v;
        },

        get drawOrder() {
            return this._drawOrder;
        },

        set drawOrder(d) {
            this._drawOrder = d;
            this.proxy.drawOrder = d;
        },

        get hidden() {
            return this._hidden;
        },

        set hidden(h) {
            this._hidden = h;
            this.proxy.hidden = h;
        },

        get opacity() {
            return this._opacity;
        },

        set opacity(o) {
            this._opacity = o;
            this.proxy.opacity = o;
        },

        get scale() {
            return this._scale;
        },

        set scale(s) {
            this._scale = s;
            this.proxy.scale = s;
        },

        get textStyle() {
            return this._textStyle;
        },

        set textStyle(v) {
            var t = this,
                ts = t._textStyle;

            if (ts === undefined) {
                ts = new gibbon.defaultTextStyle();
            }
            ts.merge(v);

            t._textStyle = ts;
            t.proxy.textStyle = ts;
        },

        get text() {
            return this._text;
        },

        set text(t) {
            this._text = t;
            this.proxy.text = t;
        },

        get textContent() {
            return this._text;
        },

        set textContent(t) {
            this._text = t;
            this.proxy.text = t;
        },

        get transformOriginX() {
            return this._transformOriginX;
        },

        set transformOriginX(t) {
            this._transformOriginX = t;
            this.proxy.transformOriginX = t;
        },

        get color() {
            return this._color;
        },

        set color(c) {
            var t = this,
                v = new gibbon.color(c);

            t._color = v;
            t.proxy.color = v;
        },

        get transformOriginY() {
            return this._transformOriginY;
        },

        set transformOriginY(t) {
            this._transformOriginY = t;
            this.proxy.transformOriginY = t;
        },

        get display() {
            return this._display;
        },

        set display(d) {
            this._display = d;
            this.proxy.setDisplayFlags(d);
        },

        get clip() {
            return this._clip;
        },

        set clip(c) {
            this._clip = c;
            this.proxy.clip = c;
        },

        get cache() {
            return this._cache;
        },

        set cache(c) {
            this._cache = c;
            this.proxy.cache = c;
        },

        get image() {
            return this._image;
        },

        set image(img) {
            if (img === undefined || typeof(img) === "string") {
                this.proxy.image.url = img || "";
                this._image = img || "";
            } else {
                var t = this,
                    image = t._image,
                    proxyImage = t.proxy.image,
                    x, y, w = t.width, h = t.height,
                    align, r
                    ;

                if (image === undefined) {
                    t._image = image = {};
                }

                if (img.sourceRect !== undefined) {

                    r = img.sourceRect;

                    if (r.x !== undefined) {
                        x = r.x;
                    } else {
                        x = image.x !== undefined ? image.x : 0;
                    }
                    if (r.y !== undefined) {
                        y = r.y;
                    } else {
                        y = image.y !== undefined ? image.y : 0;
                    }
                    if (r.width !== undefined) {
                        w = r.width;
                    } else {
                        w = image.width !== undefined ? image.width : w;
                    }
                    if (r.height !== undefined) {
                        h = r.height;
                    } else {
                        h = image.height !== undefined ? image.height : h;
                    }

                    proxyImage.sourceRect = { x:x, y:y, width:w, height:h, };
                    image.sourceRect = { x:x, y:y, width:w, height:h, };
                }

                if (img.halign !== undefined) {
                    align = parseImageAlign(img.halign);
                    proxyImage.halign = align;
                    image.halign = align;
                }

                if (img.valign !== undefined) {
                    align = parseImageAlign(img.valign);
                    proxyImage.valign = align;
                    image.valign = align;
                }

                if (img.url !== undefined) {
                    proxyImage.url = img.url;
                    image.url = img.url;
                }
            }
        },

        addImageLoadedListener: function (listener) {
            this.proxy.addEventListener("imageLoaded", listener);
        },

        removeImageLoadedListener: function (listener) {
            this.proxy.removeEventListener("imageLoaded", listener);
        },

        get transition() {
            return this._transition;
        },

        set transition(a) {
            if (a instanceof Array) {
                for (var i = 0, n = a.length; i < n; i++) {
                    this._setTransition(a[i]);
                }
            } else {
                this._setTransition(a);
            }
        },

        addTransitionEndListener: function (listener) {
            this.proxy.addEventListener("animationFinished", listener);
        },

        removeTransitionEndListener: function (listener) {
            this.proxy.removeEventListener("animationFinished", listener);
        },

        _setTransition: function (a) {
            var t = this;
            if (a !== undefined) {
                t._transition = {
                    property: a.property,
                    duration: a.duration,
                    ease: a.ease,
                    flags: a.flags,
                };
                t.proxy.animate(a.property, a.duration, a.ease, a.flags);
            } else {
                if (t._transition !== undefined) {
                    // stop/clear out all transitions on the proxy?
                    // should it stop where it's at or jump to the end value?
                    t.proxy.animate(t._transition.property, 0, "", "");
                    t._transition = undefined;
                }
            }
        },

        get className() {
            return this.style.className;
        },

        set className(c) {
            this.style.className = c;
        },

        get parent() {
            return this._parent;
        },

        set parent(newParent) {
            var oldParent = this._parent;
            this._parent = newParent;

            if (oldParent !== newParent) {
                if (oldParent !== undefined) {   // remove ourselves from old parent
                    try {
                        var i = oldParent.children.indexOf(this);
                        if (i != -1) {
                            oldParent.children.splice(i, 1);
                            if (typeof(this.id) == "string") {
                                delete oldParent[this.id];
                            }
                        }
                    }
                    catch (e) {
                    }
                }

                if (newParent !== undefined) {   // add ourselves to new parent
                    try {
                        if (newParent.children === undefined) {
                            newParent.children = [];
                        }

                        newParent.children.push(this);

                        if (typeof(this.id) == "string") {
                            newParent[this.id] = this;
                        }
    
                    }
                    catch (e) {
                    }

                    this.proxy.parent = newParent.proxy;
                }
                else {
                    this.proxy.parent = undefined;
                }
            }
        },

        appendChild: function (child) {
            child.parent = this;
        },

        removeChild: function (child) {
            child.parent = undefined;
        },

        removeChildren: function () {
            var children = this.children;

            if (children !== undefined) {
                while (children.length != 0) {
                    children[0].parent = undefined;
                }
            }
        },

        getPropertyFinal: function (key) {
            return this[key];
        },

        set layout(v) {
            var layout, layoutType = v.type;

            if (typeof(layoutType) === "function") {
                layout = new layoutType(v);
            } else {
                layout = gibbon.createLayout(layoutType, v);
            }

            this._layout = layout;
        },

        layoutChildren: function () {
            if (this._layout) {
                this._layout.layout(this);
            }
        },

        getElementById: function (id) {
            return this.getElementByProperty("id", id);
        },

        getElementByProperty: function (prop, value) {
            var t = this, i, n;

            for (i = 0, n = t.children.length; i < n; i++) {
                el = t.children[i];
                if (el[prop] == value) {
                    return el;
                }
                el = el.getElementByProperty(prop, value);
                if (el) {
                    return el;
                }
            }
        },

        getElementsByProperty: function (prop, value, elems) {
            var t = this, children = t.children, i, n;

            if (!elems) {
                elems = [];
            }

            for (i = 0, n = children.length; i < n; i++) {
                el = children[i];
                if (el[prop] == value) {
                    elems.push(el);
                }
                el.getElementsByProperty(prop, value, elems);
            }

            return elems;
        },

        getElementsWithProperty: function (prop, elems) {
            var t = this, children = t.children, i, n;

            if (!elems) {
                elems = [];
            }

            for (i = 0, n = children.length; i < n; i++) {
                el = children[i];
                if (el.hasOwnProperty(prop)) {
                    elems.push(el);
                }
                el.getElementsWithProperty(prop, elems);
            }

            return elems;
        },

        getElementsByClassName: function (className, elems) {
            var t = this, children = t.children, i, n;

            if (!elems) {
                elems = [];
            }

            for (i = 0, n = children.length; i < n; i++) {
                el = children[i];
                if (el.style && el.style.hasClass(className)) {
                    elems.push(el);
                }
                el.getElementsByClassName(className, elems);
            }

            return elems;
        },

        forEachChild: function (cb, deep) {
            var children = this.children, i, n, w;

            if (deep) {
                for (i = 0, n = children.length; i < n; i++) {
                    w = children[i];
                    cb(w);
                    w.forEachChild(cb, true);
                }
            } else {
                for (i = 0, n = children.length; i < n; i++) {
                    cb(children[i]);
                }
            }
        },

        parentHasClass: function (className) {
            var p = this.parent;
            return p !== undefined && p.style.hasClass(className);
        },

        onStyleChange: function (style) {
//gibbon.log("+--------------------------");

            var t = this, k,
                newStyles = style.computedStyles,
                currentStyles = t._currentStyles,
                styledProperties = t.__proto__._styledProperties,
                transition, newTransition
                ;

            // reset properties that are no longer set via style
            if (currentStyles) {
                for (k in currentStyles) {
                    if (!newStyles.hasOwnProperty(k)) {
                        t[k] = styledProperties[k];
//gibbon.log("onStyleChange:["+t.id+"] property reset "+k+" = "+t[k]+"=>"+styledProperties[k]);
                    }
                }
            }

            // remember the new styles for next time
            currentStyles = {};
            for (k in newStyles) {
                currentStyles[k] = newStyles[k];
            }
            t._currentStyles = currentStyles;

            // special case transition to make sure it's set first
            if (newStyles.hasOwnProperty("transition")) {
                transition = t.transition;
                newTransition = newStyles.transition;
                if (transition === undefined ||
                    transition.property !== newTransition.property ||
                    transition.duration !== newTransition.duration ||
                    transition.ease !== newTransition.ease) {
                    t.transition = newStyles.transition;
                }
                delete newStyles.transition;
            }

            for(var d in { x: 0, y: 0, width: 0, height: 0,
                           bottom: 0, right: 0 }) {
                if(newStyles.hasOwnProperty(d)) {
                    if(newStyles.rect === undefined)
                        newStyles.rect = new Object();
                    newStyles.rect[d] = newStyles[d];
                    delete newStyles[d];
                }
            }

            for (k in styledProperties) {
                if (newStyles.hasOwnProperty(k)) {
//gibbon.log("onStyleChange:["+t.id+"] ("+this.style.className+") " + k + " = " + newStyles[k]);
                    t[k] = newStyles[k];
                }
            }
            if(newStyles.rect !== undefined)
                t.rect = newStyles.rect;
//gibbon.log("+--------------------------");
        },

        _styledProperties: {
            transition:undefined,
            x: 0, y:0, width:0, height:0,
            bottom:0, right:0,
            drawOrder: 0,
            hidden: false,
            opacity: 1.0,
            scale: 1.0,
            text: "",
            transformOriginX: 0,
            transformOriginY: 0,
            display: "",
            textStyle: new gibbon.defaultTextStyle(),
            color:undefined,
            clip:false,
            image:"",
        },

        cloneNode: cloneFunc,
        clone: cloneFunc,
    };

})();


// @depend widget.js
// @depend layout.js
// @depend scene.js

(function () {

    gibbon.parseMarkup = function(curMarkup, parentWidget) {
        var widgets = [],
            markup, newWidget,
            i, n
        ;

        for (i = 0, n = curMarkup.length; i < n; i++) {
            markup = curMarkup[i],
            newWidget = new gibbon.widget();

            if (parentWidget === undefined) {
                newWidget.width = gibbon.scene.width;
                newWidget.height = gibbon.scene.height;
            } else {
                // HACK: some properties require a parent when set (top,left,bottom,right)
                // but some bug prevent us from truly adding the widget to the tree at this point
                newWidget._parent = parentWidget;
            }


            for (var key in markup) {
                if (!markup.hasOwnProperty(key)) {
                    continue;
                }

                if (key !== "children") {
                    newWidget[key] = markup[key];
                }
            }

            newWidget._parent = undefined;

            // set the parent after the other properties are parsed
            // so we add a fully formed widget to the parent
            newWidget.parent = parentWidget;

            var markupChildren = markup.children;
            if (typeof(markupChildren) == "object") {
                gibbon.parseMarkup(markupChildren, newWidget);
            }

            widgets.push(newWidget);
        }

        return widgets;
    };

    gibbon.createWidgets = function(markup) {
        try {
            var widgets;

            if (markup !== undefined) {
                widgets = gibbon.parseMarkup([ markup ]);
            }

            return widgets[0];
        }
        catch (e) {
            gibbon.log("gibbon.createWidgets: " + e);
        }

        return null;
    };

})();

// @depend object.js
// @depend primitives.js
// @depend scene.js
// @depend layout.js
// @depend style.js
// @depend markup.js
// @depend widget.js


// @depend preamble.js
// @depend ../common/framework.js

// browser simulation

if (typeof window === "undefined") {

    function clearTimeout(id) {  if(id !== undefined) nrdp.gibbon.clearTimeout(id); }
    function setTimeout(cb, interval) { return nrdp.gibbon.setTimeout(cb, interval, true); }
    function setInterval(cb, interval) { return nrdp.gibbon.setTimeout(function() { cb(); return interval; }, interval, false); }
    function clearInterval(id) { if(id !== undefined) nrdp.gibbon.clearTimeout(id); }

    var XMLHttpRequest = function() {
        this._id = 0;
        this._eventListeners = {};
        this._readyState = 0;
        this.timeout = 30000;
        this._responseText = "";
        this._status = 0;
        this._headers = {};
    };
    XMLHttpRequest.prototype = {
        constructor:XMLHttpRequest,
        get UNSENT() { return 0; },
        get OPENED() { return 1; },
        get HEADERS_RECEIVED() { return 2; },
        get LOADING() { return 3; },
        get DONE() { return 4; },
        addEventListener: function(eventType, listener) {
            if (listener) {
                if (!this._eventListeners[eventType])
                    this._eventListeners[eventType] = [];
                this._eventListeners[eventType].push(listener);
            }
        },
        removeEventListener: function(eventType, listener) {
            if (listener) {
                var myListeners = this._eventListeners[eventType];
                if (myListeners) {
                    var index = myListeners.indexOf(listener);
                    if (index >= 0) {
                        if (myListeners.length == 1)
                            delete this._eventListeners[eventType];
                        else
                            this._eventListeners[eventType].splice(index, 1);
                    }
                }
            }
        },
        _callEventListeners: function(event) {
            if (this["on" + event.type.toLowerCase()])
                this["on" + event.type.toLowerCase()](event);
            if (this._eventListeners[event.type]) {
                var myListeners = this._eventListeners[event.type].slice(0);
                for (var i = 0; i < myListeners.length; i++) {
                    var listener = myListeners[i];
                    if (listener)
                        listener(event);
                }
            }
        },
        _setReadyState: function(s) {
            this._readyState = s;
            this._callEventListeners({type: "readyStateChange"});
        },
        open: function(method, url, async, user, password) {
            if(method != "GET")
                throw "SyntaxError";
            this._url = url;
            this._async = async;
            this._setReadyState(this.OPENED);
        },
        setRequestHeader: function(header, value)  {
            this._headers[header] = value;
        },
        _finished: function(text) {
            this._responseText = text;
            this._setReadyState(this.DONE);
        },
        send: function(data)  {
            if(data)
                throw "XMLHTTP send data not supported!!";
            this.abort();
            var xhr = this;
            this._id = nrdp.gibbon.load(this._url, function(t) { xhr._finished(t) }, this._headers, this._async);
            this._setReadyState(this.LOADING);
        },
        abort: function() {
            if(this._id) {
                nrdp.gibbon.stopLoad(this._id);
                this._id = 0;
            }
        },
        get readyState() { return this._readyState; },
        get responseXML() { return; },
        get responseText() { return this._responseText; },
        get response() { return this.responseText; },
        get responseType() { return "text"; },
        get status() { return this._status; }
    };

    var localStorage = {
        setItem: function(key, value) {
            nrdp.storage.setItem(nrdp.storage.NO_DEVICE_ACCOUNT, key, value);
        },
        getItem: function(key) {
            return nrdp.storage.getItem(nrdp.storage.NO_DEVICE_ACCOUNT, key);
        },
        removeItem: function(key) {
            nrdp.storage.removeItem(nrdp.storage.NO_DEVICE_ACCOUNT, key);
        }
    };

    var sessionStorage = {
        setItem: function(key, value) {
            if (!nrdp.isReady)
                return;
            var tmp = nrdp.storage.transientData || {};
            if (tmp.sessionStorage === undefined)
                tmp.sessionStorage = {};
            tmp.sessionStorage[key] = value;
            nrdp.storage.transientData = tmp;
        },
        getItem: function(key) {
            if (!nrdp.isReady)
                return undefined;
            var tmp = nrdp.storage.transientData || {};
            if (tmp.sessionStorage === undefined)
                tmp.sessionStorage = {};
            return tmp.sessionStorage[key];
        },
        removeItem: function (key) {
            if (!nrdp.isReady)
                return;
            var tmp = nrdp.storage.transientData || {};
            if (tmp.sessionStorage === undefined)
                tmp.sessionStorage = {};
            delete tmp.sessionStorage[key];
            nrdp.storage.transientData = tmp;
        }
    };

    var navigator = {
        userAgent: "Gibbon/nGn"
    };
    var screen = {
        get width() { return nrdp.gibbon.scene.width; },
        get height() { return nrdp.gibbon.scene.height; }
    };
    var console = {
        _log: function(type, msg) {
            if(typeof nrdp_platform !== "undefined" && nrdp_platform.log)
                nrdp_platform.log(msg + " [CONSOLE." + type + "]" );
            else
                nrdp.log[type](msg);
        },
        info: function(msg) { this._log("info", msg); },
        log: function(msg) { this._log("debug", msg); },
        error: function(msg) { this._log("error", msg); }
    };
    var window = {
        navigator: navigator,
        localStorage: localStorage,
        sessionStorage: sessionStorage,
        console: console,
        nrdp: nrdp,
        setTimeout: setTimeout,
        location: "http://gibbon.netflix.com/v2/nGn/",
        removeEventListener: function(evt, listener) { },
        addEventListener: function(evt, listener) {
        }
    };
    var document = {
        removeEventListener: function(evt, listener) { },
        addEventListener: function(evt, listener) {
            if(evt == "DOMContentLoaded") {
                if(nrdp.isReady)
                    listener();
                else
                    nrdp.addEventListener("init", listener);
            } else if(evt == "keyup") {
                nrdp.gibbon.addEventListener("key", function(evt) {
                                                 if(evt.data.type != "release")
                                                     return;
                                                 listener({which: evt.data.code, preventDefault: function() { }});
                                             });
            } else if(evt == "keydown") {
                nrdp.gibbon.addEventListener("key", function(evt) {
                                                 if(evt.data.type != "press")
                                                     return;
                                                 listener({which: evt.data.code, preventDefault: function() { }});
                                             });
            }
        },
        get cookie() {
            var cookies = document._cookies;
            var rv = "";
            for (var i in cookies) {
                if (cookies.hasOwnProperty(i)) {
                    if (rv) rv += "; ";
                    rv += i + "=" + cookies[i];
                }
            }
            return rv;
        },
        set cookie(c) {
            var cookies = document._cookies;
            var bits = c.split(';')[0].split('=');
            cookies[bits[0]] = bits[1];
        },
        _cookies: {}
    };
    var Image = function() {
        this._src = "";
        return this;
    };
    Image.prototype = {
        constructor:Image,
        get src() { return this._src; },
        set src(s) { var t = this; t._src = s; setTimeout(function() { t.onload(); }, 5); }
    };

}

// @depend native/base.js

(function () {

    var listItemMarkup = {
        id: "foo",
        transition: { property: "x", duration: 1000, ease: "linear" },
        image: "http://cdn-4.nflximg.com/en_us/boxshots/150/70120584.jpg",
        rect: { y: 200, width: 150, height: 214 },
        display: "opaque",
    };

    var listItemMarkup_bar = {
        id: "bar",
        color: "#FF0000",
        rect: { y: 200, width: 150, height: 214 },
        display: "opaque",
    };

    var sceneMarkup = {
        scene: {
            id: "list",
            rect: { width: "100%", height: "100%" },
            color: "#FFF",
            children: [
                listItemMarkup_bar,
                listItemMarkup,
            ],
        },
    };

    gibbon.init(function () {

        gibbon.scene.start(sceneMarkup);

        var widget = gibbon.getElementById("foo"), x = 0;
        var box = gibbon.getElementById("bar");

        document.addEventListener("keydown", function (event) {
                switch (event.which) {
                case 37:    // left
                    x = 0;
                    break;
                case 39:    // right
                    x = 1000;
                    break;
            }

            widget.x = x;
            box.x = x;
        });
    });

})();
