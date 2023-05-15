var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/controls/ctrlbase'], function (require, exports, __CTRL_BASE__) {
    var CScrollControl = (function (_super) {
        __extends(CScrollControl, _super);
        function CScrollControl($self, isPagingScheme) {
            if (isPagingScheme === void 0) { isPagingScheme = false; }
            _super.call(this, $self);
            this._elText = document.createElement("div");
            this._elScrollBar = document.createElement("div");
            this._elText.classList.add(CScrollControl.CLASS.TEXT);
            this._elText.textContent = hx.l('LOC_LOADING_DOT_ID');
            this.setDrawEngine(function (aRect, aDrawParam) {
                return [document.createElement("div")];
            });
            this.buildNaviMap(__CTRL_BASE__.KBuilderTopDown);
            this.$self().append(this._elScrollBar).append(this._elText);
            this.addClass(CScrollControl.CLASS.VIEW);
            this._elScrollBar.classList.add(CScrollControl.CLASS.SCROLLBAR);
            this._isPageScheme = isPagingScheme;
            this.registerSignal(["MovableStateBefore", "MovableStateAfter"]);
        }
        CScrollControl.prototype.setShow = function () {
            this._elScrollBar.classList.add(CScrollControl.CLASS.SHOW);
            return this;
        };
        CScrollControl.prototype.setHide = function () {
            this._elScrollBar.classList.remove(CScrollControl.CLASS.SHOW);
            return this;
        };
        CScrollControl.prototype._calculateScrollArea = function () {
            var viewHeight = this.$self().height();
            var scrollHeight = this._elText.scrollHeight;
            if (viewHeight >= scrollHeight) {
                console.log("does not need scroll handler");
                this._doKeyUp = null;
                this._doKeyDown = null;
                this._doKeyChannelUp = null;
                this._doKeyChannelDown = null;
                this.setHide();
                return this;
            }
            this.setShow();
            var offsetTopText = this._elText.offsetTop;
            var offsetTopScrollBar = this._elScrollBar.offsetTop;
            var limitScrollBar = viewHeight - this._elScrollBar.offsetHeight;
            var DEGREE = 50;
            var degreeText = scrollHeight / DEGREE;
            var degreeScrollBar = limitScrollBar / DEGREE;
            var stText = this._elText.style;
            var stScrollBar = this._elScrollBar.style;
            this._element.style.overflow = 'hidden';
            if (this._isPageScheme) {
                this.emit('MovableStateAfter', true);
                degreeText = this.$self().height();
                this._setKeyHandler(function () {
                    var ret = this.hasFocus();
                    if (ret) {
                        stText.top = Math.min((parseInt(stText.top) || 0) + degreeText, offsetTopText);
                        this.emit('MovableStateBefore', parseInt(stText.top) !== offsetTopText);
                        this.emit('MovableStateAfter', parseInt(stText.top) !== -(scrollHeight - viewHeight));
                    }
                    return ret;
                }, function () {
                    var ret = this.hasFocus();
                    if (ret) {
                        stText.top = Math.max((parseInt(stText.top) || 0) - degreeText, -(scrollHeight - viewHeight));
                        this.emit('MovableStateBefore', parseInt(stText.top) !== offsetTopText);
                        this.emit('MovableStateAfter', parseInt(stText.top) !== -(scrollHeight - viewHeight));
                    }
                    return ret;
                });
            }
            else {
                this._setKeyHandler(function () {
                    var ret = this.hasFocus();
                    if (ret) {
                        stText.top = Math.min((parseInt(stText.top) || 0) + degreeText, offsetTopText);
                        stScrollBar.top = Math.max((parseInt(stScrollBar.top) || offsetTopScrollBar) - degreeScrollBar, offsetTopScrollBar);
                    }
                    return ret;
                }, function () {
                    var ret = this.hasFocus();
                    if (ret) {
                        stText.top = Math.max((parseInt(stText.top) || 0) - degreeText, -(scrollHeight - viewHeight));
                        stScrollBar.top = Math.min((parseInt(stScrollBar.top) || offsetTopScrollBar) + degreeScrollBar, limitScrollBar);
                        ret = parseInt(stText.top) !== Math.ceil(-(scrollHeight - viewHeight));
                    }
                    return ret;
                }, function () {
                    var ret = this.hasFocus();
                    if (ret) {
                        stText.top = Math.min((parseInt(stText.top) || 0) + degreeText * 10, offsetTopText);
                        stScrollBar.top = Math.max((parseInt(stScrollBar.top) || offsetTopScrollBar) - degreeScrollBar * 10, offsetTopScrollBar);
                    }
                    return ret;
                }, function () {
                    var ret = this.hasFocus();
                    if (ret) {
                        stText.top = Math.max((parseInt(stText.top) || 0) - degreeText * 10, -(scrollHeight - viewHeight));
                        stScrollBar.top = Math.min((parseInt(stScrollBar.top) || offsetTopScrollBar) + degreeScrollBar * 10, limitScrollBar);
                        ret = parseInt(stText.top) !== Math.ceil(-(scrollHeight - viewHeight));
                    }
                    return ret;
                });
            }
            return this;
        };
        CScrollControl.prototype._setKeyHandler = function (before, after, beforePage, afterPage) {
            if (this._isPageScheme) {
                this._doKeyLeft = before;
                this._doKeyRight = after;
            }
            else {
                this._doKeyUp = before;
                this._doKeyDown = after;
                if (beforePage && afterPage) {
                    this._doKeyChannelUp = beforePage;
                    this._doKeyChannelDown = afterPage;
                }
            }
            return this;
        };
        CScrollControl.prototype.setText = function (aText) {
            this._elText.textContent = aText;
            return this._calculateScrollArea();
        };
        CScrollControl.prototype.setHTML = function (aHTML) {
            this._elText.innerHTML = aHTML;
            return this._calculateScrollArea();
        };
        Object.defineProperty(CScrollControl.prototype, "textTop", {
            get: function () {
                var self = this;
                var elText = self._elText;
                if (elText) {
                    var style = elText.style;
                    if (style) {
                        return parseInt(style.top) || 0;
                    }
                }
                return 0;
            },
            set: function (top) {
                var self = this;
                var elText = self._elText;
                if (elText) {
                    var style = elText.style;
                    if (style) {
                        style.top = top.toString();
                    }
                }
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(CScrollControl.prototype, "scrollTop", {
            get: function () {
                var self = this;
                var elScrollBar = self._elScrollBar;
                if (elScrollBar) {
                    var style = elScrollBar.style;
                    if (style) {
                        return parseInt(style.top) || elScrollBar.offsetTop;
                    }
                    return elScrollBar.offsetTop;
                }
                return 0;
            },
            set: function (top) {
                var self = this;
                var elScrollBar = self._elScrollBar;
                if (elScrollBar) {
                    var style = elScrollBar.style;
                    if (style) {
                        style.top = top.toString();
                    }
                }
            },
            enumerable: true,
            configurable: true
        });
        /* author: bglee@humaxdigital.com
         * date: 2015-05-08 ���� 1:42
         * comments:
            - page scheme mode only
         */
        CScrollControl.prototype.connectMovableState = function (holder, handlerBefore, handlerAfter) {
            this.connect("MovableStateBefore", holder, handlerBefore);
            this.connect("MovableStateAfter", holder, handlerAfter);
        };
        CScrollControl.CLASS = {
            VIEW: "-view",
            TEXT: "-text",
            SHOW: "-show",
            SCROLLBAR: "-scroll-bar"
        };
        return CScrollControl;
    })(__CTRL_BASE__.CControl);
    return CScrollControl;
});
