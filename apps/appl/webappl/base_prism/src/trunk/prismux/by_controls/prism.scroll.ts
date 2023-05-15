/**
 * Created by bglee on 2015-03-09.
 */
///<reference path="../../def/sugar.d.ts"/>
import __CTRL_BASE__ = require('prismux/controls/ctrlbase');
import CArrowLayout = require("prismux/layout/prism.layout.arrow");
declare var hx: ifSugar;

class CScrollControl extends __CTRL_BASE__.CControl {
    static CLASS = {
        VIEW: "-view",
        TEXT: "-text",
        SHOW: "-show",
        SCROLLBAR: "-scroll-bar"
    };
    private _elText = document.createElement("div");
    private _elScrollBar = document.createElement("div");
    private _doKeyUp: (aKey: number) => boolean;
    private _doKeyDown: (aKey: number) => boolean;
    private _doKeyChannelUp: (aKey: number) => boolean;
    private _doKeyChannelDown: (aKey: number) => boolean;
    private _doKeyLeft: (aKey: number) => boolean;
    private _doKeyRight: (aKey: number) => boolean;
    private _isPageScheme: boolean;

    constructor($self, isPagingScheme = false) {
        super($self);
        this._elText.classList.add(CScrollControl.CLASS.TEXT);
        this._elText.textContent = hx.l('LOC_LOADING_DOT_ID');
        this.setDrawEngine((aRect, aDrawParam) => {
            return [document.createElement("div")];
        });
        this.buildNaviMap(__CTRL_BASE__.KBuilderTopDown);
        this.$self()
            .append(this._elScrollBar)
            .append(this._elText);
        this.addClass(CScrollControl.CLASS.VIEW);
        this._elScrollBar.classList.add(CScrollControl.CLASS.SCROLLBAR);
        this._isPageScheme = isPagingScheme;
        this.registerSignal(["MovableStateBefore", "MovableStateAfter"]);
    }
    setShow() {
        this._elScrollBar.classList.add(CScrollControl.CLASS.SHOW);
        return this;
    }
    setHide() {
        this._elScrollBar.classList.remove(CScrollControl.CLASS.SHOW);
        return this;
    }
    private _calculateScrollArea() {
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
            this._setKeyHandler(function() {
                var ret = this.hasFocus();
                if (ret) {
                    stText.top = <any>Math.min((parseInt(stText.top) || 0) + degreeText, offsetTopText);
                    this.emit('MovableStateBefore', parseInt(stText.top) !== offsetTopText);
                    this.emit('MovableStateAfter', parseInt(stText.top) !== -(scrollHeight - viewHeight));
                }
                return ret;
            }, function() {
                var ret = this.hasFocus();
                if (ret) {
                    stText.top = <any>Math.max((parseInt(stText.top) || 0) - degreeText, -(scrollHeight - viewHeight));
                    this.emit('MovableStateBefore', parseInt(stText.top) !== offsetTopText);
                    this.emit('MovableStateAfter', parseInt(stText.top) !== -(scrollHeight - viewHeight));
                }
                return ret;
            });
        } else {
            this._setKeyHandler(function() {
                var ret = this.hasFocus();
                if (ret) {
                    stText.top = <any>Math.min((parseInt(stText.top) || 0) + degreeText, offsetTopText);
                    stScrollBar.top = <any>Math.max((parseInt(stScrollBar.top) || offsetTopScrollBar) - degreeScrollBar, offsetTopScrollBar);
                }
                return ret;
            }, function() {
                var ret = this.hasFocus();
                if (ret) {
                    stText.top = <any>Math.max((parseInt(stText.top) || 0) - degreeText, -(scrollHeight - viewHeight));
                    stScrollBar.top = <any>Math.min((parseInt(stScrollBar.top) || offsetTopScrollBar) + degreeScrollBar, limitScrollBar);
                    ret = parseInt(stText.top) !== Math.ceil(-(scrollHeight - viewHeight));
                }
                return ret;
            }, function() {
                var ret = this.hasFocus();
                if (ret) {
                    stText.top = <any>Math.min((parseInt(stText.top) || 0) + degreeText * 10, offsetTopText);
                    stScrollBar.top = <any>Math.max((parseInt(stScrollBar.top) || offsetTopScrollBar) - degreeScrollBar * 10, offsetTopScrollBar);
                }
                return ret
            }, function() {
                var ret = this.hasFocus();
                if (ret) {
                    stText.top = <any>Math.max((parseInt(stText.top) || 0) - degreeText * 10, -(scrollHeight - viewHeight));
                    stScrollBar.top = <any>Math.min((parseInt(stScrollBar.top) || offsetTopScrollBar) + degreeScrollBar * 10, limitScrollBar);
                    ret = parseInt(stText.top) !== Math.ceil(-(scrollHeight - viewHeight));
                }
                return ret;
            });
        }
        return this;
    }
    private _setKeyHandler(before: (key: number) => boolean, after: (key: number) => boolean, beforePage?: (key: number) => boolean, afterPage?: (key: number) => boolean) {
        if (this._isPageScheme) {
            this._doKeyLeft = before;
            this._doKeyRight = after;
        } else {
            this._doKeyUp = before;
            this._doKeyDown = after;
            if (beforePage && afterPage) {
                this._doKeyChannelUp = beforePage;
                this._doKeyChannelDown = afterPage;
            }
        }
        return this;
    }
    setText(aText: string) {
        this._elText.textContent = aText;
        return this._calculateScrollArea();
    }
    setHTML(aHTML: string) {
        this._elText.innerHTML = aHTML;
        return this._calculateScrollArea();
    }
    get textTop(): number {
        var self = this;
        var elText = self._elText;
        if (elText) {
            var style = elText.style;
            if (style) {
                return parseInt(style.top) || 0;
            }
        }
        return 0;
    }
    set textTop(top: number) {
        var self = this;
        var elText = self._elText;
        if (elText) {
            var style = elText.style;
            if (style) {
                style.top = top.toString();
            }
        }
    }
    get scrollTop(): number {
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
    }
    set scrollTop(top: number) {
        var self = this;
        var elScrollBar = self._elScrollBar;
        if (elScrollBar) {
            var style = elScrollBar.style;
            if (style) {
                style.top = top.toString();
            }
        }
    }
    /* author: bglee@humaxdigital.com
     * date: 2015-05-08 ¿ÀÈÄ 1:42
     * comments:
        - page scheme mode only
     */
    connectMovableState(holder: any, handlerBefore: string, handlerAfter: string) {
        this.connect("MovableStateBefore", holder, handlerBefore);
        this.connect("MovableStateAfter", holder, handlerAfter);
    }
}
export = CScrollControl;
