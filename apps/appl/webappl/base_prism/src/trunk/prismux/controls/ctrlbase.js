/// <reference path="../../def/jquery.d.ts"/>
/// <reference path="../../def/require.d.ts"/>
/// <reference path="common.ts"/>
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports"], function (require, exports) {
    var CtrlBase;
    (function (CtrlBase) {
        console.log("CtrlBase Start");
        var CCircularArray = (function () {
            function CCircularArray(array) {
                var a = this;
                if (array) {
                    a.setArray(array);
                }
            }
            CCircularArray.prototype.destroy = function () {
                delete this._array;
                delete this._idxCur;
                delete this._idxLast;
            };
            CCircularArray.prototype.getArray = function () {
                var a = this;
                return a._array;
            };
            CCircularArray.prototype.setArray = function (array) {
                var a = this;
                a._array = array;
                a._idxCur = 0;
                a._idxLast = array.length - 1;
            };
            CCircularArray.prototype.appendArray = function (array) {
                var a = this;
                a._array = a._array.concat(array);
                a._idxLast = array.length - 1;
            };
            CCircularArray.prototype.cur = function () {
                var a = this;
                return a._idxCur;
            };
            CCircularArray.prototype.curItem = function () {
                var a = this;
                return a._array[a._idxCur];
            };
            CCircularArray.prototype.setCur = function (index) {
                var a = this;
                a._idxCur = a.indexing(index);
            };
            CCircularArray.prototype.inc = function (count) {
                var a = this;
                count = typeof count != 'undefined' ? count : 1;
                a._idxCur = a.indexing(a._idxCur + count);
            };
            CCircularArray.prototype.dec = function (count) {
                var a = this;
                count = typeof count != 'undefined' ? count : 1;
                a._idxCur = a.indexing(a._idxCur - count);
            };
            CCircularArray.prototype.indexing = function (index) {
                var ret;
                var length = this._array.length;
                if (index >= 0) {
                    ret = index % length;
                }
                else {
                    ret = length + index % length;
                    if (ret === length) {
                        ret = 0;
                    }
                }
                return ret;
            };
            CCircularArray.prototype.at = function (index) {
                return this._array[this.indexing(index)];
            };
            CCircularArray.prototype.get = function (count, offset) {
                var a = this;
                var CResult = function () {
                };
                CResult.prototype = new Array();
                CResult.prototype.firstAvailIndex = function () {
                    var i, a = this;
                    for (i = 0; i < a.length; i += 1) {
                        if (a[i])
                            return i;
                    }
                    return -1;
                };
                CResult.prototype.lastAvailIndex = function () {
                    var i, a = this;
                    for (i = a.length - 1; i >= 0; i -= 1) {
                        if (a[i])
                            return i;
                    }
                    return -1;
                };
                var ret = new CResult();
                offset = typeof offset != 'undefined' ? offset : 0;
                var len = a._array.length;
                var i;
                if (count > len) {
                    var halfCount = Math.floor(len / 2);
                    var centering = halfCount <= offset && halfCount <= count - offset;
                    for (i = 0; i < count; i += 1) {
                        ret.push(null);
                    }
                    if (centering) {
                        var start = offset - halfCount;
                        for (i = 0; i < len; i += 1) {
                            ret[i + start] = a.at(a._idxCur - halfCount + i);
                        }
                    }
                    else if (len - 1 <= offset) {
                        var start = offset - len - 1;
                        for (i = 0; i < len; i += 1) {
                            ret[i + start] = a.at(a._idxCur + i + 1);
                        }
                    }
                    else if (len <= count - offset) {
                        var start = offset;
                        for (i = 0; i < len; i += 1) {
                            ret[i + start] = a.at(a._idxCur + i);
                        }
                    }
                    else {
                        for (i = 0; i < len; i += 1) {
                            ret[i] = a.at(a._idxCur - offset + i);
                        }
                    }
                }
                else {
                    for (i = 0; i < count; i += 1) {
                        ret.push(a.at(a._idxCur - offset + i));
                    }
                }
                return ret;
            };
            CCircularArray.prototype.del = function (index) {
                var a = this;
                var compromised = a.indexing(index);
                if (isNaN(compromised)) {
                    return;
                }
                a._array.splice(compromised, 1);
            };
            CCircularArray.prototype.each = function (fn, offset) {
                var a = this;
                var index = offset ? a.indexing(a._idxCur + offset) : a._idxCur;
                var need = false;
                do {
                    need = fn(index, a.at(index));
                    index = a.indexing(index + 1);
                } while (need && index != a._idxCur);
            };
            CCircularArray.prototype.eachReverse = function (fn) {
                var a = this;
                var index = a._idxCur;
                var need = false;
                do {
                    need = fn(index, a.at(index));
                    index = a.indexing(index - 1);
                } while (need && index != a._idxCur);
            };
            CCircularArray.prototype.eachArray = function (fn) {
                var a = this;
                var index = a._idxCur;
                var need = false;
                do {
                    if (index < a._array.length) {
                        need = fn(index, a.at(index));
                    }
                    else {
                        need = fn(-1, undefined);
                    }
                    index++;
                } while (need);
            };
            CCircularArray.prototype.length = function () {
                var a = this;
                var ret = 0;
                if (a._array) {
                    ret = a._array.length;
                }
                return ret;
            };
            CCircularArray.prototype.clone = function () {
                var a = this;
                var ret = new CCircularArray(a._array);
                ret.setCur(a._idxCur);
                return ret;
            };
            return CCircularArray;
        })();
        var KCssPropTransition = 'transition';
        var KCssEvntTransitionEnd = 'transitionend';
        var KCssTransitionDuration = 'transition-duration';
        var browser = (function () {
            var N = navigator.appName, ua = navigator.userAgent, tem;
            var M = ua.match(/(opera|chrome|safari|firefox|msie)\/?\s*(\.?\d+(\.\d+)*)/i);
            if (M && (tem = ua.match(/version\/([\.\d]+)/i)) != null)
                M[2] = tem[1];
            M = M ? [M[1], M[2]] : [N, navigator.appVersion, '-?'];
            return M;
        }());
        if (browser[0] == "chrome") {
            KCssPropTransition = '-webkit-transition';
            KCssEvntTransitionEnd = 'webkitTransitionEnd';
            KCssTransitionDuration = '-webkit-transition-duration';
        }
        else if (browser[0] == "opera") {
            KCssPropTransition = '-o-transition';
            KCssEvntTransitionEnd = 'oTransitionEnd otransitionend';
            KCssTransitionDuration = '-o-transition-duration';
        }
        else if (browser[0] == "msie") {
            KCssPropTransition = '-ms-transition';
            KCssEvntTransitionEnd = 'msTransitionEnd mstransitionend';
            KCssTransitionDuration = '-ms-transition-duration';
        }
        var KCssTransitionParamPos = 'top .5s linear, left .5s linear';
        var KCssTransitionParamOpa = 'opacity .3s linear';
        var KCssTransitionParamSize = 'width .5s linear, height .5s linear';
        CtrlBase.TCssStr = {
            KCssPropTransition: KCssPropTransition,
            KCssEvntTransitionEnd: KCssEvntTransitionEnd,
            KCssTransitionDuration: KCssTransitionDuration,
            KCssTransitionParamPos: KCssTransitionParamPos,
            KCssTransitionParamOpa: KCssTransitionParamOpa,
            KCssTransitionParamSize: KCssTransitionParamSize
        };
        var Util = (function () {
            function Util() {
            }
            Util.afterTransition = function (aElement, aCallBack, aWithEvent) {
                var durations = ['0'];
                if (getComputedStyle && getComputedStyle(aElement).transitionDuration) {
                    durations = getComputedStyle(aElement).transitionDuration.split(',');
                }
                else {
                    durations = aElement.style.transitionDuration.split(',');
                }
                var delay = parseFloat(durations[0].replace('s', '')) * 1000 || 300;
                if (delay) {
                    if (aWithEvent) {
                        var bRunCallback = false;
                        var timeout;
                        var transitionEndedHandler;
                        timeout = setTimeout(function () {
                            aCallBack();
                            bRunCallback = true;
                        }, delay * 2);
                        aElement.addEventListener(KCssEvntTransitionEnd, transitionEndedHandler = function (e) {
                            if (!bRunCallback) {
                                aCallBack();
                            }
                            clearTimeout(timeout);
                            aElement.removeEventListener(KCssEvntTransitionEnd, transitionEndedHandler);
                        });
                    }
                    else {
                        setTimeout(aCallBack, delay);
                    }
                }
                else {
                    throw "Invalid transiton duration";
                }
            };
            Util.remove = function (aElement) {
                var parent = aElement.parentElement;
                if (parent) {
                    parent.removeChild(aElement);
                }
            };
            Util.prepend = function (aElement, aNewChild) {
                aElement.insertBefore(aNewChild, aElement.firstElementChild);
            };
            Util.getRect = function (aElement) {
                return new TRect({
                    top: aElement.offsetTop,
                    left: aElement.offsetLeft,
                    right: aElement.offsetLeft + aElement.offsetWidth,
                    bottom: aElement.offsetTop + aElement.offsetHeight
                });
            };
            return Util;
        })();
        CtrlBase.Util = Util;
        var TRect = (function () {
            function TRect(aParam) {
                this.top = 0;
                this.left = 0;
                this.right = 0;
                this.bottom = 0;
                if (aParam) {
                    this.top = aParam.top;
                    this.left = aParam.left;
                    this.right = aParam.right;
                    this.bottom = aParam.bottom;
                }
            }
            TRect.prototype.getHeight = function () {
                return this.bottom - this.top;
            };
            TRect.prototype.getWidth = function () {
                return this.right - this.left;
            };
            TRect.prototype.getCenterX = function () {
                return this.left + Math.floor(this.getWidth() / 2);
            };
            TRect.prototype.getCenterY = function () {
                return this.top + Math.floor(this.getHeight() / 2);
            };
            TRect.prototype.hasSize = function () {
                return this.getHeight() > 0 || this.getWidth() > 0;
            };
            TRect.prototype.setHeight = function (aHeight) {
                this.bottom = this.top + aHeight;
            };
            TRect.prototype.setWidth = function (aWidth) {
                this.right = this.left + aWidth;
            };
            TRect.prototype.moveTop = function (aIncrement) {
                this.top += aIncrement;
                this.bottom += aIncrement;
            };
            TRect.prototype.moveLeft = function (aIncrement) {
                this.left += aIncrement;
                this.right += aIncrement;
            };
            TRect.prototype.getIntersectedRect = function (aRect) {
                var ret = new TRect;
                var x0 = Math.max(this.left, aRect.left);
                var x1 = Math.min(this.right, aRect.right);
                if (x0 <= x1) {
                    var y0 = Math.max(this.top, aRect.top);
                    var y1 = Math.min(this.bottom, aRect.bottom);
                    if (y0 <= y1) {
                        ret.left = x0;
                        ret.top = y0;
                        ret.right = x1;
                        ret.bottom = y1;
                        return ret;
                    }
                }
                return null;
            };
            return TRect;
        })();
        CtrlBase.TRect = TRect;
        var KClassFocusable = "-f";
        var KClassFocusDisable = "-f-d";
        var KClassFocused = "-fd";
        var KClassActiveFocused = "-afd";
        var KClassActiveFocusedLeaf = "-afd-leaf";
        CtrlBase.TClassStr = {
            KClassFocusable: KClassFocusable,
            KClassFocused: KClassFocused,
            KClassActiveFocused: KClassActiveFocused,
            KClassActiveFocusedLeaf: KClassActiveFocusedLeaf
        };
        var KKeyStrUp = "Up";
        var KKeyStrDown = "Down";
        var KKeyStrLeft = "Left";
        var KKeyStrRight = "Right";
        var KKeyStrEnter = "Enter";
        var KKeyStrChannelUp = "ChannelUp";
        var KKeyStrChannelDown = "ChannelDown";
        var KKeyStrBack = "Back";
        var KKeyStrEscape = "Escape";
        var KKeyUp = 38;
        var KKeyDown = 40;
        var KKeyLeft = 37;
        var KKeyRight = 39;
        var KKeyEnter = 13;
        var KKeyChannelUp = 427;
        var KKeyChannelDown = 428;
        var KKeyBack = 8;
        var KKeyEscape = 27;
        CtrlBase.TKeycode = {
            KKeyUp: 38,
            KKeyDown: 40,
            KKeyLeft: 37,
            KKeyRight: 39,
            KKeyEnter: 13,
            KKeyChannelUp: 427,
            KKeyChannelDown: 428,
            KKeyBack: 8,
            KKeyEscape: 27
        };
        var keyStrTable = {}; // Workaround: http://stackoverflow.com/questions/14994406/typescript-using-number-as-key-in-map
        keyStrTable[KKeyUp] = KKeyStrUp;
        keyStrTable[KKeyDown] = KKeyStrDown;
        keyStrTable[KKeyLeft] = KKeyStrLeft;
        keyStrTable[KKeyRight] = KKeyStrRight;
        keyStrTable[KKeyEnter] = KKeyStrEnter;
        keyStrTable[KKeyChannelUp] = KKeyStrChannelUp;
        keyStrTable[KKeyChannelDown] = KKeyStrChannelUp;
        keyStrTable[KKeyBack] = KKeyStrBack;
        keyStrTable[KKeyEscape] = KKeyStrEscape;
        var eModeKeyMap;
        (function (eModeKeyMap) {
            eModeKeyMap[eModeKeyMap["NORMAL"] = 0] = "NORMAL";
            eModeKeyMap[eModeKeyMap["MOVE"] = 1] = "MOVE";
        })(eModeKeyMap || (eModeKeyMap = {}));
        var CKeyMap = (function () {
            function CKeyMap(aFocusChanged, aActiveFocusClass) {
                this._index = -1;
                this._map = [];
                this._mode = 0 /* NORMAL */;
                this._focusChanged = aFocusChanged;
                this._activeFocusClass = aActiveFocusClass;
            }
            CKeyMap.prototype.destroy = function () {
                this._map = null;
            };
            CKeyMap.prototype.addMapItem = function (aMapItem) {
                this._map.push(aMapItem);
            };
            CKeyMap.prototype.getMapItem = function (aIndex) {
                return this._map[aIndex];
            };
            CKeyMap.prototype.setActiveFocus = function (aIndex) {
                this._index = aIndex;
            };
            CKeyMap.prototype.getFocusedElement = function () {
                if (this._map && this._map.length) {
                    return this._map[this._index].el;
                }
                return null;
            };
            CKeyMap.prototype.getFocusedIndex = function () {
                return this._index;
            };
            CKeyMap.prototype.getMapCount = function () {
                return this._map.length;
            };
            CKeyMap.prototype.getIndex = function (aElement) {
                var index = -1;
                var i, len, item;
                for (i = 0, len = this._map.length; i < len; i++) {
                    item = this._map[i];
                    if (item.el == aElement) {
                        index = i;
                        break;
                    }
                }
                return index;
            };
            CKeyMap.prototype.changeFocus = function (aNewIndex) {
                var oldMapItem = this._map[this._index];
                var newMapItem = this._map[aNewIndex];
                var oldEl = oldMapItem.el;
                var newEl = newMapItem.el;
                oldEl.classList.remove(KClassFocused);
                newEl.classList.add(KClassFocused);
                if (oldEl.classList.contains(this._activeFocusClass)) {
                    oldEl.classList.remove(this._activeFocusClass);
                    newEl.classList.add(this._activeFocusClass);
                }
                this._index = aNewIndex;
                this._focusChanged(oldEl, newEl);
            };
            CKeyMap.prototype.doKey = function (aKeyStr) {
                var handlers = {};
                var oldMapItem = this._map[this._index];
                var newIndex;
                handlers[KKeyStrUp] = function () {
                    newIndex = oldMapItem.u;
                };
                handlers[KKeyStrDown] = function () {
                    newIndex = oldMapItem.d;
                };
                handlers[KKeyStrLeft] = function () {
                    newIndex = oldMapItem.l;
                };
                handlers[KKeyStrRight] = function () {
                    newIndex = oldMapItem.r;
                };
                if (handlers[aKeyStr] && oldMapItem) {
                    handlers[aKeyStr]();
                    if (newIndex !== undefined) {
                        if (this._mode === 1 /* MOVE */) {
                            this.swap(newIndex);
                        }
                        else {
                            this.changeFocus(newIndex);
                        }
                        return true;
                    }
                }
                return false;
            };
            CKeyMap.prototype.setMode = function (aMode) {
                var dirty = this._mode !== aMode;
                this._mode = aMode;
                return dirty;
            };
            CKeyMap.prototype.swap = function (aNewIndex) {
                var oldMapItem = this._map[this._index];
                var newMapItem = this._map[aNewIndex];
                var tempEl = newMapItem.el;
                var tempPos = newMapItem.el.style.left;
                newMapItem.el.style.left = oldMapItem.el.style.left;
                oldMapItem.el.style.left = tempPos;
                newMapItem.el = oldMapItem.el;
                oldMapItem.el = tempEl;
                this._index = aNewIndex;
            };
            CKeyMap.prototype.isMovable = function (aKeyStr) {
                var current = this._map[this._index];
                if (current) {
                    switch (aKeyStr) {
                        case KKeyStrUp:
                            return current.u !== undefined;
                        case KKeyStrDown:
                            return current.d !== undefined;
                        case KKeyStrLeft:
                            return current.l !== undefined;
                        case KKeyStrRight:
                            return current.r !== undefined;
                    }
                }
                return false;
            };
            CKeyMap.prototype.getNextMovableIndex = function (aKeyStr) {
                var current = this._map[this._index];
                var ret = -1;
                switch (aKeyStr) {
                    case KKeyStrUp:
                        if (current.u !== undefined) {
                            ret = current.u;
                        }
                        break;
                    case KKeyStrDown:
                        if (current.d !== undefined) {
                            ret = current.d;
                        }
                        break;
                    case KKeyStrLeft:
                        if (current.l !== undefined) {
                            ret = current.l;
                        }
                        break;
                    case KKeyStrRight:
                        if (current.r !== undefined) {
                            ret = current.r;
                        }
                        break;
                }
                return ret;
            };
            CKeyMap.prototype.getNextMovableElement = function (aKeyStr) {
                var index = this.getNextMovableIndex(aKeyStr);
                if (index != -1) {
                    return this._map[index].el;
                }
                return null;
            };
            CKeyMap.prototype.setMovableItem = function (aIndex, aDataIndex, aMaxRow, aDirection) {
                var row = aDataIndex % aMaxRow;
                var col = Math.floor(aDataIndex / aMaxRow);
                var item;
                var i;
                var ret = -1;
                var el;
                var listCandidate = [];
                var cloneDataIndex = aDataIndex;
                if (aDirection === "l") {
                    while (cloneDataIndex >= aMaxRow) {
                        listCandidate.push(cloneDataIndex -= aMaxRow);
                    }
                }
                else {
                    for (i = aDataIndex % aMaxRow - 1; i >= 0; i--) {
                        listCandidate.unshift(aDataIndex - i - 1);
                    }
                }
                if (listCandidate.length === 0) {
                    return ret;
                }
                for (i = aIndex - 1; i >= 0; i--) {
                    item = this._map[i];
                    if (item && item.el) {
                        el = item.el;
                        ret = listCandidate.indexOf(parseInt(el.getAttribute("data")));
                        if (ret > -1) {
                            if (aDirection === "l") {
                                this._map[i].r = aIndex;
                            }
                            else {
                                this._map[i].d = aIndex;
                            }
                            ret = i;
                            break;
                        }
                    }
                }
                return ret;
            };
            return CKeyMap;
        })();
        CtrlBase.CKeyMap = CKeyMap;
        CtrlBase.KBuilderTopDown = function (aKeyMap, aFocusable, aPrevFocusInfo, aPrevKeyStr) {
            var i, len, el, mapItem, prevMapItem = null;
            var startIndex = -1;
            if (aFocusable.length) {
                startIndex = 0;
                for (i = 0, len = aFocusable.length; i < len; i++) {
                    el = aFocusable[i];
                    mapItem = {
                        el: el
                    };
                    if (i >= 0 && prevMapItem) {
                        if (el.classList.contains(KClassFocused)) {
                            startIndex = i;
                        }
                        prevMapItem.d = i;
                        mapItem.u = i - 1;
                    }
                    aKeyMap.addMapItem(mapItem);
                    prevMapItem = mapItem;
                }
                if (startIndex < 0 && aPrevFocusInfo) {
                    for (i = 0; i < len; i++) {
                        if (parseInt(aFocusable[i].style.top) === aPrevFocusInfo.rect.top) {
                            startIndex = i;
                        }
                    }
                }
                aFocusable[startIndex].classList.add(KClassFocused);
                aKeyMap.setActiveFocus(startIndex);
            }
        };
        CtrlBase.KBuilderLeftRight = function (aKeyMap, aFocusable, aPrevFocusInfo, aPrevKeyStr) {
            var i, j, len, el, mapItem, prevMapItem = null;
            var startIndex = 0;
            if (aFocusable.length) {
                for (i = 0, len = aFocusable.length; i < len; i++) {
                    el = aFocusable[i];
                    mapItem = {
                        el: el
                    };
                    if (i && prevMapItem) {
                        if (el.classList.contains(KClassFocused)) {
                            startIndex = i;
                        }
                        prevMapItem.r = i;
                        mapItem.l = i - 1;
                    }
                    aKeyMap.addMapItem(mapItem);
                    prevMapItem = mapItem;
                }
                aFocusable[startIndex].classList.add(KClassFocused);
                aKeyMap.setActiveFocus(startIndex);
            }
        };
        CtrlBase.KBuilderGrid = function (aKeyMap, aFocusable, aPrevFocusInfo, aPrevKeyStr, aHint) {
            if (aHint) {
                var temporary = true;
                var focusableLen = aFocusable.length;
                var itemIndex = 0;
                var itemData = parseInt(aFocusable[itemIndex].getAttribute('data'));
                var startData = parseInt(aFocusable[itemIndex].getAttribute('data'));
                var endData = parseInt(aFocusable[focusableLen - 1].getAttribute('data'));
                var startIndex = 0;
                var startDataIndex = startData;
                var i, j, rowLen = aHint.length;
                var findindex = function (aData) {
                    var ret = -1;
                    var len = aFocusable.length;
                    var i;
                    for (i = 0; i < len; i++) {
                        if (parseInt(aFocusable[i].getAttribute('data')) === aData) {
                            ret = i;
                            break;
                        }
                    }
                    return ret;
                };
                if (temporary) {
                    for (i = 0; i < startData; i++) {
                        aKeyMap.addMapItem(null);
                    }
                }
                for (i = 0; i < rowLen; i++) {
                    var colBlock = aHint[i];
                    var colLen = colBlock.length;
                    for (j = 0; j < colLen; j++) {
                        if (itemData === colBlock[j]) {
                            var mapItem = {
                                el: aFocusable[itemIndex]
                            };
                            if (mapItem.el.classList.contains(KClassFocused)) {
                                startIndex = itemIndex;
                                startDataIndex = itemData;
                            }
                            // up
                            if (i > 0 && aHint[i - 1][j] !== undefined) {
                                var t = 1;
                                while (i - t >= 0) {
                                    var temp = aHint[i - t][j];
                                    if (temp >= startData && temp <= endData && temp !== itemData) {
                                        if (temporary) {
                                            mapItem.u = temp;
                                        }
                                        else {
                                            mapItem.u = findindex(temp);
                                        }
                                        break;
                                    }
                                    else {
                                        t++;
                                        continue;
                                    }
                                }
                            }
                            // down
                            if (i < rowLen - 1 && aHint[i + 1][j] !== undefined) {
                                var t = 1;
                                while (i + t <= rowLen - 1) {
                                    var temp = aHint[i + t][j];
                                    if (temp >= startData && temp <= endData && temp !== itemData) {
                                        if (temporary) {
                                            mapItem.d = temp;
                                        }
                                        else {
                                            mapItem.d = findindex(temp);
                                        }
                                        break;
                                    }
                                    else {
                                        t++;
                                        continue;
                                    }
                                }
                            }
                            // left
                            if (j > 0 && aHint[i][j - 1] !== undefined) {
                                var t = 1;
                                while (j - t >= 0) {
                                    var temp = aHint[i][j - t];
                                    if (temp >= startData && temp <= endData && temp !== itemData) {
                                        if (temporary) {
                                            mapItem.l = temp;
                                        }
                                        else {
                                            mapItem.l = findindex(temp);
                                        }
                                        break;
                                    }
                                    else {
                                        t++;
                                        continue;
                                    }
                                }
                            }
                            // right
                            if (j < colLen - 1 && aHint[i][j + 1] !== undefined) {
                                var t = 1;
                                while (j + t <= colLen - 1) {
                                    var temp = aHint[i][j + t];
                                    if (temp >= startData && temp <= endData && temp !== itemData) {
                                        if (temporary) {
                                            mapItem.r = temp;
                                        }
                                        else {
                                            mapItem.r = findindex(temp);
                                        }
                                        break;
                                    }
                                    else {
                                        t++;
                                        continue;
                                    }
                                }
                            }
                            aKeyMap.addMapItem(mapItem);
                            if (itemIndex < focusableLen - 1) {
                                itemData = parseInt(aFocusable[++itemIndex].getAttribute('data'));
                            }
                        }
                    }
                }
                aFocusable[startIndex].classList.add(KClassFocused);
                if (temporary) {
                    aKeyMap.setActiveFocus(startDataIndex);
                }
                else {
                    aKeyMap.setActiveFocus(startIndex);
                }
            }
            else {
                var i, j, len;
                var el;
                var mapItem;
                var prevMapItem = null;
                var aboveMapItem = null;
                var startIndex = 0;
                var rowCount = 0;
                var colCount = 0;
                var posY = -1;
                for (i = 0, len = aFocusable.length; i < len; i++) {
                    el = aFocusable[i];
                    mapItem = {
                        el: el
                    };
                    if (el.classList.contains(KClassFocused)) {
                        startIndex = i;
                    }
                    var clientRect = el.getBoundingClientRect();
                    if (i === 0) {
                        aKeyMap.addMapItem(mapItem);
                        posY = clientRect.top;
                        continue;
                    }
                    if (posY === clientRect.top) {
                        prevMapItem = aKeyMap.getMapItem(i - 1);
                        prevMapItem.r = i;
                        mapItem.l = i - 1;
                    }
                    else {
                        posY = clientRect.top;
                        if (!colCount) {
                            colCount = i;
                        }
                        rowCount++;
                    }
                    if (rowCount) {
                        aboveMapItem = aKeyMap.getMapItem(i - colCount);
                        aboveMapItem.d = i;
                        mapItem.u = i - colCount;
                    }
                    aKeyMap.addMapItem(mapItem);
                }
                aFocusable[startIndex].classList.add(KClassFocused);
                aKeyMap.setActiveFocus(startIndex);
            }
            if (aPrevKeyStr) {
                aKeyMap.doKey(aPrevKeyStr);
            }
        };
        CtrlBase.KBuilderGridHorizontal = function (aKeyMap, aFocusable, aPrevFocusInfo, aPrevKeyStr, aMAX_ROW) {
            var i, len;
            var el;
            var mapItem;
            var prevMapItem = null;
            var aboveMapItem = null;
            var startIndex = 0;
            var rowCount = 0;
            var colCount = 0;
            var posX = -1;
            var posY = -1;
            var dataIndex;
            var row;
            var col;
            for (i = 0, len = aFocusable.length; i < len; i++) {
                el = aFocusable[i];
                dataIndex = parseInt(el.getAttribute("data"));
                mapItem = {
                    el: el
                };
                if (el.classList.contains(KClassFocused)) {
                    startIndex = i;
                }
                var clientRect = el.getBoundingClientRect();
                if (i === 0) {
                    aKeyMap.addMapItem(mapItem);
                    continue;
                }
                row = dataIndex % aMAX_ROW;
                col = Math.floor(dataIndex / aMAX_ROW);
                /*
                 * self set property: u, l;
                 */
                var relativeMapIndex;
                if (row > 0) {
                    //TODO: find <nearest up item> set d: i; self sef u: <nearest up item>
                    //TODO: set: u
                    relativeMapIndex = aKeyMap.setMovableItem(i, dataIndex, aMAX_ROW, "u");
                    if (relativeMapIndex !== -1) {
                        mapItem.u = relativeMapIndex;
                    }
                }
                if (col > 0) {
                    //TODO: <find nearest left item> set r: i; self set l: <find nearest left item>
                    relativeMapIndex = aKeyMap.setMovableItem(i, dataIndex, aMAX_ROW, "l");
                    //TODO: set: l
                    if (relativeMapIndex !== -1) {
                        mapItem.l = relativeMapIndex;
                    }
                }
                /*
                if (posY === clientRect.top) {
                    prevMapItem = aKeyMap.getMapItem(i - 1);
                    prevMapItem.r = i;
                    mapItem.l = i - 1;
                } else {
                    posY = clientRect.top;
                    posX = clientRect.left;
                    if (!rowCount) {
                        rowCount = i % rowCount;
                    }
                    colCount = (i || 1) / rowCount;
                }
                if (rowCount) {
    
                }
                if (colCount) {
                    aboveMapItem = aKeyMap.getMapItem(i - rowCount);
                    aboveMapItem.d = i;
                    mapItem.u = i - colCount;
                }
                */
                aKeyMap.addMapItem(mapItem);
            }
            aFocusable[startIndex].classList.add(KClassFocused);
            aKeyMap.setActiveFocus(startIndex);
            if (aPrevKeyStr) {
                aKeyMap.doKey(aPrevKeyStr);
            }
        };
        CtrlBase.KBuilderWeightDistance = function (aKeyMap, aFocusable, aPrevFocusInfo, aPrevKeyStr) {
            var i, j, cnt = aFocusable.length, el;
            var getPos = function (aElement) {
                var clientRect = aElement.getBoundingClientRect();
                var top = clientRect.top;
                var left = clientRect.left;
                var width = clientRect.width;
                var height = clientRect.height;
                var bottom = top + height;
                var right = left + width;
                var topMiddle = top + (height / 2);
                var leftMiddle = left + (width / 2);
                return {
                    top: { x: leftMiddle, y: top },
                    right: { x: right, y: topMiddle },
                    bottom: { x: leftMiddle, y: bottom },
                    left: { x: left, y: topMiddle },
                    x: leftMiddle,
                    y: topMiddle
                };
            };
            var getPosByRect = function (aRect) {
                var top = aRect.top;
                var left = aRect.left;
                var right = aRect.right;
                var bottom = aRect.bottom;
                var width = aRect.getWidth();
                var height = aRect.getHeight();
                var topMiddle = top + (height / 2);
                var leftMiddle = left + (width / 2);
                return {
                    top: { x: leftMiddle, y: top },
                    right: { x: right, y: topMiddle },
                    bottom: { x: leftMiddle, y: bottom },
                    left: { x: left, y: topMiddle },
                    x: leftMiddle,
                    y: topMiddle
                };
            };
            var startIndex = 0;
            var rectInfoList = [];
            for (i = 0; i < cnt; i++) {
                el = aFocusable[i];
                if (el.classList.contains(KClassFocused)) {
                    startIndex = i;
                }
                rectInfoList.push(getPos(el));
            }
            var distanceLeft = function (me, some, findArea) {
                var me = me;
                var some = some;
                var distance;
                if (me != some) {
                    if (me.top.y < some.right.y && me.bottom.y > some.right.y) {
                        if (me != some) {
                            if (me.right.x >= some.right.x) {
                                distance = Math.abs(me.left.x - some.right.x);
                                return distance;
                            }
                        }
                    }
                }
                return Number.MAX_VALUE;
            };
            var distanceRight = function (me, some, findArea) {
                var me = me;
                var some = some;
                var distance;
                if (me != some) {
                    if (me.top.y < some.left.y && me.bottom.y > some.left.y) {
                        if (me != some) {
                            if (me.left.x <= some.left.x) {
                                distance = Math.abs(me.right.x - some.left.x);
                                return distance;
                            }
                        }
                    }
                }
                return Number.MAX_VALUE;
            };
            var distanceTop = function (me, some, findArea) {
                var me = me;
                var some = some;
                var distance;
                if (me != some) {
                    if (me.top.y >= some.bottom.y && me.top.y - findArea <= some.bottom.y) {
                        distance = weightDistance(me, some, 1);
                        return distance;
                    }
                }
                return Number.MAX_VALUE;
            };
            var distanceBottom = function (me, some, findArea) {
                var me = me;
                var some = some;
                var distance;
                if (me != some) {
                    if (me.bottom.y <= some.top.y && me.bottom.y + findArea >= some.top.y) {
                        distance = weightDistance(me, some, 1);
                        return distance;
                    }
                }
                return Number.MAX_VALUE;
            };
            var weightDistance = function (me, some, xWeight) {
                var me = me;
                var some = some;
                var xWeight = xWeight;
                var yWeight = 10 - xWeight;
                var distance;
                distance = Math.sqrt(Math.pow(me.x - some.x, 2) / yWeight + Math.pow(me.y - some.y, 2) / xWeight);
                return distance;
            };
            var fillKeyMapItem = function (aKeyMapItem, aSourcePosData) {
                var valNearestLeft = Number.MAX_VALUE;
                var idxNearestLeft = -1;
                var valNearestRight = Number.MAX_VALUE;
                var idxNearestRight = -1;
                var valNearestTop = Number.MAX_VALUE;
                var idxNearestTop = -1;
                var valNearestBot = Number.MAX_VALUE;
                var idxNearestBot = -1;
                var findArea = 46;
                for (j = 0; j < cnt; j++) {
                    var some = rectInfoList[j];
                    var resultTopVal = distanceTop(aSourcePosData, some, findArea);
                    var resultLeftVal = distanceLeft(aSourcePosData, some, findArea);
                    var resultBottomVal = distanceBottom(aSourcePosData, some, findArea);
                    var resultRightVal = distanceRight(aSourcePosData, some, findArea);
                    if (valNearestTop > resultTopVal) {
                        valNearestTop = resultTopVal;
                        idxNearestTop = j;
                    }
                    if (valNearestLeft > resultLeftVal) {
                        valNearestLeft = resultLeftVal;
                        idxNearestLeft = j;
                    }
                    if (valNearestBot > resultBottomVal) {
                        valNearestBot = resultBottomVal;
                        idxNearestBot = j;
                    }
                    if (valNearestRight > resultRightVal) {
                        valNearestRight = resultRightVal;
                        idxNearestRight = j;
                    }
                }
                //result
                if (!aKeyMapItem.l && idxNearestLeft != -1) {
                    aKeyMapItem.l = idxNearestLeft;
                }
                if (!aKeyMapItem.r && idxNearestRight != -1) {
                    aKeyMapItem.r = idxNearestRight;
                }
                if (!aKeyMapItem.u && idxNearestTop != -1) {
                    aKeyMapItem.u = idxNearestTop;
                }
                if (!aKeyMapItem.d && idxNearestBot != -1) {
                    aKeyMapItem.d = idxNearestBot;
                }
            };
            var fillKeyMapItemAndReturnNearestIndex = function (aKeyMapItem, aSourcePosData) {
                var valNearestLeft = Number.MAX_VALUE;
                var idxNearestLeft = -1;
                var valNearestRight = Number.MAX_VALUE;
                var idxNearestRight = -1;
                var valNearestTop = Number.MAX_VALUE;
                var idxNearestTop = -1;
                var valNearestBot = Number.MAX_VALUE;
                var idxNearestBot = -1;
                var valNearest = Number.MAX_VALUE;
                var idxNearest = -1;
                var findArea = 46;
                for (j = 0; j < cnt; j++) {
                    var some = rectInfoList[j];
                    var resultTopVal = distanceTop(aSourcePosData, some, findArea);
                    var resultLeftVal = distanceLeft(aSourcePosData, some, findArea);
                    var resultBottomVal = distanceBottom(aSourcePosData, some, findArea);
                    var resultRightVal = distanceRight(aSourcePosData, some, findArea);
                    if (valNearestTop > resultTopVal) {
                        valNearestTop = resultTopVal;
                        idxNearestTop = j;
                    }
                    if (valNearestLeft > resultLeftVal) {
                        valNearestLeft = resultLeftVal;
                        idxNearestLeft = j;
                        if (resultLeftVal < valNearest) {
                            valNearest = resultLeftVal;
                            idxNearest = j;
                        }
                    }
                    if (valNearestBot > resultBottomVal) {
                        valNearestBot = resultBottomVal;
                        idxNearestBot = j;
                    }
                    if (valNearestRight > resultRightVal) {
                        valNearestRight = resultRightVal;
                        idxNearestRight = j;
                        if (resultRightVal < valNearest) {
                            valNearest = resultRightVal;
                            idxNearest = j;
                        }
                    }
                }
                //result
                if (!aKeyMapItem.l && idxNearestLeft != -1) {
                    aKeyMapItem.l = idxNearestLeft;
                }
                if (!aKeyMapItem.r && idxNearestRight != -1) {
                    aKeyMapItem.r = idxNearestRight;
                }
                if (!aKeyMapItem.u && idxNearestTop != -1) {
                    aKeyMapItem.u = idxNearestTop;
                }
                if (!aKeyMapItem.d && idxNearestBot != -1) {
                    aKeyMapItem.d = idxNearestBot;
                }
                return idxNearest;
            };
            for (i = 0; i < cnt; i++) {
                el = aFocusable[i];
                var item = {
                    el: el
                };
                var me = rectInfoList[i];
                fillKeyMapItem(item, me);
                aKeyMap.addMapItem(item);
            }
            aKeyMap.setActiveFocus(startIndex);
            if (aPrevFocusInfo) {
                var prevPosData = getPosByRect(aPrevFocusInfo.rect);
                var item = {
                    el: null
                };
                var nearestIndex = fillKeyMapItemAndReturnNearestIndex(item, prevPosData);
                if (aPrevKeyStr) {
                    aKeyMap.doKey(aPrevKeyStr);
                }
                if (aPrevFocusInfo.activeFocus) {
                    aKeyMap.getFocusedElement().classList.add(KClassActiveFocusedLeaf);
                }
            }
            /*
            aFocusable[startIndex].classList.add(KClassFocused);
            aKeyMap.setActiveFocus(startIndex);
            if (this._parent) {
                if (!this._parent.isFocused()) {
                    return;
                }
            }
            if (aPrevFocused) {
                aFocusable[startIndex].classList.add(KClassActiveFocusedLeaf);
            }
            */
        };
        var CSignalSource = (function () {
            function CSignalSource() {
                this._signalTypes = {};
            }
            CSignalSource.prototype.destroy = function () {
                this._signalTypes = null;
            };
            CSignalSource.prototype.registerSignal = function (aSignalList) {
                var i, len, signalName;
                for (i = 0, len = aSignalList.length; i < len; i++) {
                    signalName = aSignalList[i];
                    if (this._signalTypes[signalName]) {
                        throw "Event [" + signalName + "] already exists";
                    }
                    this._signalTypes[signalName] = [];
                }
            };
            CSignalSource.prototype.connect = function (aSignalName, aHolder, aSlotName) {
                if (!(aHolder[aSlotName] instanceof Function)) {
                    throw "holder does not have the slot";
                }
                var signalHandlers;
                var signalHandler;
                signalHandlers = this._signalTypes[aSignalName];
                var i, len;
                for (i = 0, len = signalHandlers.length; i < len; i++) {
                    signalHandler = signalHandlers[i];
                    if (signalHandler.holder == aHolder) {
                        signalHandler.slotName = aSlotName;
                        return;
                    }
                }
                signalHandlers.push({
                    holder: aHolder,
                    slotName: aSlotName
                });
            };
            CSignalSource.prototype.disconnect = function (aHolder) {
                var signalName;
                var signalHandlers;
                var signalHandler;
                var i;
                for (signalName in this._signalTypes) {
                    if (this._signalTypes.hasOwnProperty(signalName)) {
                        signalHandlers = this._signalTypes[signalName];
                        for (i = signalHandlers.length - 1; i >= 0; i--) {
                            signalHandler = signalHandlers[i];
                            if (signalHandler.holder == aHolder) {
                                signalHandlers.splice(i, 1);
                                break;
                            }
                        }
                    }
                }
            };
            CSignalSource.prototype.length = function (aSignalName) {
                return this._signalTypes[aSignalName].length;
            };
            CSignalSource.prototype.emit = function () {
                var args = [];
                for (var _i = 0; _i < arguments.length; _i++) {
                    args[_i - 0] = arguments[_i];
                }
                var ret = false;
                var signalName;
                var i, len, handlerInfoList, handlerInfo;
                var holder;
                signalName = args.shift();
                handlerInfoList = this._signalTypes[signalName];
                for (i = 0, len = handlerInfoList.length; i < len; i++) {
                    handlerInfo = handlerInfoList[i];
                    handlerInfo.holder[handlerInfo.slotName].apply(handlerInfo.holder, args);
                    ret = true;
                }
                return ret;
            };
            return CSignalSource;
        })();
        CtrlBase.CSignalSource = CSignalSource;
        (function (TParamOrientation) {
            TParamOrientation[TParamOrientation["EUnknown"] = 0] = "EUnknown";
            TParamOrientation[TParamOrientation["EVertical"] = 1] = "EVertical";
            TParamOrientation[TParamOrientation["EHorizontal"] = 2] = "EHorizontal";
        })(CtrlBase.TParamOrientation || (CtrlBase.TParamOrientation = {}));
        var TParamOrientation = CtrlBase.TParamOrientation;
        var CClockTimer = (function (_super) {
            __extends(CClockTimer, _super);
            function CClockTimer() {
                var _this = this;
                _super.call(this);
                this._clockTimer = setInterval(function () {
                    _this._clockTimerCallback();
                }, 1000);
                var now = new Date();
                this._clockDate = now;
                this._clockSec = now.getSeconds();
                this._clockMin = now.getMinutes();
                this._clockHour = now.getHours();
                this._clockDay = now.getDate();
                this.registerSignal(["MinuteChanged"]);
            }
            CClockTimer.prototype._clockTimerCallback = function () {
                var now = new Date();
                // for DST time change: this will generate all events
                if (30000 < Math.abs(now.getTime() - this._clockDate.getTime()) || Math.abs(now.getTimezoneOffset() - this._clockDate.getTimezoneOffset())) {
                    console.log('DTG', 'TIME CHANGE DETECTED');
                    this._clockSec = -1;
                    this._clockMin = -1;
                    this._clockHour = -1;
                    this._clockDay = -1;
                }
                this._clockDate = now;
                if (this._clockSec != now.getSeconds()) {
                    if (this._clockMin != now.getMinutes()) {
                        if (this._clockHour != now.getHours()) {
                            if (this._clockDay != now.getDate()) {
                                this._clockDay = now.getDate();
                            }
                            this._clockHour = now.getHours();
                        }
                        this._clockMin = now.getMinutes();
                        //this.fireEvent(CClockTimer.THandlerType.EMin, this._clockDay, this._clockHour, this._clockMin);
                        this._emitMinuteChanged();
                    }
                    this._clockSec = now.getSeconds();
                }
            };
            CClockTimer.prototype.connectMinuteChanged = function (aHolder, aSlotName, aSlot) {
                this.connect("MinuteChanged", aHolder, aSlotName);
            };
            CClockTimer.prototype._emitMinuteChanged = function () {
                this.emit.call(this, "MinuteChanged");
            };
            return CClockTimer;
        })(CSignalSource);
        CtrlBase.CClockTimer = CClockTimer;
        var CControl = (function (_super) {
            __extends(CControl, _super);
            function CControl(aEl, focusableClassName, focusedClassName, focusChangedEventHandler) {
                _super.call(this);
                /*
                 * Virtual functions for subclasses
                 */
                this.functionClass = null;
                this._drawEngine = null;
                //FIXME: consider draw()
                this._drawParam = {};
                this._drawParamVolitile = {};
                this._shown = false;
                this._hideTimer = null;
                this._children = [];
                this._$self = aEl || $("<div>");
                this._element = this._$self[0];
                this._id = this._$self.attr("id");
                this._transitioning = false;
                this._focusableClassName = KClassFocusable;
                this._focusedClassName = KClassFocused;
                this.registerSignal([
                    "FocusChanged",
                    "ChildFocusChanged",
                    "FocusGained",
                    "FocusLost",
                    "ItemSelected",
                    "TransitioningChanged",
                    "KeyMapUpdated"
                ]);
            }
            CControl.prototype.destroy = function () {
                if (this._keyMap) {
                    this._keyMap.destroy();
                }
                delete this._keyMap;
                this.clearHideAfter();
                if (this._children) {
                    var i, len, child;
                    for (i = 0, len = this._children.length; i < len; i++) {
                        child = this._children[i];
                        child.destroy();
                    }
                }
                else {
                    throw "control double freed";
                }
                delete this._children;
                this._$self.remove();
                delete this._$self;
                delete this._element;
                delete this._parent;
            };
            CControl.prototype.$self = function () {
                return this._$self;
            };
            CControl.prototype.setId = function (aId) {
                this._$self.attr("id", aId);
            };
            CControl.prototype.addClass = function (aClassName) {
                this._$self.addClass(aClassName);
            };
            CControl.prototype.getName = function () {
                return this.name;
            };
            /*
             * Focus management
             */
            CControl.prototype.setActiveFocus = function () {
                this._setActiveFocus(true);
            };
            CControl.prototype.getFocusedElement = function () {
                if (this._keyMap) {
                    return this._keyMap.getFocusedElement();
                }
                return null;
            };
            CControl.prototype.getFocusedElementIndex = function () {
                if (this._keyMap) {
                    return this._keyMap.getFocusedIndex();
                }
                return -1;
            };
            CControl.prototype.getFocusableElementCount = function () {
                var ret = 0;
                if (this._keyMap) {
                    ret = this._keyMap.getMapCount();
                }
                return ret;
            };
            CControl.prototype.getElementByIndex = function (aIndex) {
                if (this._keyMap) {
                    var ret = this._keyMap.getMapItem(aIndex);
                    if (ret) {
                        return ret.el;
                    }
                }
                return null;
            };
            CControl.prototype.getIndexByElement = function (aElement) {
                if (this._keyMap) {
                    return this._keyMap.getIndex(aElement);
                }
                return -1;
            };
            CControl.prototype.getFocusedChild = function () {
                if (this._children.length) {
                    var index = $(this._keyMap.getFocusedElement()).data("index");
                    return this._children[index];
                }
                return null;
            };
            CControl.prototype.getChildIndex = function (aChild) {
                var i, len, c;
                for (i = 0, len = this._children.length; i < len; i++) {
                    c = this._children[i];
                    if (c == aChild) {
                        return i;
                    }
                }
                return -1;
            };
            CControl.prototype.getChildCount = function () {
                return this._children.length;
            };
            CControl.prototype.getChild = function (aIndex) {
                return this._children[aIndex];
            };
            CControl.prototype.setFocusedElement = function (aElement) {
                var ret = false;
                var elementIndex = this._keyMap.getIndex(aElement);
                if (elementIndex != -1) {
                    var focusedIndex = this._keyMap.getFocusedIndex();
                    if (focusedIndex != elementIndex) {
                        this._keyMap.changeFocus(elementIndex);
                        ret = true;
                    }
                }
                return ret;
            };
            CControl.prototype.setFocusedElementByIndex = function (aIndex, aForce) {
                var ret = false;
                if (0 <= aIndex && this._keyMap && aIndex < this._keyMap.getMapCount()) {
                    var focusedIndex = this._keyMap.getFocusedIndex();
                    if ((aForce || focusedIndex != aIndex) && this._keyMap.getMapItem(aIndex)) {
                        this._keyMap.changeFocus(aIndex);
                        ret = true;
                    }
                }
                return ret;
            };
            CControl.prototype.setFocusedChild = function (aControl) {
                var index = this.getChildIndex(aControl);
                if (index != -1) {
                    this.setFocusedElementByIndex(index);
                }
                else {
                    throw "child not found something wrong?";
                }
            };
            CControl.prototype.setFocus = function (focused) {
                try {
                    this.doSetFocus(focused);
                }
                catch (e) {
                    console.log('CControl[' + this.getName() + '].setFocus FAILED with ' + e);
                }
            };
            CControl.prototype.isFocused = function () {
                return this._$self.hasClass(KClassFocused);
            };
            CControl.prototype.isActiveFocused = function () {
                var i, len, c;
                for (i = 0, len = this._children.length; i < len; i++) {
                    c = this._children[i];
                    if (c.isActiveFocused()) {
                        return true;
                    }
                }
                if (this._keyMap) {
                    return $(this._keyMap.getFocusedElement()).hasClass("-afd-leaf");
                }
                return false;
            };
            CControl.prototype.hasFocus = function () {
                return this.doHasFocus();
            };
            CControl.prototype.hasFocusable = function () {
                return this.doHasFocusable();
            };
            /*
             * Animation
             */
            CControl.prototype.isTransitioning = function () {
                return this._transitioning;
            };
            CControl.prototype.isChildTransitioning = function () {
                var i, len, c;
                for (i = 0, len = this._children.length; i < len; i++) {
                    c = this._children[i];
                    if (c.isTransitioning()) {
                        return true;
                    }
                }
                return false;
            };
            CControl.prototype._setTransitioning = function (aTransitioning, aIgnoreCheck) {
                if (aIgnoreCheck === void 0) { aIgnoreCheck = false; }
                if (!aIgnoreCheck && this._transitioning == aTransitioning) {
                    console.error("settting transitioning error " + aTransitioning);
                    throw "something wrong?";
                }
                this._transitioning = aTransitioning;
                this._emitTransitioningChanged(aTransitioning);
            };
            CControl.prototype.setTransition = function (aTransition) {
                this._setTransitioning(aTransition);
            };
            /*
             * Control visibility utility
             */
            CControl.prototype.isShown = function () {
                return this._shown;
            };
            CControl.prototype.show = function (param) {
                if (this.isTransitioning()) {
                    return false;
                }
                this._shown = false;
                if (this._hideTimer) {
                    clearTimeout(this._hideTimer);
                }
                if (this.doShow(param)) {
                    this._shown = true;
                    return true;
                }
                return false;
            };
            CControl.prototype.doShow = function (param) {
                throw 'doShow is not implemented!';
            };
            CControl.prototype.hide = function (param) {
                if (this.isTransitioning()) {
                    return false;
                }
                if (!this._shown) {
                    return true;
                }
                this._shown = true;
                this.clearHideAfter();
                if (this.doHide(param)) {
                    this._shown = false;
                    return true;
                }
                return false;
            };
            CControl.prototype.doHide = function (param) {
                throw 'doHide is not implemented!';
            };
            CControl.prototype.hideAfter = function (milisecond) {
                var _this = this;
                if (this._hideTimer) {
                    clearTimeout(this._hideTimer);
                }
                this._hideTimer = setTimeout(function () {
                    _this.hide({ timedOut: true });
                }, milisecond);
            };
            CControl.prototype.clearHideAfter = function () {
                if (this._hideTimer) {
                    clearTimeout(this._hideTimer);
                }
            };
            /*
             * Child control utilities
             */
            CControl.prototype.addControl = function (control, bFirst) {
                if (bFirst === void 0) { bFirst = false; }
                if (bFirst) {
                    this._children.unshift(control);
                }
                else {
                    this._children.push(control);
                }
            };
            CControl.prototype.removeControl = function (control) {
                var i;
                for (i = 0; i < this._children.length; i += 1) {
                    var child = this._children[i];
                    if (child == control) {
                        child._setActiveFocus(false);
                        this._children.splice(i, 1);
                        return true;
                    }
                }
                return false;
            };
            CControl.prototype.removeAllControl = function (buildNaviMap) {
                this._children = [];
                if (buildNaviMap) {
                    this.buildNaviMapForChild(CtrlBase.KBuilderTopDown);
                }
            };
            CControl.prototype.hasControl = function (name) {
                var rv = false;
                var i, max;
                max = this._children.length;
                for (i = 0; i < max; i++) {
                    if (this._children[i].getName() == name)
                        rv = true;
                }
                return rv;
            };
            CControl.prototype.doKeyForChild = function (aKeyStr, args) {
                var i, childList = [], child;
                for (i = 0; i < this._children.length; i++) {
                    childList.push(this._children[i]);
                }
                for (i = 0; i < childList.length; i++) {
                    child = childList[i];
                    if (child.$self().hasClass(KClassFocused)) {
                        if (child.doKey(aKeyStr, args)) {
                            return true;
                        }
                    }
                }
                return false;
            };
            CControl.prototype.setFocusedControl = function (aChildControl) {
                this.setFocusedElement(aChildControl.$self().get(0));
            };
            CControl.prototype.removeControlAll = function () {
                this._children.length = 0;
            };
            CControl.prototype.releaseKey = function (key, args) {
                var handler = 'releaseKey' + key;
                if (typeof this[handler] != 'undefined') {
                    return this[handler](args);
                }
                return false;
            };
            CControl.prototype.doKey = function (aKeyStr, aArgs) {
                if (this.isTransitioning()) {
                    console.log("key while transitioning");
                    return true;
                }
                /*
                 * COMMENT
                 * for compatibility prism controls version
                 */
                if (typeof aKeyStr === "number") {
                    var keyValue = hx.key[aKeyStr].replace("KEY_", "").split("_");
                    aKeyStr = "";
                    for (var i = 0; i < keyValue.length; i++) {
                        aKeyStr += keyValue[i].slice(0, 1);
                        aKeyStr += keyValue[i].slice(1).toLowerCase();
                    }
                }
                var handled = false;
                var handler = '_doKey' + aKeyStr;
                if (this[handler + 'Debug']) {
                }
                if (this[handler]) {
                    handled = this[handler](aArgs);
                }
                if (!handled && this.hasChild()) {
                    handled = this.doKeyForChild(aKeyStr, aArgs);
                }
                if (!handled) {
                    handled = this.doNaviKey(aKeyStr);
                }
                if (!handled) {
                    handler = handler + "Latent";
                    if (this[handler]) {
                        handled = this[handler](aArgs);
                    }
                }
                return handled;
            };
            /*
             * intrrupt
             */
            CControl.prototype.interrupt = function (type, args) {
                console.log("INTERRUPT", "[" + type + "] {" + args + "}");
                return this.doInterrupt(type, args);
            };
            CControl.prototype.doInterrupt = function (type, args) {
                var children = this._children;
                var handled = false;
                var i, loop;
                for (i = 0, loop = children.length; i < loop; i++) {
                    if (children[i].doInterrupt(type, args)) {
                        handled = true;
                        break;
                    }
                }
                return handled;
            };
            CControl.prototype.hasChild = function () {
                return (this._children && this._children.length) ? true : false;
            };
            CControl.prototype._doFocusChanged = function (aElOld, aElNew) {
                if (this.hasChild()) {
                    var oldIndex = $(aElOld).data("index");
                    var newIndex = $(aElNew).data("index");
                    var oldChild = this._children[oldIndex];
                    var newChild = this._children[newIndex];
                    oldChild._setActiveFocus(false);
                    newChild._setActiveFocus(true);
                    this._doChildFocusChanged(oldChild, newChild);
                }
                this._emitFocusChanged(aElOld, aElNew);
            };
            CControl.prototype._setActiveFocus = function (aFocused) {
                if (this._keyMap) {
                    if (this.hasChild()) {
                        var focusedChild = this.getFocusedChild();
                        if (focusedChild) {
                            focusedChild._setActiveFocus(aFocused);
                        }
                    }
                    else {
                        var focused = this._keyMap.getFocusedElement();
                        if (focused) {
                            if (aFocused) {
                                focused.classList.add(KClassActiveFocusedLeaf);
                            }
                            else {
                                focused.classList.remove(KClassActiveFocusedLeaf);
                            }
                        }
                    }
                }
                if (aFocused) {
                    this._doFocusGained();
                }
                else {
                    this._doFocusLost();
                }
            };
            CControl.prototype._doFocusGained = function () {
                //            console.info(this.name + "._doFocusGained");
                this._emitFocusGained();
            };
            CControl.prototype._doFocusLost = function () {
                //            console.info(this.name + "._doFocusLost");
                this._emitFocusLost();
            };
            CControl.prototype._doChildFocusChanged = function (aOld, aNew) {
                //            console.info(this.name + "._doChildFocusChanged");
                this._emitChildFocusChanged(aOld, aNew);
            };
            CControl.prototype.connectFocusChanged = function (aHolder, aSlotName, aSlot) {
                this.connect("FocusChanged", aHolder, aSlotName);
            };
            CControl.prototype._emitFocusChanged = function (aOld, aNew) {
                this.emit.call(this, "FocusChanged", aOld, aNew);
            };
            CControl.prototype.connectChildFocusChanged = function (aHolder, aSlotName, aSlot) {
                this.connect("ChildFocusChanged", aHolder, aSlotName);
            };
            CControl.prototype._emitChildFocusChanged = function (aOld, aNew) {
                this.emit.call(this, "ChildFocusChanged", aOld, aNew);
            };
            CControl.prototype.connectFocusGained = function (aHolder, aSlotName, aSlot) {
                this.connect("FocusGained", aHolder, aSlotName);
            };
            CControl.prototype._emitFocusGained = function () {
                this.emit.call(this, "FocusGained", this);
            };
            CControl.prototype.connectFocusLost = function (aHolder, aSlotName, aSlot) {
                this.connect("FocusLost", aHolder, aSlotName);
            };
            CControl.prototype._emitFocusLost = function () {
                this.emit.call(this, "FocusLost", this);
            };
            CControl.prototype.connectItemSelected = function (aHolder, aSlotName, aSlot) {
                this.connect("ItemSelected", aHolder, aSlotName);
            };
            CControl.prototype._emitItemSelected = function (aIndex, aEl) {
                this.emit.call(this, "ItemSelected", this, aIndex, aEl);
            };
            CControl.prototype.connectTransitioningChanged = function (aHolder, aSlotName, aSlot) {
                this.connect("TransitioningChanged", aHolder, aSlotName);
            };
            CControl.prototype._emitTransitioningChanged = function (aTransitioning) {
                this.emit.call(this, "TransitioningChanged", aTransitioning);
            };
            //FIXME added for error
            CControl.prototype.connectItemInserted = function (aHolder, aSlotName, aSlot) {
                this.connect("ItemInserted", aHolder, aSlotName);
            };
            CControl.prototype.connectItemRemoved = function (aHolder, aSlotName, aSlot) {
                this.connect("ItemRemoved", aHolder, aSlotName);
            };
            CControl.prototype.connectKeyMapUpdated = function (aHolder, aSlotName, aSlot) {
                this.connect("KeyMapUpdated", aHolder, aSlotName);
            };
            CControl.prototype._emitKeyMapUpdated = function (aKeyMap) {
                this.emit.call(this, "KeyMapUpdated", aKeyMap);
            };
            /*
             * Key navigation map utilities
             */
            CControl.prototype.isMovable = function (aKeyStr) {
                if (this._keyMap) {
                    return this._keyMap.isMovable(aKeyStr);
                }
                return false;
            };
            CControl.prototype.getNextMovableIndex = function (aKeyStr) {
                if (this._keyMap) {
                    return this._keyMap.getNextMovableIndex(aKeyStr);
                }
                return -1;
            };
            CControl.prototype.getNextMovableElement = function (aKeyStr) {
                if (this._keyMap) {
                    return this._keyMap.getNextMovableElement(aKeyStr);
                }
                return null;
            };
            CControl.prototype.getNextMovableChild = function (aKeyStr) {
                if (this._keyMap && this._children.length) {
                    var index = this._keyMap.getNextMovableIndex(aKeyStr);
                    return this._children[index];
                }
                return null;
            };
            CControl.prototype.buildNaviMap = function (aBuilder) {
                var _this = this;
                var keyMap = new CKeyMap(function (aOld, aNew) {
                    _this._doFocusChanged(aOld, aNew);
                }, this._children.length ? KClassActiveFocused : KClassActiveFocusedLeaf);
                var $focusables = this._$self.find("." + KClassFocusable);
                aBuilder(keyMap, $focusables);
                this._keyMap = keyMap;
                this._keyMapBuilder = aBuilder;
            };
            CControl.prototype.buildNaviMapForChild = function (aBuilder, aFocusedChild) {
                var _this = this;
                if (this._keyMap) {
                    this._keyMap.destroy();
                    this._keyMap = null;
                }
                var i, len = this._children.length;
                if (len) {
                    var child, $child, element, focusables = [];
                    var focusedIndex = this._children.indexOf(aFocusedChild);
                    focusedIndex = focusedIndex == -1 ? 0 : focusedIndex;
                    for (i = 0; i < len; i++) {
                        child = this._children[i];
                        $child = child.$self();
                        $child.addClass(KClassFocusable);
                        if (i == focusedIndex) {
                            $child.addClass(KClassFocused);
                        }
                        else {
                            $child.removeClass(KClassFocused);
                        }
                        $child.data("index", "" + i);
                        element = $child[0];
                        focusables.push(element);
                    }
                    var keyMap = new CKeyMap(function (aOld, aNew) {
                        _this._doFocusChanged(aOld, aNew);
                    }, this._children.length ? KClassActiveFocused : KClassActiveFocusedLeaf);
                    aBuilder(keyMap, focusables);
                    this._keyMap = keyMap;
                    this._keyMapBuilder = aBuilder;
                }
            };
            CControl.prototype.focusedChild = function () {
                var i;
                for (i = 0; i < this._children.length; i++) {
                    var child = this._children[i].$self();
                    if (child.hasClass(this._focusedClassName)) {
                        return this._children[i];
                    }
                }
                return null;
            };
            CControl.prototype.doNaviKey = function (aKeyStr) {
                if (this._keyMap) {
                    return this._keyMap.doKey(aKeyStr);
                }
                return false;
            };
            CControl.prototype._findChildByJq = function ($jq) {
                var i;
                for (i = 0; i < this._children.length; i++) {
                    var child = this._children[i];
                    if (child._id == $jq.attr('id')) {
                        return child;
                    }
                }
                return null;
            };
            CControl.prototype.handleFocusChanged = function ($old, $new) {
                if (this._children.length) {
                    var childOld = this._findChildByJq($old);
                    var childNew = this._findChildByJq($new);
                    if (childOld) {
                        childOld.setFocus(false);
                    }
                    if (childNew) {
                        childNew.setFocus(true);
                    }
                    this.doHandleFocusChanged(childOld, childNew);
                }
                else {
                    if (this.doHandleFocusChanged) {
                        this.doHandleFocusChanged($old, $new);
                    }
                }
            };
            CControl.prototype.doHandleFocusChanged = function (childOld, childNew) {
            };
            /*
             * Clock update event utilities
             */
            //        doClockSecUpdated(second) {
            //        }
            //        doClockMinUpdated(minute) {
            //        }
            //        doClockHourUpdated(hour) {
            //        }
            //        doClockDayUpdated(day) {
            //        }
            /*
             * Key event utilities
             */
            CControl.prototype._doKeyOk = function () {
                var i;
                if (this._focusedClassName) {
                    if (this._children.length) {
                        for (i = 0; i < this._children.length; i++) {
                            var focusedChild = this._children[i].$self();
                            if (focusedChild.hasClass(this._focusedClassName)) {
                                return this.doSelect(focusedChild) ? true : false;
                            }
                        }
                    }
                    else {
                        //var focused = $(this._id + ' .' + this._focusedClassName);
                        var focused = this._$self.find("." + this._focusedClassName);
                        return this.doSelect(focused) ? true : false;
                    }
                }
                return false;
            };
            CControl.prototype._doKeyEnter = function () {
                return this._doKeyOk();
            };
            CControl.prototype.doSelect = function ($aFocusedChild) {
                return false;
            };
            CControl.prototype.doSetFocus = function (focused) {
                if (focused) {
                    if (!this._$self.hasClass(this._focusedClassName)) {
                        this._$self.addClass(this._focusedClassName);
                    }
                }
                else {
                    this._$self.removeClass(this._focusedClassName);
                }
            };
            CControl.prototype.doHasFocus = function () {
                return this._$self.hasClass(this._focusedClassName);
            };
            CControl.prototype.doHasFocusable = function () {
                var ret = this._$self.find('.' + this._focusableClassName).length;
                console.log('CControl[' + this.getName() + '].prototype.doHasFocusable [' + this._focusableClassName + ']' + ret);
                return ret ? true : false;
            };
            CControl.prototype.preLoadImages = function ($imgs, callback) {
                this.images = [];
                var loadedCount = 0;
                var i, length = $imgs.length;
                for (i = 0; i < length; i++) {
                    var img = $imgs[i];
                    var src = '';
                    if (img instanceof HTMLImageElement) {
                        src = img.src;
                    }
                    else {
                        src = img;
                    }
                    var $img = $('<img>');
                    $img.attr('src', src);
                    var img = $img.get(0);
                    if (img.complete) {
                        loadedCount++;
                        if (loadedCount >= length) {
                            if (callback) {
                                callback();
                            }
                        }
                        console.log(StringFormat('CControl.preLoadImages: Total Count - {0}, Loaded Count - {1}', length, loadedCount));
                    }
                    else {
                        img.decode(function () {
                            loadedCount++;
                            if (loadedCount >= length) {
                                if (callback) {
                                    callback();
                                }
                            }
                            console.log('CControl.preLoadImages: Success');
                            console.log(StringFormat('CControl.preLoadImages: Total Count - {0}, Loaded Count - {1}', length, loadedCount));
                        }, function () {
                            loadedCount++;
                            if (loadedCount >= length) {
                                if (callback) {
                                    callback();
                                }
                            }
                            console.log('CControl.preLoadImages: Fail');
                            console.log(StringFormat('CControl.preLoadImages: Total Count - {0}, Loaded Count - {1}', length, loadedCount));
                        });
                    }
                    this.images[src] = $img;
                }
            };
            CControl.prototype._saveFocusInfo = function () {
                if (this._keyMap) {
                    var prevFocusedEl = this._keyMap.getFocusedElement();
                    this._prevFocusInfo = {
                        rect: Util.getRect(prevFocusedEl),
                        activeFocus: prevFocusedEl.classList.contains(KClassActiveFocusedLeaf)
                    };
                }
            };
            CControl.prototype._handleFocusChanged = function (aElOld, aElNew) {
                this._emitFocusChanged(aElOld, aElNew);
            };
            CControl.prototype._keyMapFocusChanged = function (aOld, aNew) {
                this._handleFocusChanged(aOld, aNew);
            };
            CControl.prototype._makeKeyMap = function (drawnElements, aKeepFocus) {
                var _this = this;
                /* setKeepFocus => update
                if (aKeepFocus) {
                    this._saveFocusInfo();
                }
                */
                if (drawnElements && drawnElements.length) {
                    var keyMap = new CKeyMap(function (aOld, aNew) {
                        _this._keyMapFocusChanged(aOld, aNew);
                    }, this._group ? KClassActiveFocused : KClassActiveFocusedLeaf);
                    this._keyMapBuilder(keyMap, drawnElements, this._prevFocusInfo, aKeepFocus ? null : this._prevKeyStr, this._doGetKeyMapHint());
                    this._keyMap = keyMap;
                    this._element.classList.add(KClassFocusable);
                    if (this._prevFocusInfo && this._prevFocusInfo.activeFocus) {
                        this.setActiveFocus();
                    }
                }
                else {
                    this._keyMap = null;
                    this._element.classList.remove(KClassFocusable);
                }
                this._emitKeyMapUpdated(this._keyMap);
            };
            CControl.prototype._doGetKeyMapHint = function () {
                return null;
            };
            CControl.prototype.draw = function (aRect) {
                aRect = aRect || null;
                var param = this._prepareParam();
                var keepFocus = param['keepFocus'];
                //            var drawnElements: HTMLElement[]
                //            drawnElements = this._doDraw(aRect, param);
                this._focusableElement = this._doDraw(aRect, param);
                this._clearVolitile();
                this._makeKeyMap(this._focusableElement, keepFocus);
                this._saveFocusInfo();
            };
            CControl.prototype._doDraw = function (aRect, aDrawParam) {
                var ret;
                if (this._drawEngine) {
                    ret = this._drawEngine(aRect, aDrawParam);
                }
                return ret;
            };
            CControl.prototype.setDrawEngine = function (aFnDrawEngine) {
                this._drawEngine = aFnDrawEngine;
            };
            CControl.prototype.update = function (aFirstCalled) {
                if (aFirstCalled === void 0) { aFirstCalled = true; }
                {
                }
                /* do not use argument, argument used by internal only */
                var keepFocus = this.getKeepFocus();
                var children = this._children;
                var i;
                this._doUpdate(keepFocus);
                for (i = 0; i < children.length; i++) {
                    children[i].update(false);
                }
                if (aFirstCalled) {
                    this.draw();
                }
            };
            CControl.prototype._doUpdate = function (aKeepFocus) {
                if (aKeepFocus === void 0) { aKeepFocus = false; }
                if (aKeepFocus) {
                    this._saveFocusInfo();
                }
            };
            CControl.prototype._clearVolitile = function () {
                this._drawParamVolitile = {};
            };
            CControl.prototype._setDrawParam = function (aParamName, aValue, aVolitile) {
                if (aVolitile) {
                    this._drawParamVolitile[aParamName] = aValue;
                }
                else {
                    this._drawParam[aParamName] = aValue;
                }
            };
            CControl.prototype._getDrawParam = function (aParamName) {
                var ret = this._drawParamVolitile[aParamName];
                if (ret == null) {
                    ret = this._drawParam[aParamName];
                }
                return ret;
            };
            CControl.prototype._prepareParam = function () {
                var ret = {};
                var key;
                for (key in this._drawParam) {
                    if (this._drawParam.hasOwnProperty(key)) {
                        ret[key] = this._drawParam[key];
                    }
                }
                for (key in this._drawParamVolitile) {
                    if (this._drawParamVolitile.hasOwnProperty(key)) {
                        ret[key] = this._drawParamVolitile[key];
                    }
                }
                return ret;
            };
            // Item info
            CControl.prototype.setItemHeight = function (aItemHeight) {
                this._setDrawParam(KParamStrItemHeight, aItemHeight, false);
            };
            CControl.prototype.getItemHeight = function () {
                return this._getDrawParam(KParamStrItemHeight) || false;
            };
            CControl.prototype.setItemWidth = function (aItemWidth) {
                this._setDrawParam(KParamStrItemWidth, aItemWidth, false);
            };
            CControl.prototype.getItemWidth = function () {
                return this._getDrawParam(KParamStrItemWidth) || false;
            };
            // Animaion
            CControl.prototype.setAnimation = function (aAnimation) {
                this._setDrawParam(KParamAnimation, aAnimation, false);
            };
            CControl.prototype.getAnimation = function () {
                return this._getDrawParam(KParamAnimation) || false;
            };
            // Keep focus
            CControl.prototype.setKeepFocus = function (aKeepFocus) {
                this._setDrawParam(KParamKeepFocus, aKeepFocus, true);
            };
            CControl.prototype.getKeepFocus = function () {
                return this._getDrawParam(KParamKeepFocus) || false;
            };
            CControl.prototype.setStartIndex = function (aIndex) {
                this._setDrawParam(KParamStrStartIndex, aIndex, false);
            };
            CControl.prototype.getStartIndex = function () {
                return this._getDrawParam(KParamStrStartIndex);
            };
            // Utilities
            CControl.prototype.isFocusable = function () {
                return this._element.classList.contains(KClassFocusable);
            };
            // Scrolling scheme
            CControl.prototype.setScrollScheme = function (aScheme, aFixedScrollUnit) {
                this._setDrawParam(KParamStrScrollSchemeVertical, aScheme, false);
                this._setDrawParam(KParamStrScrollSchemeHorizontal, aScheme, false);
                if (aScheme == 5 /* EByFixed */) {
                    if (aFixedScrollUnit) {
                        this._setDrawParam(KParamStrScrollSchemeFixedUnitVertical, aFixedScrollUnit, false);
                        this._setDrawParam(KParamStrScrollSchemeFixedUnitHorizontal, aFixedScrollUnit, false);
                    }
                    else {
                        throw "fixed scroll unit is missiong";
                    }
                }
            };
            CControl.prototype.setVerticalScrollScheme = function (aScheme, aFixedScrollUnit) {
                this._setDrawParam(KParamStrScrollSchemeVertical, aScheme, false);
                if (aScheme == 5 /* EByFixed */) {
                    if (aFixedScrollUnit) {
                        this._setDrawParam(KParamStrScrollSchemeFixedUnitVertical, aFixedScrollUnit, false);
                    }
                    else {
                        throw "fixed scroll unit is missiong";
                    }
                }
            };
            CControl.prototype.getVerticalScrollScheme = function () {
                return this._getDrawParam(KParamStrScrollSchemeVertical) || 1 /* EByItem */;
            };
            CControl.prototype.setHorizontalScrollScheme = function (aScheme, aFixedScrollUnit) {
                this._setDrawParam(KParamStrScrollSchemeHorizontal, aScheme, false);
                if (aScheme == 5 /* EByFixed */) {
                    if (aFixedScrollUnit) {
                        this._setDrawParam(KParamStrScrollSchemeFixedUnitHorizontal, aFixedScrollUnit, false);
                    }
                    else {
                        throw "fixed scroll unit is missiong";
                    }
                }
            };
            CControl.prototype.getHorizontalScrollScheme = function () {
                return this._getDrawParam(KParamStrScrollSchemeHorizontal) || 1 /* EByItem */;
            };
            CControl.prototype.getSize = function (aPageSize) {
                return {
                    width: this._element.offsetWidth,
                    height: this._element.offsetHeight
                };
            };
            CControl.prototype.setParent = function (aParentControl) {
                this._parent = aParentControl;
            };
            CControl.KClassFocusable = "-f";
            CControl.KClassFocused = "-fd";
            return CControl;
        })(CSignalSource);
        CtrlBase.CControl = CControl;
        var CLayeredGroupControl = (function (_super) {
            __extends(CLayeredGroupControl, _super);
            function CLayeredGroupControl($self) {
                _super.call(this, $self);
                this._layerInfoStack = [];
                this.addClass("-layered");
            }
            CLayeredGroupControl.prototype.destroy = function () {
                var i, iLen, layerInfo, j, jLen, controls;
                for (i = 0, iLen = this._layerInfoStack.length; i < iLen; i++) {
                    layerInfo = this._layerInfoStack[i];
                    controls = layerInfo.childControls;
                    for (j = 0, jLen = controls.length; j < jLen; j++) {
                        controls[j].destroy();
                    }
                    Util.remove(layerInfo.elLayer);
                }
                this._layerInfoStack = null;
                _super.prototype.destroy.call(this);
            };
            CLayeredGroupControl.prototype.getLayoutElement = function (aClassName) {
                var ret;
                if (this._elLayer) {
                    var elements = this._elLayer.getElementsByClassName(aClassName);
                    if (elements.length == 1) {
                        ret = elements.item(0);
                    }
                    else if (elements.length == 0) {
                        ret = document.createElement("div");
                        ret.classList.add(aClassName);
                        this._elLayer.appendChild(ret);
                    }
                    else {
                        throw "Layout element duplicated";
                    }
                }
                return ret;
            };
            CLayeredGroupControl.prototype.getCurrentLayerElement = function () {
                return this._elLayer;
            };
            CLayeredGroupControl.prototype.createLayer = function (aParam) {
                var _this = this;
                if (aParam === void 0) { aParam = {}; }
                var elLayerPrev = this._elLayer;
                if (this._elLayer) {
                    this._layerInfoStack.push({
                        createParam: this._createParam,
                        elLayer: this._elLayer,
                        keyMap: this._keyMap,
                        keyMapBuilder: this._keyMapBuilder,
                        childControls: this._children
                    });
                    this._createParam = null;
                    this._elLayer = null;
                    this._keyMap = null;
                    this._children = [];
                }
                var elLayer = document.createElement("div");
                elLayer.classList.add("-layer");
                elLayer.style.position = "absolute";
                elLayer.style.width = "inherit";
                elLayer.style.height = "inherit";
                if (aParam.addClass) {
                    var classes = aParam.addClass.split(' ');
                    var i;
                    for (i = 0; i < classes.length; i++) {
                        elLayer.classList.add(classes[i]);
                    }
                }
                var executeNext = [];
                if (aParam.transition) {
                    this._setTransitioning(true);
                    if (aParam.transition.custom) {
                        executeNext.push(aParam.transition.custom.fnCreate(elLayerPrev, elLayer));
                    }
                    else {
                        if (elLayerPrev) {
                            switch (aParam.transition.prevLayer) {
                                case "fadeOut":
                                    elLayerPrev.style.transition = KCssTransitionParamOpa;
                                    executeNext.push(function () {
                                        elLayerPrev.style.opacity = "0";
                                    });
                                    break;
                                case "moveLeft":
                                    elLayerPrev.style.transition = KCssTransitionParamPos;
                                    executeNext.push(function () {
                                        elLayerPrev.style.left = -_this._element.offsetWidth + "px";
                                    });
                                    break;
                            }
                        }
                        switch (aParam.transition.newLayer) {
                            case "fadeIn":
                                elLayer.style.opacity = "0";
                                elLayer.style.transition = KCssTransitionParamOpa;
                                executeNext.push(function () {
                                    elLayer.style.opacity = "1";
                                });
                                break;
                            case "moveLeft":
                                elLayer.style.transition = KCssTransitionParamPos;
                                elLayer.style.left = this._element.offsetWidth + "px";
                                executeNext.push(function () {
                                    elLayer.style.left = "0px";
                                });
                                break;
                        }
                        if (aParam.transition.fnBeforeTransition) {
                            executeNext.push(function () {
                                aParam.transition.fnBeforeTransition(elLayerPrev, elLayer);
                            });
                        }
                    }
                    Util.afterTransition(elLayer, function () {
                        _this._setTransitioning(false);
                        if (aParam.transition.fnAfterTransition) {
                            aParam.transition.fnAfterTransition();
                        }
                    });
                }
                this._element.appendChild(elLayer);
                this._elLayer = elLayer;
                this._createParam = aParam;
                if (executeNext.length) {
                    setTimeout(function () {
                        var i, len;
                        for (i = 0, len = executeNext.length; i < len; i++) {
                            executeNext[i]();
                        }
                    }, 1);
                }
                return elLayer;
            };
            CLayeredGroupControl.prototype.removeLayer = function () {
                var _this = this;
                var prevChild = this._children;
                var prevElLayer = this._elLayer;
                var prevCreateParam = this._createParam;
                if (prevCreateParam.fnBeforeRemoval) {
                    prevCreateParam.fnBeforeRemoval();
                }
                var destroy = function () {
                    var i, len, control;
                    for (i = 0, len = prevChild.length; i < len; i++) {
                        control = prevChild[i];
                        control.destroy();
                    }
                    Util.remove(prevElLayer);
                    if (prevCreateParam.fnAfterRemoval) {
                        prevCreateParam.fnAfterRemoval();
                    }
                    prevChild = null;
                    prevElLayer = null;
                    prevCreateParam = null;
                };
                this._children = [];
                if (this._keyMap) {
                    this._keyMap.destroy();
                }
                this._keyMap = null;
                this._elLayer = null;
                var layerInfo = this._layerInfoStack.pop();
                var removed = false;
                if (layerInfo) {
                    this._children = layerInfo.childControls;
                    this._keyMap = layerInfo.keyMap;
                    this._keyMapBuilder = layerInfo.keyMapBuilder;
                    this._elLayer = layerInfo.elLayer;
                    this._elLayer.classList.remove("-stacked");
                    this._createParam = layerInfo.createParam;
                    var executeNext = [];
                    var transitionParam = prevCreateParam.transition;
                    if (transitionParam) {
                        if (transitionParam.custom) {
                            transitionParam.custom.fnRemove(prevElLayer, this._elLayer);
                        }
                        else {
                            switch (transitionParam.prevLayer) {
                                case "fadeOut":
                                    executeNext.push(function () {
                                        _this._elLayer.style.opacity = "1";
                                    });
                                    break;
                                case "moveLeft":
                                    this._elLayer.style.left = "0px";
                                    break;
                            }
                            switch (transitionParam.newLayer) {
                                case "fadeIn":
                                    prevElLayer.style.opacity = "0";
                                    break;
                                case "moveLeft":
                                    prevElLayer.style.left = this._element.offsetWidth + "px";
                                    break;
                            }
                        }
                        this._setTransitioning(true);
                        Util.afterTransition(prevElLayer, function () {
                            destroy();
                            var i, len;
                            for (i = 0, len = executeNext.length; i < len; i++) {
                                executeNext[i]();
                            }
                            _this._setTransitioning(false);
                        });
                    }
                    else {
                        destroy();
                    }
                    removed = true;
                }
                else {
                    destroy();
                }
                return removed;
            };
            CLayeredGroupControl.prototype.getLayerCount = function () {
                if (this._layerInfoStack) {
                    return this._layerInfoStack.length;
                }
                return 0;
            };
            CLayeredGroupControl.prototype.setAllLayersHidden = function () {
                var i, len, el;
                for (i = 0, len = this._layerInfoStack.length; i < len; i++) {
                    el = this._layerInfoStack[i].elLayer;
                    el.style.visibility = "hidden";
                }
                this._elLayer.style.visibility = "hidden";
            };
            CLayeredGroupControl.prototype.unsetAllLayersHidden = function () {
                var i, len, el;
                for (i = 0, len = this._layerInfoStack.length; i < len; i++) {
                    el = this._layerInfoStack[i].elLayer;
                    el.style.visibility = "";
                }
                this._elLayer.style.visibility = "";
            };
            return CLayeredGroupControl;
        })(CControl);
        CtrlBase.CLayeredGroupControl = CLayeredGroupControl;
        var CDynamicControl = (function (_super) {
            __extends(CDynamicControl, _super);
            function CDynamicControl(aProxy, aTarget) {
                _super.call(this, aTarget);
                this._proxy = aProxy;
                this._cssFiles = [];
                this._cssLinks = [];
                this._activated = false;
            }
            CDynamicControl.prototype.setPreloadJs = function (aJsFiles) {
                this._preloadJs = aJsFiles;
            };
            CDynamicControl.prototype.getPreloadJs = function () {
                return this._preloadJs;
            };
            CDynamicControl.prototype.preLoad = function (aCallback) {
                var _this = this;
                this._loadCssFiles(function () {
                    _this.doPreActivate();
                    aCallback();
                });
            };
            CDynamicControl.prototype.afterLoad = function (aParam) {
                this.doActivate(aParam);
                this.doPostActivate();
            };
            CDynamicControl.prototype.activate = function (param, aCallback) {
                var _this = this;
                console.log('APP', this.getName() + '.activate CSS count=' + this._cssFiles.length);
                this._loadCssFiles(function () {
                    console.log('PRF', 'DYCTRL.CSS loaded');
                    var ok = false;
                    try {
                        _this.doPreActivate();
                        _this.doActivate(param);
                        _this.doPostActivate();
                        ok = true;
                        _this._activated = true;
                    }
                    catch (e) {
                    }
                    console.log('APP', _this.getName() + '.activation: ' + (ok ? 'OK' : 'FAILED'));
                    if (aCallback) {
                        aCallback(ok);
                    }
                }, aCallback);
            };
            CDynamicControl.prototype.doPreActivate = function () {
            };
            CDynamicControl.prototype.doActivate = function (param) {
            };
            CDynamicControl.prototype.doPostActivate = function () {
            };
            CDynamicControl.prototype.deActivate = function (param) {
                this._activated = false;
                this.doPreDeactivate();
                try {
                    this.doDeActivate(param);
                }
                catch (e) {
                    console.log('APP', this.getName() + '.deActivate ======================= FAILED =======================with ' + e.message);
                }
                this._unloadCssFiles();
                this.doPostDeactivate();
                console.log('APP', this.getName() + '.deActivate');
            };
            CDynamicControl.prototype.doPreDeactivate = function () {
            };
            CDynamicControl.prototype.doDeActivate = function (param) {
                throw 'doDeactivate is not implemented!';
            };
            CDynamicControl.prototype.doPostDeactivate = function () {
            };
            CDynamicControl.prototype.isActivated = function () {
                return this._activated;
            };
            CDynamicControl.prototype.suspend = function () {
                console.log('APP', this.getName() + '.SUSPEND');
                this._unloadCssFiles();
                if (this.doSuspend) {
                    this.doSuspend();
                }
            };
            CDynamicControl.prototype.doSuspend = function () {
            };
            CDynamicControl.prototype.resume = function (callback) {
                var _this = this;
                console.log('APP', this.getName() + '.RESUME');
                this._loadCssFiles(function () {
                    callback();
                    if (_this.doResume) {
                        _this.doResume();
                    }
                });
            };
            CDynamicControl.prototype.doResume = function () {
            };
            CDynamicControl.prototype.getCssFiles = function () {
                return this._cssFiles;
            };
            CDynamicControl.prototype.addCssFile = function (cssFile) {
                this._cssFiles.push(cssFile);
            };
            CDynamicControl.prototype._loadCssFiles = function (fnOk, fnFail) {
                var _this = this;
                var i, cssFilesToLoad = [];
                for (i = 0; i < this._cssFiles.length; i += 1) {
                    cssFilesToLoad.push(this._cssFiles[i]);
                }
                var addRemoveLinkList = function (success, link) {
                    if (success) {
                        if (link) {
                            _this._cssLinks.push(link);
                        }
                        if (cssFilesToLoad.length) {
                            var cssFile = cssFilesToLoad.shift();
                            console.log('APP', _this.getName() + '.CSS: "' + cssFile + '"');
                            loadStyleSheet(cssFile, addRemoveLinkList);
                        }
                        else {
                            if (fnOk) {
                                fnOk();
                            }
                        }
                    }
                    else {
                        if (link) {
                            console.log('APP', _this.getName() + '.CSS: "' + link + '" LOADING FAILED');
                        }
                        fnFail(false);
                    }
                };
                addRemoveLinkList(true);
            };
            CDynamicControl.prototype._unloadCssFiles = function () {
                while (this._cssLinks.length) {
                    var cssLink = this._cssLinks.pop();
                    console.log('APP', this.getName() + '.UNLOAD CSS "' + cssLink.href + '"');
                    removeStyleSheet(cssLink);
                }
            };
            CDynamicControl.prototype._requestCustomReq = function (aReqName, aParam) {
                this._proxy._requestCustomReq(aReqName, aParam);
            };
            return CDynamicControl;
        })(CLayeredGroupControl);
        CtrlBase.CDynamicControl = CDynamicControl;
        var CDynamicControlProxy = (function (_super) {
            __extends(CDynamicControlProxy, _super);
            function CDynamicControlProxy(aID, $holder, aJsFileName) {
                _super.call(this);
                this._id = aID;
                this._$holder = $holder;
                this._targetJS = aJsFileName;
                this._dynamicControl = null;
                this._suspendedInfos = [];
                this._activating = false;
                this._resuming = false;
                this._activeDir = '';
                this.registerSignal(["PreActivated", "Activated", "DeActivated", "Suspended", "Resumed", "CustomEvent"]);
            }
            CDynamicControlProxy.prototype.destroy = function () {
                _super.prototype.destroy.call(this);
            };
            CDynamicControlProxy.prototype._activateCheck = function (aActiveDir, aActivateParam, aCallback) {
                if (this._activeDir == aActiveDir) {
                    aCallback(true, true);
                    return false;
                }
                if (this.findSuspendedControl(aActiveDir)) {
                    this._resumeControlByDir(aActiveDir);
                    aCallback(true, true);
                    return false;
                }
                if (!this._isPossibleToActivate(aActiveDir)) {
                    aCallback(false, false);
                    return false;
                }
                this._activating = true;
                this._activeDir = aActiveDir;
                this._activateParam = aActivateParam;
                return true;
            };
            CDynamicControlProxy.prototype.preActivate = function (aActiveDir, aActivateParam, aCallback) {
                var _this = this;
                if (!this._activateCheck(aActiveDir, aActivateParam, aCallback)) {
                    return;
                }
                require(["pages/" + this._activeDir + "/" + this._targetJS], function (aDynamicControlCreator) {
                    _this._createDynamicControl(aDynamicControlCreator, function () {
                        _this._dynamicControl.preLoad(function () {
                            _this._emitPreActivated(aActiveDir);
                            aCallback(true, false);
                        });
                    });
                });
            };
            CDynamicControlProxy.prototype.finalizeActivate = function () {
                var ret = false;
                try {
                    this._dynamicControl.afterLoad(this._activateParam);
                    ret = true;
                }
                catch (e) {
                }
                this._activating = false;
                this._emitActivated(this._activeDir);
                return ret;
            };
            CDynamicControlProxy.prototype.activate = function (aActiveDir, aActivateParam, aCallback) {
                var _this = this;
                if (!this._activateCheck(aActiveDir, aActivateParam, aCallback)) {
                    return;
                }
                require(["pages/" + this._activeDir + "/" + this._targetJS], function (aDynamicControlCreator) {
                    _this._createDynamicControl(aDynamicControlCreator, function () {
                        _this._dynamicControl.activate(aActivateParam, function (ok) {
                            _this._activating = false;
                            aCallback(true, false);
                            _this._emitActivated(aActiveDir);
                        });
                    });
                });
            };
            CDynamicControlProxy.prototype.deActivate = function (aParam) {
                if (this._activating) {
                    console.log('APP', this._id + '[' + this._activeDir + '].deActivate WARNNING ACTIVATING CONTROL');
                    return false;
                }
                else if (this._dynamicControl === null) {
                    console.log('APP', this._id + '[' + this._activeDir + '].deActivate HAS NO CONTROL');
                    return false;
                }
                else {
                    console.log('APP', this._id + '[' + this._activeDir + '].deActivate pushedLen=' + this._suspendedInfos.length);
                }
                this._$holder.removeClass(CDynamicControlProxy.KClassActive).empty();
                if (this._dynamicControl) {
                    this._dynamicControl.hide();
                    this._dynamicControl.setFocus(false);
                    this._dynamicControl.deActivate(aParam);
                    this._dynamicControl.destroy();
                    this._dynamicControl = null;
                }
                var deActivatedDir = this._activeDir;
                this._activeDir = '';
                this._emitDeActivated(deActivatedDir);
                return true;
            };
            CDynamicControlProxy.prototype.control = function () {
                return this._dynamicControl;
            };
            CDynamicControlProxy.prototype.isLoading = function () {
                return this._activating;
            };
            CDynamicControlProxy.prototype.isActive = function () {
                return this._dynamicControl !== null;
            };
            CDynamicControlProxy.prototype.isResuming = function () {
                return this._resuming;
            };
            CDynamicControlProxy.prototype.getId = function () {
                return this._activeDir;
            };
            CDynamicControlProxy.prototype.getActiveDir = function () {
                if (this.isActive()) {
                    return this._activeDir;
                }
                return null;
            };
            CDynamicControlProxy.prototype.findSuspendedControl = function (activeDir) {
                var i;
                for (i = 0; i < this._suspendedInfos.length; i += 1) {
                    var info = this._suspendedInfos[i];
                    if (info.activeDir == activeDir) {
                        return info;
                    }
                }
                return null;
            };
            CDynamicControlProxy.prototype.suspendControl = function (aReason) {
                var stackLevel = this._suspendedInfos.length;
                console.log('APP', this._id + '[' + this._activeDir + '].suspendControl level=' + stackLevel + ' reason=' + aReason);
                if (this._activeDir && this._activeDir.length) {
                    var $children = this._$holder.children().detach();
                    this._suspendedInfos.push({
                        $el: $children,
                        control: this._dynamicControl,
                        activeDir: this._activeDir,
                        reason: aReason
                    });
                    var suspendedId = this._activeDir;
                    this._activeDir = '';
                    this._dynamicControl.suspend();
                    this._$holder.removeClass(CDynamicControlProxy.KClassActive);
                    this._dynamicControl = null;
                    this._emitSuspended(aReason, suspendedId);
                    return true;
                }
                return false;
            };
            CDynamicControlProxy.prototype.resumeControl = function (aReason) {
                var _this = this;
                var i;
                if (this._activating || this._dynamicControl) {
                    console.log('APP', this._id + '[' + this._activeDir + '].resumeControl NOT POSSIBLE');
                    return false;
                }
                console.log('APP', this._id + '[' + this._activeDir + '].resumeControl level=' + this._suspendedInfos.length);
                var item = null;
                for (i = 0; i < this._suspendedInfos.length; i += 1) {
                    var info = this._suspendedInfos[i];
                    if (info.reason == aReason) {
                        item = info;
                        break;
                    }
                }
                if (item) {
                    this._suspendedInfos.splice(i, 1);
                }
                if (item) {
                    this._activeDir = item.activeDir;
                    this._dynamicControl = item.control;
                    this._resuming = true;
                    this._dynamicControl.resume(function () {
                        _this._resuming = false;
                        _this._$holder.append(item.$el);
                        _this._$holder.addClass(CDynamicControlProxy.KClassActive);
                        _this._emitResumed(aReason, _this._activeDir);
                    });
                    return true;
                }
                return false;
            };
            CDynamicControlProxy.prototype._createDynamicControl = function (aCreator, aCallback) {
                this._$holder.addClass(CDynamicControlProxy.KClassActive);
                this._dynamicControl = new aCreator(this);
                var preloadJs = this._dynamicControl.getPreloadJs();
                if (preloadJs) {
                    require(preloadJs, aCallback);
                }
                else {
                    aCallback();
                }
            };
            CDynamicControlProxy.prototype._resumeControlByDir = function (aActiveDir) {
                var _this = this;
                var i;
                if (this._activating || this._dynamicControl) {
                    console.log('APP', this._id + '[' + this._activeDir + '].resumeControl NOT POSSIBLE');
                    return null;
                }
                console.log('APP', this._id + '[' + this._activeDir + '].resumeControl level=' + this._suspendedInfos.length);
                var info;
                var item = null;
                for (i = 0; i < this._suspendedInfos.length; i += 1) {
                    var info = this._suspendedInfos[i];
                    if (info.activeDir == aActiveDir) {
                        item = info;
                        break;
                    }
                }
                if (item) {
                    this._suspendedInfos.splice(i, 1);
                }
                if (item) {
                    this._activeDir = item.activeDir;
                    this._dynamicControl = item.control;
                    this._resuming = true;
                    this._dynamicControl.resume(function () {
                        _this._resuming = false;
                        _this._$holder.append(item.$el);
                        _this._$holder.addClass(CDynamicControlProxy.KClassActive);
                        _this._emitResumed(item.reason, _this._activeDir);
                    });
                    return item.reason;
                }
                return null;
            };
            CDynamicControlProxy.prototype.clearSuspendedControl = function (aReason) {
                var item = null;
                var i, index = -1;
                for (i = 0; i < this._suspendedInfos.length; i += 1) {
                    var info = this._suspendedInfos[i];
                    if (info.reason == aReason) {
                        info.$el.remove();
                        info.$el = null;
                        info.control.setFocus(false);
                        info.control.deActivate();
                        info.control.destroy();
                        info.control = null;
                        index = i;
                        break;
                    }
                }
                if (index != -1) {
                    this._suspendedInfos.splice(index, 1);
                    console.log('APP', this._id + '[' + this._activeDir + '].clearSuspendedControl reason=' + aReason + ' len=' + this._suspendedInfos.length);
                }
            };
            CDynamicControlProxy.prototype.clearSuspendedPages = function () {
                console.log('APP', this._id + '[' + this._activeDir + '].clearSuspendedPages level=' + this._suspendedInfos.length);
                var i, item = null;
                for (i = 0; i < this._suspendedInfos.length; i += 1) {
                    var info = this._suspendedInfos[i];
                    info.$el.remove();
                    info.$el = null;
                    info.control.setFocus(false);
                    info.control.deActivate();
                    info.control = null;
                }
                this._suspendedInfos = [];
            };
            CDynamicControlProxy.prototype.hasControlStack = function () {
                return this._suspendedInfos.length > 0;
            };
            CDynamicControlProxy.prototype._isPossibleToActivate = function (aActiveDir) {
                if (this._activating) {
                    console.log('APP', this._id + '[' + aActiveDir + '] ALREADY ACTIVATING');
                    return false;
                }
                else if (this._dynamicControl) {
                    console.log('APP', this._id + '[' + aActiveDir + '] DEACTIVATE FIRST');
                    return false;
                }
                return true;
            };
            CDynamicControlProxy.prototype.connectPreActivated = function (aHolder, aSlotName, aSlot) {
                this.connect("PreActivated", aHolder, aSlotName);
            };
            CDynamicControlProxy.prototype._emitPreActivated = function (aId) {
                this.emit.call(this, "PreActivated", aId);
            };
            CDynamicControlProxy.prototype.connectActivated = function (aHolder, aSlotName, aSlot) {
                this.connect("Activated", aHolder, aSlotName);
            };
            CDynamicControlProxy.prototype._emitActivated = function (aId) {
                this.emit.call(this, "Activated", aId);
            };
            CDynamicControlProxy.prototype.connectDeActivated = function (aHolder, aSlotName, aSlot) {
                this.connect("DeActivated", aHolder, aSlotName);
            };
            CDynamicControlProxy.prototype._emitDeActivated = function (aId) {
                this.emit.call(this, "DeActivated", aId);
            };
            CDynamicControlProxy.prototype.connectSuspended = function (aHolder, aSlotName, aSlot) {
                this.connect("Suspended", aHolder, aSlotName);
            };
            CDynamicControlProxy.prototype._emitSuspended = function (aReason, aId) {
                this.emit.call(this, "Suspended", aReason, aId);
            };
            CDynamicControlProxy.prototype.connectResumed = function (aHolder, aSlotName, aSlot) {
                this.connect("Resumed", aHolder, aSlotName);
            };
            CDynamicControlProxy.prototype._emitResumed = function (aReason, aId) {
                this.emit.call(this, "Resumed", aReason, aId);
            };
            CDynamicControlProxy.prototype.connectCustomEvent = function (aHolder, aSlotName, aSlot) {
                this.connect("CustomEvent", aHolder, aSlotName);
            };
            CDynamicControlProxy.prototype._emitCustomEvent = function (aReqName, aParam) {
                this.emit.call(this, "CustomEvent", aReqName, aParam);
            };
            CDynamicControlProxy.prototype._requestCustomReq = function (aReqName, aParam) {
                this._emitCustomEvent(aReqName, aParam);
            };
            CDynamicControlProxy.KClassActive = "-dynamic-active";
            return CDynamicControlProxy;
        })(CSignalSource);
        CtrlBase.CDynamicControlProxy = CDynamicControlProxy;
        /*
         * bglee works
         */
        var KParamStrItemHeight = "itemHeight";
        var KParamStrItemWidth = "itemWidth";
        var KParamStrMaxRowCount = "maxRowCount";
        var KParamStrMaxColCount = "maxColCount";
        var KParamStrMargin = "margin";
        var KParamStrViewCount = "viewCount";
        var KParamStrAnchorIndex = "anchorIndex";
        var KParamStrAnchorHeight = "anchorHeight";
        var KParamStrAnchorWidth = "anchorWidth";
        var KParamStrStartIndex = "startIndex";
        var KParamStrMaxKeyQueueCount = "maxKeyQueueCount";
        var KParamStrTransparentAnchor = "transparentAnchor";
        var KParamStrDrawEffect = "drawEffect";
        var KParamStrOrientation = "orientation";
        var KParamAnimation = "animation";
        var KParamKeepFocus = "keepFocus";
        var KParamStrScrollSchemeVertical = "scrollingSchemeVertical";
        var KParamStrScrollSchemeHorizontal = "scrollingSchemeHorizontal";
        var KParamStrScrollSchemeFixedUnitVertical = "scrollingSchemeFixedUnitVertical";
        var KParamStrScrollSchemeFixedUnitHorizontal = "scrollingSchemeFixedUnitHorizontal";
        (function (TFocusInfo) {
            TFocusInfo[TFocusInfo["KFocusUnknown"] = 0] = "KFocusUnknown";
            TFocusInfo[TFocusInfo["KFocusNone"] = 1] = "KFocusNone";
            TFocusInfo[TFocusInfo["KFocusAble"] = 2] = "KFocusAble";
            TFocusInfo[TFocusInfo["KFocused"] = 3] = "KFocused";
        })(CtrlBase.TFocusInfo || (CtrlBase.TFocusInfo = {}));
        var TFocusInfo = CtrlBase.TFocusInfo;
        (function (TParamScrollScheme) {
            TParamScrollScheme[TParamScrollScheme["EUnknown"] = 0] = "EUnknown";
            TParamScrollScheme[TParamScrollScheme["EByItem"] = 1] = "EByItem";
            TParamScrollScheme[TParamScrollScheme["EByPage"] = 2] = "EByPage";
            TParamScrollScheme[TParamScrollScheme["EByFixedFocusRemains"] = 3] = "EByFixedFocusRemains";
            TParamScrollScheme[TParamScrollScheme["EByVariableFocusRemains"] = 4] = "EByVariableFocusRemains";
            TParamScrollScheme[TParamScrollScheme["EByFixed"] = 5] = "EByFixed";
        })(CtrlBase.TParamScrollScheme || (CtrlBase.TParamScrollScheme = {}));
        var TParamScrollScheme = CtrlBase.TParamScrollScheme;
        var CDrawnElements = (function () {
            function CDrawnElements() {
                this._drawnElements = {};
            }
            CDrawnElements.prototype.destroy = function (aExcept) {
                if (aExcept === void 0) { aExcept = []; }
                this.removeAll(aExcept);
            };
            CDrawnElements.prototype.removeAll = function (aExcept) {
                if (aExcept === void 0) { aExcept = []; }
                var key, el;
                for (key in this._drawnElements) {
                    if (this._drawnElements.hasOwnProperty(key)) {
                        el = this._drawnElements[key];
                        Util.remove(el);
                    }
                }
                this._drawnElements = {};
            };
            CDrawnElements.prototype.getCount = function () {
                return Object.keys(this._drawnElements).length;
            };
            CDrawnElements.prototype.getElements = function () {
                var ret = [], key;
                for (key in this._drawnElements) {
                    if (this._drawnElements.hasOwnProperty(key)) {
                        ret.push(this._drawnElements[key]);
                    }
                }
                return ret;
            };
            CDrawnElements.prototype.getElement = function (aKey) {
                return this._drawnElements[aKey];
            };
            CDrawnElements.prototype.setElement = function (aKey, aItem) {
                this._drawnElements[aKey] = aItem;
            };
            CDrawnElements.prototype.pickElement = function (aKey) {
                var ret = null;
                if (this._drawnElements.hasOwnProperty(aKey)) {
                    ret = this._drawnElements[aKey];
                    delete this._drawnElements[aKey];
                }
                return ret;
            };
            CDrawnElements.prototype.remove = function (aKey) {
                var el = this._drawnElements[aKey];
                if (el) {
                    Util.remove(el);
                }
                delete this._drawnElements[aKey];
            };
            CDrawnElements.prototype.forEach = function (fn) {
                var key;
                for (key in this._drawnElements) {
                    if (this._drawnElements.hasOwnProperty(key)) {
                        if (fn(key, this._drawnElements[key])) {
                            break;
                        }
                    }
                }
            };
            CDrawnElements.prototype.setAnimation = function (aEnable) {
                if (aEnable) {
                    this.forEach(function (aKey, aItem) {
                        aItem.style[KCssPropTransition] = KCssTransitionParamPos;
                        //FIXME: dummy return
                        return true;
                    });
                }
                else {
                    this.forEach(function (aKey, aItem) {
                        delete aItem.style[KCssPropTransition];
                        //FIXME: dummy return
                        return true;
                    });
                }
            };
            return CDrawnElements;
        })();
        CtrlBase.CDrawnElements = CDrawnElements;
        var CDataProvider = (function (_super) {
            __extends(CDataProvider, _super);
            function CDataProvider() {
                _super.call(this);
                this.registerSignal(['ItemInserted', 'ItemUpdated', 'ItemMoved', 'ItemRemoved', 'ItemRefreshed']);
            }
            CDataProvider.prototype.destroy = function () {
                _super.prototype.destroy.call(this);
            };
            CDataProvider.prototype.getItem = function (aKey) {
                throw "not implemented";
                return null;
            };
            CDataProvider.prototype.getLength = function () {
                throw "not implemented";
                return 0;
            };
            CDataProvider.prototype.getCheckedItems = function () {
                return this._doGetCheckedItems();
            };
            CDataProvider.prototype.deleteCheckedItem = function () {
                this._doDeleteCheckedItem();
            };
            CDataProvider.prototype.setItemCheck = function (aIndex) {
                this._doItemCheck(aIndex);
            };
            CDataProvider.prototype.setItemUnCheck = function (aIndex) {
                this._doItemUnCheck(aIndex);
            };
            CDataProvider.prototype.setItemCheckClear = function () {
                this._doItemCheckClear();
            };
            CDataProvider.prototype.isItemCheck = function (aIndex) {
                this._doIsItemCheck(aIndex);
            };
            CDataProvider.prototype.insertItems = function (aKey, aItems) {
                this._doInsertItems(aKey, aItems);
                this.emit("ItemInserted", aKey, aItems);
            };
            CDataProvider.prototype.removeItems = function (aKeys) {
                this._doRemoveItems(aKeys);
                this.emit("ItemRemoved", aKeys);
            };
            CDataProvider.prototype.updateItems = function (aKeys, aItems) {
                this._doUpdateItems(aKeys, aItems);
                this.emit("ItemUpdated", aKeys, aItems);
            };
            CDataProvider.prototype.connectItemInserted = function (aHolder, aSlotName, aSlot) {
                this.connect("ItemInserted", aHolder, aSlotName);
            };
            CDataProvider.prototype.connectItemRemoved = function (aHolder, aSlotName, aSlot) {
                this.connect("ItemRemoved", aHolder, aSlotName);
            };
            CDataProvider.prototype.connectItemUpdated = function (aHolder, aSlotName, aHandler) {
                this.connect("ItemUpdated", aHolder, aSlotName);
            };
            CDataProvider.prototype.sort = function (aSortEngine) {
                if (aSortEngine) {
                    if (typeof aSortEngine !== "function") {
                        throw "sort error argument type must function or undefined";
                    }
                }
                this._sort(aSortEngine);
            };
            CDataProvider.prototype._sort = function (aSortEngine) {
            };
            return CDataProvider;
        })(CSignalSource);
        CtrlBase.CDataProvider = CDataProvider;
        var CListDataProvider = (function (_super) {
            __extends(CListDataProvider, _super);
            function CListDataProvider(aArray, aRolling) {
                _super.call(this);
                for (var i = 0; i < aArray.length; i++) {
                    this._addCheckFn(aArray[i]);
                }
                this._bDataRolling = aRolling;
                this._listData = (this._bDataRolling) ? new CCircularArray(aArray) : aArray.slice(0);
                this.getItem = this._getItem;
                this.getLength = this._getLength;
                this._doInsertItems = this._insertItems;
                this._doRemoveItems = this._removeItems;
                this._doUpdateItems = this._updateItems;
                this._doItemCheck = this._itemCheck;
                this._doItemUnCheck = this._itemUnCheck;
                this._doItemCheckClear = this._itemCheckClear;
                this._doIsItemCheck = this._isItemCheck;
                this._doGetCheckedItems = this._getCheckedItems;
                this._doDeleteCheckedItem = this._deleteCheckedItem;
                this._itemsChecked = [];
            }
            CListDataProvider.prototype._isItemCheck = function (aIndex) {
                var item = this.getItem(aIndex);
                var ret = false;
                if (item) {
                    ret = !!item[CListDataProvider.KPropertyCheck];
                }
                return ret;
            };
            CListDataProvider.prototype._itemCheck = function (aIndex) {
                var item = this.getItem(aIndex);
                if (item) {
                    item[CListDataProvider.KPropertyCheck] = true;
                    this._itemsChecked.push(item);
                }
            };
            CListDataProvider.prototype._itemUnCheck = function (aIndex) {
                var item = this.getItem(aIndex);
                if (item) {
                    item[CListDataProvider.KPropertyCheck] = false;
                    this._itemsChecked.splice(this._itemsChecked.indexOf(item), 1);
                }
            };
            CListDataProvider.prototype._itemCheckClear = function () {
                var i, item, len = this.getLength();
                var fnClear = function (aItem) {
                    if (aItem.hasOwnProperty(CListDataProvider.KPropertyCheck)) {
                        aItem[CListDataProvider.KPropertyCheck] = false;
                    }
                };
                if (this._bDataRolling) {
                    for (i = 0; i < len; i++) {
                        item = this._listData.at(i);
                        fnClear(item);
                    }
                }
                else {
                    for (i = 0; i < len; i++) {
                        item = this._listData[i];
                        fnClear(item);
                    }
                }
                this._itemsChecked.length = 0;
            };
            CListDataProvider.prototype._getCheckedItems = function () {
                return this._itemsChecked;
            };
            CListDataProvider.prototype._deleteCheckedItem = function () {
                var i, list = this._listData, len = this.getLength();
                var aKeys = [];
                if (this._bDataRolling) {
                    for (i = 0; i < len; i++) {
                        if (list.at(i)[CListDataProvider.KPropertyCheck]) {
                            aKeys.push(i);
                        }
                    }
                }
                else {
                    for (i = 0; i < len; i++) {
                        if (list[i][CListDataProvider.KPropertyCheck]) {
                            aKeys.push(i);
                        }
                    }
                }
                this._itemsChecked.length = 0;
                this.removeItems(aKeys);
                /*
                if (this._bDataRolling) {
                    for (i = len - 1; i >= 0; i--) {
                        item = this._listData.at(i);
                        if (item[CListDataProvider.KPropertyCheck]) {
                            this._listData.del(i);
                        }
                    }
                } else {
                    for (i = len - 1; i >= 0; i--) {
                        item = this._listData[i];
                        if (item[CListDataProvider.KPropertyCheck]) {
                            this._listData.splice(i, 1);
                        }
                    }
                }
                */
            };
            CListDataProvider.prototype._getItem = function (aKey) {
                return (this._bDataRolling) ? this._listData.at(aKey) : this._listData[aKey];
            };
            CListDataProvider.prototype._getLength = function () {
                return (this._bDataRolling) ? this._listData.length() : this._listData.length;
            };
            CListDataProvider.prototype._addCheckFn = function (aItem) {
                aItem[CListDataProvider.KMethodIsCheck] = function () {
                    return !!this[CListDataProvider.KPropertyCheck];
                };
            };
            CListDataProvider.prototype._insertItems = function (aKey, aItems) {
                var list = this._listData;
                var i;
                for (i = 0; i < aItems.length; i++) {
                    this._addCheckFn(aItems[i]);
                }
                if (this._bDataRolling) {
                    list.appendArray(aItems);
                }
                else {
                    if (aItems.length) {
                        var front, end;
                        front = list.slice(0, aKey);
                        end = list.slice(aKey);
                        this._listData = front.concat(aItems, end);
                    }
                    else {
                        list.splice(aKey, 0, aItems);
                    }
                }
            };
            CListDataProvider.prototype._removeItems = function (aKey) {
                var i;
                aKey.sort(function (a, b) {
                    return a < b ? -1 : a > b ? 1 : 0;
                });
                if (this._bDataRolling) {
                    for (i = 0; i < aKey.length; i++) {
                        this._listData.del(aKey[i]);
                    }
                }
                else {
                    for (i = aKey.length - 1; i >= 0; i--) {
                        this._listData.splice(aKey[i], 1);
                    }
                }
            };
            CListDataProvider.prototype._updateItems = function (aKey, aItem) {
                var i, len;
                if (aItem) {
                    for (i = 0, len = aKey.length; i < len; i++) {
                        if (aItem[i]) {
                            this._listData[aKey[i]] = aItem[i];
                        }
                    }
                }
            };
            CListDataProvider.prototype.getIndex = function (aKey) {
                return (this._bDataRolling) ? this._listData.indexing(aKey) : aKey;
            };
            CListDataProvider.prototype._sort = function (aSortEngine) {
                var data = this._listData;
                var isCircular = this._listData instanceof CCircularArray;
                if (isCircular) {
                    data = this._listData.getArray();
                }
                var sortedData = aSortEngine(data);
                if (isCircular) {
                    if (sortedData instanceof CCircularArray) {
                        throw "sortEngine must return any[]";
                    }
                    this._listData = new CCircularArray(sortedData);
                }
                else {
                    this._listData = sortedData;
                }
            };
            CListDataProvider.KPropertyCheck = 'check_data_provider';
            CListDataProvider.KMethodIsCheck = 'isCheck';
            return CListDataProvider;
        })(CDataProvider);
        CtrlBase.CListDataProvider = CListDataProvider;
        var CDataControl = (function (_super) {
            __extends(CDataControl, _super);
            function CDataControl(aElement) {
                //TODO: about jQuery
                _super.call(this, $(aElement));
                this._redrawAfterOperation = false;
                this._drawnElements = new CDrawnElements();
                this.registerSignal(["DataItemSelected", "FocusedDataItemChanged"]);
                this.registerSignal(["ItemInserted", "ItemRemoved"]);
                this.registerSignal(["DataItemOrderChanged"]);
            }
            CDataControl.prototype.destroy = function () {
                if (this._drawnElements) {
                    this._drawnElements.destroy();
                    delete this._drawnElements;
                }
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.destroy();
                    delete this._ownedDataProvider;
                }
                if (this._drawer) {
                    delete this._drawer;
                }
            };
            CDataControl.prototype.draw = function (aRect) {
                if (!this._drawer) {
                    throw "data drawer must be provided to draw";
                }
                _super.prototype.draw.call(this, aRect);
            };
            CDataControl.prototype.setDataDrawer = function (aDrawer) {
                this._drawer = aDrawer;
            };
            CDataControl.prototype.getFocusedItemInfo = function () {
                var el = this.getFocusedElement();
                var key, item;
                if (el) {
                    key = el.attributes["data"];
                    item = this._ownedDataProvider.getItem(key);
                    return {
                        el: el,
                        key: key,
                        item: item
                    };
                }
                return null;
            };
            CDataControl.prototype.setFocusedItem = function (aKey) {
                var el = this._drawnElements.getElement(aKey);
                if (el) {
                    this.setFocusedElement(el);
                }
            };
            CDataControl.prototype.getNextFocusIndex = function (aIncrementRow, aFocusRemain) {
                throw "implement for scrolling!";
            };
            CDataControl.prototype.setDrawnHint = function (aHint) {
                this._drawerHint = aHint;
            };
            CDataControl.prototype.getDrawerHint = function (aIndex) {
                var ret = null;
                var i, len = this._drawerHint.length;
                for (i = 0; i < len; i++) {
                    if (this._drawerHint[i].index === aIndex) {
                        ret = this._drawerHint[i];
                        break;
                    }
                }
                return ret;
            };
            CDataControl.prototype.doItemChagned = function (aKeys) {
            };
            CDataControl.prototype.doItemInserted = function (aKey, aItems, aNeedFocus) {
            };
            CDataControl.prototype.doItemRemoved = function (aKey, aUnsetFocus) {
            };
            CDataControl.prototype.setRedrawAfterOperation = function (aRedraw) {
                this._redrawAfterOperation = aRedraw;
            };
            CDataControl.prototype.setOwnedDataProvider = function (dataProvider) {
                this._keyMapHint = null;
                if (this._drawnElements) {
                    this._drawnElements.removeAll();
                }
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.destroy();
                }
                this._ownedDataProvider = dataProvider;
            };
            CDataControl.prototype.getOwnedDataProvider = function () {
                return this._ownedDataProvider;
            };
            CDataControl.prototype.clearDrawnElements = function () {
                this._clearDrawnElements();
            };
            CDataControl.prototype._clearDrawnElements = function () {
                if (this._drawnElements) {
                    this._drawnElements.removeAll();
                }
            };
            CDataControl.prototype._setDrawnElement = function (aKey, el) {
                if (this._drawnElements) {
                    this._drawnElements.setElement(aKey, el);
                }
            };
            CDataControl.prototype._getDrawnElement = function (aKey) {
                if (this._drawnElements) {
                    return this._drawnElements.getElement(aKey);
                }
                return;
            };
            CDataControl.prototype._getDrawnElements = function () {
                if (this._drawnElements) {
                    return this._drawnElements.getElements();
                }
                return;
            };
            CDataControl.prototype._removeDrawnElement = function (aKey) {
                if (this._drawnElements) {
                    this._drawnElements.remove(aKey);
                }
                if (this._prevDrawnElements) {
                    this._prevDrawnElements.remove(aKey);
                }
            };
            CDataControl.prototype._pickDrawnElements = function () {
                var ret = this._drawnElements;
                this._drawnElements = new CDrawnElements();
                return ret;
            };
            CDataControl.prototype._setPrevDrawnElements = function (aDrawnElements) {
                if (this._prevDrawnElements) {
                    this._prevDrawnElements.destroy();
                }
                aDrawnElements.forEach(function (aKey, aEl) {
                    aEl.classList.remove(CtrlBase.TClassStr.KClassActiveFocusedLeaf);
                    aEl.classList.remove(CtrlBase.TClassStr.KClassFocused);
                    //FIXME: dummy return
                    return true;
                });
                this._prevDrawnElements = aDrawnElements;
            };
            CDataControl.prototype._handleFocusChanged = function (aElOld, aElNew) {
                //FIXME
                _super.prototype._handleFocusChanged.call(this, aElOld, aElNew);
                var key = aElNew.attributes["data"];
                var item = this._ownedDataProvider.getItem(key);
                this._emitFocusedDataItemChanged(key, item, aElNew);
            };
            /*
             Signals
             */
            CDataControl.prototype._doKeyEnterLatent = function () {
                //FIXME
                //            super._doKeyEnterLatent();
                var focusedInfo = this.getFocusedItemInfo();
                return this._emitDataItemSelected(focusedInfo.key, focusedInfo.item, focusedInfo.el);
            };
            CDataControl.prototype.connectDataItemSelected = function (aHolder, aSlotName, aSlot) {
                this.connect("DataItemSelected", aHolder, aSlotName);
            };
            CDataControl.prototype._emitDataItemSelected = function (aKey, aItem, aEl) {
                return this.emit.call(this, "DataItemSelected", aKey, aItem, aEl);
            };
            CDataControl.prototype.connectFocusedDataItemChanged = function (aHolder, aSlotName, aSlot) {
                this.connect("FocusedDataItemChanged", aHolder, aSlotName);
            };
            CDataControl.prototype._emitFocusedDataItemChanged = function (aKey, aItem, aEl) {
                this.emit.call(this, "FocusedDataItemChanged", aKey, aItem, aEl);
            };
            CDataControl.prototype.connectDataItemOrderChanged = function (aHolder, aSlotName, aSlot) {
                this.connect("DataItemOrderChanged", aHolder, aSlotName);
            };
            CDataControl.prototype._emitDataItemOrderChanged = function (aItemKey, aItemMovedPosition, aDataProvider) {
                this.emit.call(this, "DataItemOrderChanged", aItemKey, aItemMovedPosition, aDataProvider);
            };
            CDataControl.prototype.getContentAvail = function () {
                return this._contentAvail;
            };
            CDataControl.prototype.setContentAvail = function (aContentAvail) {
                this._contentAvail = aContentAvail;
            };
            CDataControl.prototype.getCustomScrollSize = function (aKeyStr, aDrawnRect) {
                // todo child control impliment
                return null;
            };
            CDataControl.prototype.getCustomScrollSizeByIndex = function (aIndex) {
                // todo child control impliment
                return null;
            };
            CDataControl.prototype.setDataRolling = function (aRolling) {
                this._bDataRolling = aRolling;
            };
            CDataControl.prototype.getDataRolling = function () {
                return this._bDataRolling;
            };
            CDataControl.prototype._doUpdate = function (aKeepFocus) {
                if (aKeepFocus === void 0) { aKeepFocus = false; }
                _super.prototype._doUpdate.call(this, aKeepFocus);
                if (aKeepFocus) {
                    this._pickDrawnElements().destroy();
                }
            };
            CDataControl.prototype.prependItems = function (aItems) {
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.insertItems(0, aItems);
                }
            };
            CDataControl.prototype.insertItems = function (position, aItems) {
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.insertItems(position, aItems);
                }
            };
            CDataControl.prototype.appendItems = function (aItems) {
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.insertItems(this._ownedDataProvider.getLength(), aItems);
                }
            };
            CDataControl.prototype.updateItems = function (aKeys, aItems) {
                this.setKeepFocus(true);
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.updateItems(aKeys, aItems);
                }
            };
            CDataControl.prototype.removeItems = function (index) {
                this.setKeepFocus(true);
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.removeItems(index);
                }
            };
            CDataControl.prototype.getItemLength = function () {
                if (this._ownedDataProvider) {
                    return this._ownedDataProvider.getLength();
                }
                return;
            };
            CDataControl.prototype.doSelect = function (focusedChild) {
                var itemInfo = this.getFocusedItemInfo();
                if (itemInfo) {
                    return this._emitDataItemSelected(itemInfo.key, itemInfo.item, itemInfo.el);
                }
                else {
                    return false;
                }
            };
            CDataControl.prototype.getCheckedItems = function () {
                if (this._ownedDataProvider) {
                    return this._ownedDataProvider.getCheckedItems();
                }
                return;
            };
            CDataControl.prototype.deleteCheckedItem = function () {
                this.setKeepFocus(true);
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.deleteCheckedItem();
                }
            };
            CDataControl.prototype.setItemCheck = function (aIndex) {
                this.setKeepFocus(true);
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.setItemCheck(aIndex);
                }
            };
            CDataControl.prototype.setItemUnCheck = function (aIndex) {
                this.setKeepFocus(true);
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.setItemUnCheck(aIndex);
                }
            };
            CDataControl.prototype.setItemCheckClear = function () {
                this.setKeepFocus(true);
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.setItemCheckClear();
                }
            };
            CDataControl.prototype.isItemCheck = function (aIndex) {
                if (this._ownedDataProvider) {
                    this._ownedDataProvider.isItemCheck(aIndex);
                }
            };
            return CDataControl;
        })(CControl);
        CtrlBase.CDataControl = CDataControl;
        var CListDataControl = (function (_super) {
            __extends(CListDataControl, _super);
            function CListDataControl(aElement, aRolling) {
                _super.call(this, aElement);
                this._stackMove = null;
                this._bDataRolling = aRolling;
                this._element.classList.add("-list");
            }
            CListDataControl.prototype.endTransition = function () {
                this._fnEndTransition();
            };
            CListDataControl.prototype.getNextFocusIndex = function (aIncrementRow, aFocusRemain) {
                if (aFocusRemain) {
                    return aIncrementRow > 0 ? this.getFocusedElementIndex() + aIncrementRow : this.getFocusedElementIndex() + aIncrementRow - 2;
                }
                else {
                    return aIncrementRow > 0 ? this.getFocusedElementIndex() + aIncrementRow - 1 : this.getFocusedElementIndex() + aIncrementRow + 1;
                }
            };
            CListDataControl.prototype.doItemInserted = function (aKey, aItems, aNeedFocus) {
                this.emit.call(this, "ItemInserted", this._drawnElements, aNeedFocus);
            };
            CListDataControl.prototype.doItemRemoved = function (aKey, aUnsetFocus) {
                this.emit.call(this, "ItemRemoved", this._drawnElements, aUnsetFocus);
            };
            CListDataControl.prototype.getSize = function (aPageSize) {
                var itemHeight = this._getDrawParam(KParamStrItemHeight) || 0;
                var itemWidth = this._getDrawParam(KParamStrItemWidth) || 0;
                var horizontal = (this._getDrawParam(KParamStrOrientation) === 2 /* EHorizontal */);
                var count = this._ownedDataProvider ? this._ownedDataProvider.getLength() : 0;
                var w;
                var h;
                w = horizontal ? itemWidth * count : itemWidth;
                h = horizontal ? itemHeight : itemHeight * count;
                if (aPageSize && this._bDataRolling) {
                    var countPage;
                    var page = 0;
                    if (!horizontal) {
                        var containerHeight = aPageSize.height;
                        countPage = containerHeight / itemHeight;
                        if (count <= countPage) {
                            page = 1;
                        }
                        else {
                            page = Math.floor(count / (countPage - 1));
                            page += count % (countPage - 1) ? 1 : 0;
                        }
                        if (page > 0) {
                            h = containerHeight;
                            page -= 1;
                            if (page > 0) {
                                h += (containerHeight - itemHeight) * page;
                            }
                        }
                    }
                    else {
                        //TODO: is horizontal page scroll?
                        var containerWidth = aPageSize.width;
                    }
                }
                return {
                    width: w,
                    height: h
                };
            };
            CListDataControl.prototype._doDraw = function (aRect, aDrawParam) {
                var focusableElements = [];
                var horizontal = (aDrawParam[KParamStrOrientation] === 2 /* EHorizontal */);
                var dp = this._ownedDataProvider;
                var fixedItemSize = horizontal ? aDrawParam[KParamStrItemWidth] : aDrawParam[KParamStrItemHeight];
                var drawPosStart = horizontal ? aRect.left : aRect.top;
                var drawPosEnd = horizontal ? aRect.right : aRect.bottom;
                var drawSize = horizontal ? aRect.getHeight() : aRect.getWidth();
                var posProp = horizontal ? "left" : "top";
                var posPropPair = horizontal ? "right" : "bottom";
                var startIndex = drawSize ? Math.floor(drawPosStart / fixedItemSize) : 0;
                var itemCount = this._bDataRolling ? startIndex + dp.getLength() : dp.getLength();
                //var itemCount: number = dp.getLength();
                var i;
                var prevDrawnItem = this._pickDrawnElements();
                var contentAvailable = {
                    up: 0,
                    left: 0,
                    right: 0,
                    down: 0
                };
                var countItemOnPage = aRect.getHeight() / fixedItemSize;
                var fixedScrollSize = fixedItemSize * (countItemOnPage - 1);
                this._keyMapBuilder = horizontal ? CtrlBase.KBuilderLeftRight : CtrlBase.KBuilderTopDown;
                var compensation = 0;
                if (!horizontal && aDrawParam[KParamStrScrollSchemeVertical] === 3 /* EByFixedFocusRemains */) {
                    if (this._bDataRolling) {
                        itemCount = dp.getLength();
                        if (aRect.bottom < 0) {
                            startIndex -= startIndex;
                        }
                    }
                    else if (aRect.top < 0) {
                        if (!this._bDataRolling) {
                            drawPosStart -= drawPosEnd;
                            drawPosEnd = 0;
                            if (startIndex < 0 && Math.abs(startIndex)) {
                                compensation = dp.getLength() % (countItemOnPage - 1) - 1;
                                startIndex += compensation;
                                this.setKeepFocus(false);
                            }
                        }
                        else {
                            startIndex -= startIndex;
                        }
                    }
                }
                for (i = startIndex; i < itemCount; i++) {
                    var elPosStart = i * fixedItemSize;
                    var elPosEnd = elPosStart + fixedItemSize;
                    var index = dp.getIndex(i);
                    if (elPosStart < drawPosEnd) {
                        // draw element if necessary
                        var el = prevDrawnItem.pickElement(index);
                        var item = dp.getItem(index);
                        if (!el) {
                            el = document.createElement("div");
                            el.classList.add("-list-item");
                            el.attributes["data"] = index;
                            el.style.position = "absolute";
                            el.style[posProp] = (i - compensation) * fixedItemSize + "px";
                            el.style[posPropPair] = parseInt(el.style[posProp]) + fixedItemSize + "px";
                            var focusInfo = this._drawer(index, item, el);
                            if (focusInfo === 2 /* KFocusAble */) {
                                el.classList.add(CtrlBase.TClassStr.KClassFocusable);
                            }
                            if (focusInfo !== 0 /* KFocusUnknown */) {
                                this._element.appendChild(el);
                            }
                        }
                        this._setDrawnElement(index, el);
                        if (drawPosStart <= elPosStart && elPosEnd <= drawPosEnd && el.classList.contains(CtrlBase.TClassStr.KClassFocusable)) {
                            focusableElements.push(el);
                        }
                    }
                }
                if (!this.getAnimation()) {
                    prevDrawnItem.destroy();
                }
                else {
                    prevDrawnItem.forEach(function (aKey, aItem) {
                        var ret = false;
                        if (aItem.classList.contains(CtrlBase.TClassStr.KClassActiveFocusedLeaf)) {
                            aItem.classList.remove(CtrlBase.TClassStr.KClassActiveFocusedLeaf);
                            ret = true;
                        }
                        return ret;
                    });
                    this._fnEndTransition = function () {
                        prevDrawnItem.destroy();
                    };
                }
                // check content available
                //            if (horizontal) {
                //                if (contentAvailable.left === 0 && startIndex) {
                //                    contentAvailable.left = fixedItemSize;
                //                }
                //                if (contentAvailable.right === 0 && i !== itemCount) {
                //                    contentAvailable.right = fixedItemSize;
                //                }
                //            } else {
                //                if (contentAvailable.up === 0 && startIndex) {
                //                    if (aRect.top % fixedScrollSize) {
                //                        contentAvailable.up = fixedItemSize;
                //                    } else {
                //                        contentAvailable.up = fixedScrollSize;
                //                    }
                //                }
                //                if (contentAvailable.down === 0 && i !== itemCount) {
                //                    contentAvailable.down = fixedItemSize;
                //                }
                //            }
                if (itemCount) {
                    var size = this.getSize();
                    var first = focusableElements[0];
                    var last = focusableElements[focusableElements.length - 1];
                    if (first) {
                        if (horizontal) {
                            var width = first.offsetWidth;
                            var left = first.offsetLeft;
                            var right = last.offsetLeft + width;
                            contentAvailable.left = left;
                            contentAvailable.right = size.width - right;
                            if (contentAvailable.right < 0) {
                                contentAvailable.right = 0;
                            }
                        }
                        else {
                            var top = first.offsetTop;
                            var down = last.offsetTop + last.offsetHeight;
                            contentAvailable.up = top;
                            contentAvailable.down = size.height - down;
                        }
                    }
                }
                //            this._contentAvail = contentAvailable;
                this.setContentAvail(contentAvailable);
                return focusableElements;
            };
            CListDataControl.prototype.setAnimation = function (aAnimation) {
                _super.prototype.setAnimation.call(this, aAnimation);
            };
            CListDataControl.prototype.setModeMove = function (aSet) {
                var keyMap = this._keyMap;
                var isDirty = keyMap.setMode(aSet ? 1 /* MOVE */ : 0 /* NORMAL */);
                var dp = this.getOwnedDataProvider();
                var len = dp.getLength();
                if (aSet) {
                    if (!this._stackMove) {
                        this._stackMove = {};
                        for (var i = 0; i < len; i++) {
                            this._stackMove[i] = i;
                        }
                    }
                }
                else if (isDirty) {
                    var focusedInfo = this.getFocusedItemInfo();
                    var stack = this._stackMove;
                    var key = focusedInfo.key;
                    var posMoved = this.getIndexByElement(this.getFocusedElement());
                    if (focusedInfo !== null) {
                        var updatedKey = stack[key];
                        if (updatedKey === posMoved) {
                            return;
                        }
                        this._emitDataItemOrderChanged(updatedKey, posMoved, this.getOwnedDataProvider());
                    }
                    var isMovingRight = posMoved > updatedKey;
                    if (isMovingRight) {
                        for (var i = 0; i < len; i++) {
                            if (stack[i] <= posMoved) {
                                stack[i]--;
                            }
                        }
                    }
                    else {
                        for (var i = 0; i < len; i++) {
                            if (stack[i] < updatedKey) {
                                stack[i]++;
                            }
                        }
                    }
                    stack[key] = posMoved;
                }
            };
            return CListDataControl;
        })(CDataControl);
        CtrlBase.CListDataControl = CListDataControl;
        var CGridDataControl = (function (_super) {
            __extends(CGridDataControl, _super);
            function CGridDataControl(aElement) {
                _super.call(this, aElement);
                this._element.classList.add("-grid");
            }
            CGridDataControl.prototype._doGetKeyMapHint = function () {
                var rowCount = this.getMaxRowCount();
                if (rowCount > 0) {
                    // horizontal
                    return rowCount;
                }
                return this._keyMapHint;
            };
            CGridDataControl.prototype.getRowElement = function (aRow, bDuplicate) {
                var ret;
                var len = this._focusableElement.length;
                var startData = parseInt(this._focusableElement[0].getAttribute('data'));
                var endData = parseInt(this._focusableElement[len - 1].getAttribute('data'));
                var i;
                if (this._drawerHint) {
                    var startPosition = this.getPositionFromHintArray(startData);
                    var endPosition = this.getPositionFromHintArray(endData);
                    if (aRow >= startPosition.row && aRow <= endPosition.row) {
                        var hintArray = this._getHintArray();
                        var rowArray = hintArray[aRow];
                        var rowLen = rowArray.length;
                        var j;
                        for (i = 0; i < rowLen; i++) {
                            for (j = 0; j < len; j++) {
                                var data = parseInt(this._focusableElement[j].getAttribute('data'));
                                if (data === rowArray[i]) {
                                    if (ret) {
                                        if (bDuplicate || ret.indexOf(this._focusableElement[j]) === -1) {
                                            ret.push(this._focusableElement[j]);
                                        }
                                    }
                                    else {
                                        ret = [this._focusableElement[j]];
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
                else {
                    var maxCol = this._getDrawParam(KParamStrMaxColCount);
                    var startRow = Math.floor(startData / maxCol);
                    var endRow = Math.floor(endData / maxCol);
                    if (aRow >= startRow && aRow <= endRow) {
                        for (i = 0; i < len; i++) {
                            var data = parseInt(this._focusableElement[i].getAttribute('data'));
                            var row = Math.floor(data / maxCol);
                            if (aRow === row) {
                                if (ret) {
                                    if (ret.indexOf(this._focusableElement[i]) === -1) {
                                        ret.push(this._focusableElement[i]);
                                    }
                                }
                                else {
                                    ret = [this._focusableElement[i]];
                                }
                            }
                        }
                    }
                }
                return ret;
            };
            CGridDataControl.prototype.getColElement = function (aCol, bDuplicate) {
                var ret;
                var len = this._focusableElement.length;
                var startData = parseInt(this._focusableElement[0].getAttribute('data'));
                var endData = parseInt(this._focusableElement[len - 1].getAttribute('data'));
                var maxCol = this._getDrawParam(KParamStrMaxColCount);
                var i;
                if (aCol <= maxCol) {
                    if (this._drawerHint) {
                        var startPosition = this.getPositionFromHintArray(startData);
                        var endPosition = this.getPositionFromHintArray(endData);
                        var hintArray = this._getHintArray();
                        var rowLen = hintArray.length;
                        var j;
                        for (i = 0; i < rowLen; i++) {
                            if (i >= startPosition.row && i <= endPosition.row) {
                                if (hintArray[i][aCol] !== undefined && hintArray[i][aCol] !== null) {
                                    for (j = 0; j < len; j++) {
                                        var data = parseInt(this._focusableElement[j].getAttribute('data'));
                                        if (hintArray[i][aCol] === data) {
                                            if (ret) {
                                                if (bDuplicate || ret.indexOf(this._focusableElement[j]) === -1) {
                                                    ret.push(this._focusableElement[j]);
                                                }
                                            }
                                            else {
                                                ret = [this._focusableElement[j]];
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        var maxCol = this._getDrawParam(KParamStrMaxColCount);
                        var startRow = Math.floor(startData / maxCol);
                        var endRow = Math.floor(endData / maxCol);
                        for (i = 0; i < len; i++) {
                            var data = parseInt(this._focusableElement[i].getAttribute('data'));
                            var col = data % maxCol;
                            if (col === aCol) {
                                if (ret) {
                                    if (ret.indexOf(this._focusableElement[i]) === -1) {
                                        ret.push(this._focusableElement[i]);
                                    }
                                }
                                else {
                                    ret = [this._focusableElement[i]];
                                }
                            }
                        }
                    }
                }
                return ret;
            };
            CGridDataControl.prototype.getNextFocusIndex = function (aIncrementRow, aFocusRemain) {
                var focusableElementCount = this.getFocusableElementCount();
                var currentIndex = this.getFocusedElementIndex();
                var maxColCount = this._getDrawParam(KParamStrMaxColCount) || 1;
                var currentRow, currentCol, nextRow;
                var arrayBlock = this._getHintArray();
                var nextIndex;
                if (this._nextFocusItem) {
                    nextIndex = parseInt(this._nextFocusItem.getAttribute('data'));
                    this._nextFocusItem = null;
                }
                else {
                    if (this._drawerHint) {
                        var position = this.getPositionFromHintArray(currentIndex);
                        if (position) {
                            currentCol = position.col;
                            currentRow = position.row;
                            if (aFocusRemain) {
                                nextRow = aIncrementRow > 0 ? currentRow - aIncrementRow + 1 : currentRow - aIncrementRow - 1;
                            }
                            else {
                                nextRow = aIncrementRow > 0 ? currentRow - aIncrementRow : currentRow - aIncrementRow;
                            }
                            if (nextRow < 0) {
                                nextRow = 0;
                            }
                            else if (nextRow >= arrayBlock.length) {
                                nextRow = arrayBlock.length - 1;
                            }
                            nextIndex = arrayBlock[nextRow][currentCol];
                            if (nextIndex === undefined) {
                                nextIndex = this.getItemLength() - 1;
                            }
                            else if (nextIndex === currentIndex) {
                                currentRow = nextRow;
                                if (aFocusRemain) {
                                    nextRow = aIncrementRow > 0 ? currentRow - aIncrementRow + 1 : currentRow - aIncrementRow - 1;
                                }
                                else {
                                    nextRow = aIncrementRow > 0 ? currentRow - aIncrementRow : currentRow - aIncrementRow;
                                }
                                if (nextRow < 0) {
                                    nextRow = 0;
                                }
                                else if (nextRow >= arrayBlock.length) {
                                    nextRow = arrayBlock.length - 1;
                                }
                                nextIndex = arrayBlock[nextRow][currentCol];
                            }
                        }
                    }
                    else {
                        currentRow = Math.ceil((currentIndex + 1) / maxColCount);
                        currentCol = currentIndex % maxColCount;
                        if (aFocusRemain) {
                            nextRow = currentRow + aIncrementRow;
                        }
                        else {
                            nextRow = aIncrementRow > 0 ? currentRow + aIncrementRow - 1 : currentRow + aIncrementRow + 1;
                        }
                        var itemLength = this.getItemLength();
                        var maxRow = Math.ceil(itemLength / maxColCount);
                        if (nextRow < 0) {
                            nextRow = 0;
                        }
                        else if (nextRow > maxRow) {
                            nextRow = maxRow;
                        }
                        nextIndex = (nextRow - 1) * maxColCount + currentCol;
                    }
                }
                return nextIndex;
            };
            CGridDataControl.prototype.getCustomScrollSize = function (aKeyStr, aDrawnRect) {
                var focusedItem = this.getFocusedElement();
                var position = this.getPositionFromHintArray(parseInt(focusedItem.getAttribute('data')));
                var ret = false;
                var nextItemContentAvail = {
                    up: 0,
                    left: 0,
                    right: 0,
                    down: 0
                };
                if (position) {
                    switch (aKeyStr) {
                        case KKeyStrUp:
                            if (position.row > 0) {
                                var nextItemIndex = this._keyMapHint[position.row - 1][position.col];
                                var hint = this.getDrawerHint(nextItemIndex);
                                if (hint) {
                                    this._nextFocusItem = this._getDrawnElement(nextItemIndex);
                                    if (this._nextFocusItem) {
                                        nextItemContentAvail.up = aDrawnRect.top - this._nextFocusItem.offsetTop;
                                    }
                                    else {
                                        nextItemContentAvail.up = hint.span.row * this.getItemHeight();
                                    }
                                    ret = true;
                                }
                            }
                            break;
                        case KKeyStrDown:
                            if (position.row < this._keyMapHint.length - 1) {
                                var nextItemIndex = this._keyMapHint[position.row + 1][position.col];
                                var hint = this.getDrawerHint(nextItemIndex);
                                if (hint) {
                                    this._nextFocusItem = this._getDrawnElement(nextItemIndex);
                                    if (this._nextFocusItem) {
                                        nextItemContentAvail.down = this._nextFocusItem.offsetTop + this._nextFocusItem.offsetHeight - aDrawnRect.bottom;
                                    }
                                    else {
                                        nextItemContentAvail.down = hint.span.row * this.getItemHeight();
                                    }
                                    ret = true;
                                }
                            }
                            break;
                        case KKeyStrLeft:
                            if (position.col > 0) {
                                this._nextFocusItem = this._getDrawnElement(this._keyMapHint[position.row][position.col - 1]);
                                if (this._nextFocusItem) {
                                    if (this._nextFocusItem.offsetTop < focusedItem.offsetTop) {
                                        nextItemContentAvail.up = focusedItem.offsetTop - this._nextFocusItem.offsetTop;
                                    }
                                    else if (this._nextFocusItem.offsetTop + this._nextFocusItem.offsetHeight > focusedItem.offsetTop + focusedItem.offsetHeight) {
                                        nextItemContentAvail.down = this._nextFocusItem.offsetTop + this._nextFocusItem.offsetHeight - focusedItem.offsetTop - focusedItem.offsetHeight;
                                    }
                                }
                                ret = true;
                            }
                            break;
                        case KKeyStrRight:
                            if (position.col < this._keyMapHint[position.row].length - 1) {
                                this._nextFocusItem = this._getDrawnElement(this._keyMapHint[position.row][position.col + 1]);
                                if (this._nextFocusItem) {
                                    if (this._nextFocusItem.offsetTop < focusedItem.offsetTop) {
                                        nextItemContentAvail.up = focusedItem.offsetTop - this._nextFocusItem.offsetTop;
                                    }
                                    else if (this._nextFocusItem.offsetTop + this._nextFocusItem.offsetHeight > focusedItem.offsetTop + focusedItem.offsetHeight) {
                                        nextItemContentAvail.down = this._nextFocusItem.offsetTop + this._nextFocusItem.offsetHeight - focusedItem.offsetTop - focusedItem.offsetHeight;
                                    }
                                    ret = true;
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
                if (ret) {
                    return nextItemContentAvail;
                }
                else {
                    return null;
                }
            };
            CGridDataControl.prototype.getCustomScrollSizeByIndex = function (aIndex) {
                var ret = {
                    top: 0,
                    left: 0
                };
                var focusedItem = this.getFocusedElement();
                var curIndex = parseInt(focusedItem.getAttribute('data'));
                if (this._keyMapHint) {
                    var curPosition = this.getPositionFromHintArray(curIndex);
                    var nextPosition = this.getPositionFromHintArray(aIndex);
                    if (this._getDrawParam(KParamStrOrientation) === 1 /* EVertical */) {
                        var itemHeight = this.getItemHeight();
                        ret.top = (curPosition.row - nextPosition.row) * itemHeight;
                    }
                }
                else {
                    if (this._getDrawParam(KParamStrOrientation) === 1 /* EVertical */) {
                        var maxCol = this._getDrawParam(KParamStrMaxColCount);
                        var curRow = Math.floor(curIndex / maxCol);
                        var nextRow = Math.floor(aIndex / maxCol);
                        var itemHeight = this.getItemHeight();
                        ret.top = (curRow - nextRow) * itemHeight;
                    }
                }
                return ret;
            };
            CGridDataControl.prototype.endTransition = function () {
                this._fnEndTransition();
            };
            CGridDataControl.prototype.getSize = function (aPageSize) {
                var horizontal = (this._getDrawParam(KParamStrOrientation) === 2 /* EHorizontal */);
                var itemHeight = this._getDrawParam(KParamStrItemHeight) || 0;
                var itemWidth = this._getDrawParam(KParamStrItemWidth) || 0;
                var maxRowCount = this._getDrawParam(KParamStrMaxRowCount) || 1;
                var maxColCount = this._getDrawParam(KParamStrMaxColCount) || 1;
                var count = this._ownedDataProvider.getLength();
                var w = itemWidth * maxColCount;
                var h;
                if (this._drawerHint) {
                    var arrayBlock = this._getHintArray();
                    h = itemHeight * arrayBlock.length;
                }
                else {
                    if (horizontal) {
                        w = itemWidth * Math.ceil(count / maxRowCount);
                        h = itemHeight * maxRowCount;
                    }
                    else {
                        h = itemHeight * Math.ceil(count / maxColCount);
                    }
                }
                return {
                    width: w,
                    height: h
                };
            };
            CGridDataControl.prototype._getHintArray = function () {
                if (!this._keyMapHint && this._drawerHint) {
                    var i, j;
                    var arrayBlock = new Array(new Array(3));
                    var arrayRowIndex = 0;
                    var arrayColIndex = 0;
                    var itemIndex = 0;
                    var itemCount = this._ownedDataProvider.getLength();
                    var maxColCount = this._getDrawParam(KParamStrMaxColCount);
                    for (i = 0; itemIndex < itemCount; i++) {
                        if (arrayColIndex >= maxColCount) {
                            arrayColIndex = 0;
                            arrayRowIndex += 1;
                            if (!arrayBlock[arrayRowIndex]) {
                                arrayBlock.push(new Array(3));
                            }
                        }
                        var hint = this.getDrawerHint(itemIndex);
                        if (hint) {
                            var rowSpan = hint.span.row;
                            var colSpan = hint.span.col;
                            var r, c;
                            var tempColIndex = arrayColIndex;
                            for (r = 0; r < rowSpan; r++) {
                                for (c = 0; c < colSpan; c++) {
                                    arrayBlock[arrayRowIndex + r][arrayColIndex] = itemIndex;
                                    arrayColIndex += 1;
                                }
                                arrayColIndex = tempColIndex;
                                if (r !== rowSpan - 1) {
                                    arrayBlock.push(new Array(3));
                                }
                            }
                            itemIndex += 1;
                            arrayColIndex = tempColIndex + colSpan;
                        }
                        else {
                            if (arrayBlock[arrayRowIndex][arrayColIndex] === undefined) {
                                arrayBlock[arrayRowIndex][arrayColIndex] = itemIndex++;
                                arrayColIndex += 1;
                            }
                            else {
                                arrayColIndex += 1;
                                continue;
                            }
                        }
                    }
                    this._keyMapHint = arrayBlock;
                }
                return this._keyMapHint;
            };
            CGridDataControl.prototype._doDrawForSpan = function (aRect, aDrawParam) {
                var focusableElements = [];
                var dp = this._ownedDataProvider;
                var itemCount = dp.getLength();
                var itemWidth = aDrawParam[KParamStrItemWidth];
                var itemHeight = aDrawParam[KParamStrItemHeight];
                var maxColCount = aDrawParam[KParamStrMaxColCount];
                var horizontal = (aDrawParam[KParamStrOrientation] === 2 /* EHorizontal */);
                var startIndex = aDrawParam[KParamStrStartIndex];
                var prevDrawnItem = this._pickDrawnElements();
                var contentAvailable = {
                    up: 0,
                    left: 0,
                    right: 0,
                    down: 0
                };
                this._keyMapBuilder = CtrlBase.KBuilderGrid;
                var arrayBlock = this._getHintArray();
                var curRow = Math.floor(aRect.top / itemHeight);
                var curCol = 0;
                var lastRow = Math.ceil(aRect.bottom / itemHeight);
                var rowLen = arrayBlock.length;
                var itemIndex = arrayBlock[curRow][curCol];
                var i, j;
                for (i = 0; i < rowLen; i++) {
                    var rowBlock = arrayBlock[i];
                    for (j = 0; j < rowBlock.length; j++) {
                        if (i >= curRow && i <= lastRow) {
                            var index = arrayBlock[i][j];
                            if (index !== undefined && !this._getDrawnElement(index)) {
                                var hint = this.getDrawerHint(index);
                                var el = prevDrawnItem.pickElement(index);
                                var elTop = i * itemHeight;
                                var elLeft = j * itemWidth;
                                var elRight, elBottom;
                                var isNewEl = false;
                                if (!el) {
                                    el = document.createElement('div');
                                    el.setAttribute('data', index);
                                    el.attributes['data'] = index;
                                    el.classList.add('-grid-item');
                                    el.style.position = 'absolute';
                                    if (hint) {
                                        var position = this.getPositionFromHintArray(index);
                                        if (position.row !== i || position.col !== j) {
                                            elTop = position.row * itemHeight;
                                            elLeft = position.col * itemWidth;
                                        }
                                        el.style.top = elTop + 'px';
                                        el.style.left = elLeft + 'px';
                                        el.style.width = itemWidth * hint.span.col + 'px';
                                        el.style.height = itemHeight * hint.span.row + 'px';
                                        elRight = elLeft + itemWidth * hint.span.col;
                                        elBottom = elTop + itemHeight * hint.span.row;
                                        el.classList.add('-span');
                                    }
                                    else {
                                        el.style.top = elTop + 'px';
                                        el.style.left = elLeft + 'px';
                                        el.style.width = itemWidth + 'px';
                                        el.style.height = itemHeight + 'px';
                                        elRight = elLeft + itemWidth;
                                        elBottom = elTop + itemHeight;
                                    }
                                    isNewEl = true;
                                }
                                else {
                                    elTop = parseInt(el.style.top);
                                    elLeft = parseInt(el.style.left);
                                    elRight = elLeft + parseInt(el.style.width);
                                    elBottom = elTop + parseInt(el.style.height);
                                }
                                // TO Draw
                                if (elTop < aRect.bottom && elBottom > aRect.top && elLeft < aRect.right && elRight > aRect.left) {
                                    if (isNewEl) {
                                        var item = dp.getItem(index);
                                        var focusInfo = this._drawer(index, item, el);
                                        this._element.appendChild(el);
                                    }
                                    else {
                                        if (el.classList.contains(KClassActiveFocusedLeaf)) {
                                            el.classList.remove(KClassActiveFocusedLeaf);
                                        }
                                    }
                                    this._setDrawnElement(index, el);
                                    // TO Focus
                                    if (elTop >= aRect.top && elBottom <= aRect.bottom && elLeft >= aRect.left && elRight <= aRect.right) {
                                        if (startIndex === index) {
                                            el.classList.add(KClassFocused);
                                        }
                                        el.classList.add(KClassFocusable);
                                        focusableElements.push(el);
                                    }
                                    else if (el.classList.contains(KClassFocusable)) {
                                        el.classList.remove(KClassFocusable);
                                    }
                                }
                                else {
                                    prevDrawnItem.setElement(index, el);
                                }
                            }
                        }
                    }
                }
                if (!this.getAnimation()) {
                    prevDrawnItem.destroy();
                }
                else {
                    prevDrawnItem.forEach(function (aKey, aItem) {
                        var ret = false;
                        if (aItem.classList.contains(CtrlBase.TClassStr.KClassActiveFocusedLeaf)) {
                            aItem.classList.remove(CtrlBase.TClassStr.KClassActiveFocusedLeaf);
                            ret = true;
                        }
                        return ret;
                    });
                    this._fnEndTransition = function () {
                        prevDrawnItem.destroy();
                    };
                }
                if (itemCount) {
                    var size = this.getSize();
                    var first = focusableElements[0];
                    var last = focusableElements[focusableElements.length - 1];
                    if (horizontal) {
                        var left = first.offsetTop - contentAvailable.left;
                        var right = last.offsetTop + last.offsetHeight + contentAvailable.right;
                        contentAvailable.left = left;
                        contentAvailable.right = size.width - right;
                    }
                    else {
                        if (size.height > aRect.getHeight()) {
                            var top = first.offsetTop - contentAvailable.up;
                            var down = last.offsetTop + last.offsetHeight + contentAvailable.down;
                            contentAvailable.up = top;
                            contentAvailable.down = size.height - down;
                        }
                    }
                }
                //            this._contentAvail = contentAvailable;
                this.setContentAvail(contentAvailable);
                return focusableElements;
            };
            CGridDataControl.prototype._doDraw = function (aRect, aDrawParam) {
                if (this._drawerHint) {
                    return this._doDrawForSpan(aRect, aDrawParam);
                }
                else {
                    var focusableElements = [];
                    var dp = this._ownedDataProvider;
                    var itemCount = dp.getLength();
                    var horizontal = (aDrawParam[KParamStrOrientation] === 2 /* EHorizontal */);
                    var itemWidth = aDrawParam[KParamStrItemWidth];
                    var itemHeight = aDrawParam[KParamStrItemHeight];
                    var maxColCount = aDrawParam[KParamStrMaxColCount];
                    var maxRowCount = aDrawParam[KParamStrMaxRowCount];
                    var curCol;
                    var curRow;
                    var startIndex;
                    var startFocusIndex;
                    curCol = aRect ? Math.floor(aRect.left / itemWidth) : 0;
                    curRow = aRect ? Math.floor(aRect.top / itemHeight) : 0;
                    if (horizontal) {
                        startIndex = aRect ? curCol * maxRowCount + curRow : 0;
                    }
                    else {
                        startIndex = aRect ? curRow * maxColCount + curCol : 0;
                    }
                    startFocusIndex = aDrawParam[KParamStrStartIndex];
                    var prevDrawnItem = this._pickDrawnElements();
                    var contentAvailable = {
                        up: 0,
                        left: 0,
                        right: 0,
                        down: 0
                    };
                    var i;
                    if (horizontal) {
                        this._keyMapBuilder = CtrlBase.KBuilderGridHorizontal;
                    }
                    else {
                        this._keyMapBuilder = CtrlBase.KBuilderGrid;
                    }
                    for (i = startIndex; i < itemCount; i++) {
                        var colCount;
                        var rowCount;
                        if (horizontal) {
                            rowCount = i % maxRowCount;
                            colCount = Math.floor(i / maxRowCount);
                        }
                        else {
                            colCount = i % maxColCount;
                            rowCount = Math.floor(i / maxColCount);
                        }
                        var elTop = rowCount * itemHeight;
                        var elLeft = colCount * itemWidth;
                        var elBottom = elTop + itemHeight;
                        var elRight = elLeft + itemWidth;
                        if (elTop < aRect.bottom && elLeft < aRect.right) {
                            var el = prevDrawnItem.pickElement(i);
                            var item = dp.getItem(i);
                            if (!el) {
                                el = document.createElement('div');
                                el.setAttribute("data", i);
                                el.classList.add('-grid-item');
                                el.attributes['data'] = i;
                                el.style.position = 'absolute';
                                el.style.top = elTop + 'px';
                                el.style.left = elLeft + 'px';
                                el.style.width = itemWidth + 'px';
                                el.style.height = itemHeight + 'px';
                                var focusInfo = this._drawer(i, item, el);
                                if (focusInfo === 2 /* KFocusAble */) {
                                    el.classList.add(CtrlBase.TClassStr.KClassFocusable);
                                }
                                if (focusInfo !== 0 /* KFocusUnknown */) {
                                    this._element.appendChild(el);
                                }
                                this._element.appendChild(el);
                            }
                            if (el.classList.contains(KClassActiveFocusedLeaf)) {
                                el.classList.remove(KClassActiveFocusedLeaf);
                            }
                            this._setDrawnElement(i, el);
                            if (aRect.top <= elTop && aRect.left <= elLeft && aRect.bottom >= elBottom && aRect.right >= elRight) {
                                if (el.classList.contains(KClassFocusDisable)) {
                                    el.classList.remove(KClassFocusDisable);
                                    el.classList.add(KClassFocusable);
                                }
                                if (el.classList.contains(KClassFocusable)) {
                                    if (startFocusIndex === i) {
                                        el.classList.add(KClassFocused);
                                    }
                                    //                                el.classList.add(KClassFocusable);
                                    focusableElements.push(el);
                                }
                            }
                            else if (el.classList.contains(KClassFocusable)) {
                                el.classList.remove(KClassFocusable);
                                el.classList.add(KClassFocusDisable);
                            }
                            if (horizontal) {
                                if (elRight > aRect.right) {
                                    contentAvailable.right = aRect.right - elLeft;
                                }
                                else if (elLeft < aRect.left) {
                                    contentAvailable.left = aRect.left - elRight;
                                }
                            }
                            else {
                                if (elBottom > aRect.bottom) {
                                    contentAvailable.down = aRect.bottom - elTop;
                                }
                                else if (elTop < aRect.top) {
                                    contentAvailable.up = aRect.top - elBottom;
                                }
                            }
                        }
                    }
                    if (!this.getAnimation()) {
                        prevDrawnItem.destroy();
                    }
                    else {
                        prevDrawnItem.forEach(function (aKey, aItem) {
                            var ret = false;
                            if (aItem.classList.contains(CtrlBase.TClassStr.KClassActiveFocusedLeaf)) {
                                aItem.classList.remove(CtrlBase.TClassStr.KClassActiveFocusedLeaf);
                                ret = true;
                            }
                            return ret;
                        });
                        this._fnEndTransition = function () {
                            prevDrawnItem.destroy();
                        };
                    }
                    if (itemCount) {
                        var size = this.getSize();
                        var first = focusableElements[0];
                        var last = focusableElements[focusableElements.length - 1];
                        if (horizontal) {
                            var left = first ? first.offsetLeft - contentAvailable.left : 0;
                            var right = last ? last.offsetLeft + last.offsetWidth + contentAvailable.right : 0;
                            contentAvailable.left = left;
                            contentAvailable.right = size.width - right;
                        }
                        else {
                            var top = first ? first.offsetTop - contentAvailable.up : 0;
                            var down = last ? last.offsetTop + last.offsetHeight + contentAvailable.down : 0;
                            contentAvailable.up = top;
                            contentAvailable.down = size.height - down;
                        }
                    }
                    //                this._contentAvail = contentAvailable;
                    this.setContentAvail(contentAvailable);
                    return focusableElements;
                }
            };
            CGridDataControl.prototype.setMaxRowCount = function (aMaxRowCount) {
                this._setDrawParam(KParamStrMaxRowCount, aMaxRowCount, false);
                this._setDrawParam(KParamStrOrientation, 1 /* EVertical */, false);
                this.setMaxColCount = function (x) {
                    throw "can not setMaxColCount after setMaxRowCount";
                };
            };
            CGridDataControl.prototype.setMaxColCount = function (aMaxColCount) {
                this._setDrawParam(KParamStrMaxColCount, aMaxColCount, false);
                this._setDrawParam(KParamStrOrientation, 1 /* EVertical */, false);
                this.setMaxRowCount = function (x) {
                    throw "can not setMaxRowCount after setMaxColCount ";
                };
            };
            CGridDataControl.prototype.getMaxRowCount = function () {
                return this._getDrawParam(KParamStrMaxRowCount);
            };
            CGridDataControl.prototype.doItemInserted = function (aKey, aItems, aNeedFocus) {
                this.emit.call(this, "ItemInserted", this._drawnElements, aNeedFocus);
            };
            CGridDataControl.prototype.doItemRemoved = function (aKey, aUnsetFocus) {
                this.emit.call(this, "ItemRemoved", this._drawnElements, aUnsetFocus);
            };
            CGridDataControl.prototype.getPositionFromHintArray = function (aItem, aMax) {
                var ret = null;
                if (this._keyMapHint) {
                    var i, j, rowLen = this._keyMapHint.length;
                    for (i = 0; i < rowLen; i++) {
                        var temp = this._keyMapHint[i];
                        var colLen = temp.length;
                        for (j = 0; j < colLen; j++) {
                            if (temp[j] === aItem) {
                                if (aMax) {
                                    if (ret) {
                                        ret.maxCol = j;
                                        ret.maxRow = i;
                                    }
                                    else {
                                        ret = {
                                            col: j,
                                            row: i,
                                            maxCol: j,
                                            maxRow: i
                                        };
                                    }
                                }
                                else {
                                    ret = {
                                        col: j,
                                        row: i
                                    };
                                    break;
                                }
                            }
                        }
                        if (!aMax && ret !== null) {
                            break;
                        }
                    }
                }
                return ret;
            };
            return CGridDataControl;
        })(CDataControl);
        CtrlBase.CGridDataControl = CGridDataControl;
        var CGroupControl = (function (_super) {
            __extends(CGroupControl, _super);
            function CGroupControl(aElement) {
                //FIXME: jQuery
                _super.call(this, $(aElement), "-group");
                this._group = true;
                this._children = [];
                this.registerSignal(["ChildViewMoved"]);
            }
            CGroupControl.prototype._destroyChild = function () {
                var i, len, c;
                for (i = 0, len = this._children.length; i < len; i++) {
                    c = this._children[i];
                    c.destroy();
                }
            };
            /*
            destroy() {
                this._destroyChild();
    
            }
            */
            CGroupControl.prototype.setKeyBuilder = function (aLayout) {
                if (aLayout == 1 /* EVertical */) {
                    this._keyMapBuilder = CtrlBase.KBuilderTopDown;
                }
                else if (aLayout == 2 /* EHorizontal */) {
                    this._keyMapBuilder = CtrlBase.KBuilderLeftRight;
                }
                else {
                    this._keyMapBuilder = CtrlBase.KBuilderLeftRight;
                }
            };
            CGroupControl.prototype.setOwnedChildControls = function (aChildControls, aFocusedChildIndex) {
                this._destroyChild();
                var i, len;
                for (i = 0, len = aChildControls.length; i < len; i += 1) {
                    aChildControls[i].setParent(this);
                }
                if (aFocusedChildIndex && aChildControls[aFocusedChildIndex]) {
                    aChildControls[aFocusedChildIndex].setFocus(true);
                }
                else {
                    aChildControls[0].setFocus(true);
                }
                this._children = aChildControls;
            };
            CGroupControl.prototype.insertOwnedChild = function (aIndex, aControl) {
                this._children.unshift(aControl);
            };
            CGroupControl.prototype.appendOwnedChild = function (aControl) {
                this._children.push(aControl);
            };
            CGroupControl.prototype.removeOwnedControl = function (aControl) {
                var index = this._children.indexOf(aControl);
                if (index != -1) {
                    this._children.splice(index, 1);
                    aControl.destroy();
                }
            };
            CGroupControl.prototype.draw = function (aRect) {
                _super.prototype.draw.call(this, aRect);
                //            this.setActiveFocus();
            };
            CGroupControl.prototype._doDraw = function (aRect, aDrawParam) {
                return this._doDrawCommon(this._element, aRect, aDrawParam);
            };
            CGroupControl.prototype._doDrawCommon = function (aParent, aRect, aDrawParam) {
                var ret = [];
                this._element.classList.add(KClassFocused);
                var i, len, c, el;
                for (i = 0, len = this._children.length; i < len; i++) {
                    c = this._children[i];
                    el = c._element;
                    el.attributes["data"] = i;
                    aParent.appendChild(el);
                    c.draw(aRect);
                    if (c.isFocusable()) {
                        ret.push(el);
                    }
                }
                return ret;
            };
            CGroupControl.prototype.getFocusedChild = function () {
                var i, len, c;
                for (i = 0, len = this._children.length; i < len; i++) {
                    c = this._children[i];
                    if (c.isFocused()) {
                        return c;
                    }
                }
                return null;
            };
            CGroupControl.prototype._setActiveFocus = function (aFocus) {
                var focusedChild;
                if (this.isFocusable()) {
                    if (aFocus) {
                        this._element.classList.add(KClassFocused);
                    }
                    else {
                        this._element.classList.remove(KClassFocused);
                    }
                    focusedChild = this.getFocusedChild();
                    focusedChild._setActiveFocus(aFocus);
                }
            };
            // CGroupControl events
            CGroupControl.prototype.connectChildFocusChanged = function (aHolder, aSlotName, aSlot) {
                this.connect("ChildFocusChanged", aHolder, aSlotName);
            };
            CGroupControl.prototype._handleViewMoved = function (aControl, aIncrement) {
                this._emitChildViewMoved(aControl, aIncrement);
            };
            CGroupControl.prototype.connectChildViewMoved = function (aHolder, aSlotName, aSlot) {
                this.connect("ChildViewMoved", aHolder, aSlotName);
            };
            CGroupControl.prototype._emitChildViewMoved = function (aControl, aIncrement) {
                this.emit.call(aControl, "ChildViewMoved", this, aIncrement);
            };
            return CGroupControl;
        })(CControl);
        CtrlBase.CGroupControl = CGroupControl;
        var CViewGroupControl = (function (_super) {
            __extends(CViewGroupControl, _super);
            function CViewGroupControl(aElement) {
                _super.call(this, document.createElement("div"));
                this._showScrollbar = false;
                this._element.classList.add("-group-view");
                this._element.style.overflow = "hidden";
                this._container = document.createElement("div");
                this._container.classList.add("-group-view-container");
                this._container.style.position = "absolute";
                this._container.style.top = "0px";
                this._container.style.left = "0px";
                //FIXME dom doder
                this._element.appendChild(this._container);
                aElement.appendChild(this._element);
                this._keyMapBuilder = CtrlBase.KBuilderTopDown;
            }
            CViewGroupControl.prototype.setClassToGroupViewElement = function (aClassName) {
                var classes = aClassName.split(" ");
                for (var i = 0; i < classes.length; i++) {
                    this._element.classList.add(classes[i]);
                }
            };
            CViewGroupControl.prototype.setOwnedChildControls = function (aChildControls) {
                if (aChildControls.length != 1) {
                    throw "just single child supported";
                }
                this._targetChild = aChildControls[0];
                _super.prototype.setOwnedChildControls.call(this, aChildControls);
            };
            CViewGroupControl.prototype.setScrollScheme = function (aScheme, aFixedScrollUnit) {
                _super.prototype.setScrollScheme.call(this, aScheme, aFixedScrollUnit);
                //this._targetChild.setScrollScheme(aScheme, aFixedScrollUnit);
            };
            CViewGroupControl.prototype.setVerticalScrollScheme = function (aScheme, aFixedScrollUnit) {
                _super.prototype.setVerticalScrollScheme.call(this, aScheme, aFixedScrollUnit);
                //this._targetChild.setVerticalScrollScheme(aScheme, aFixedScrollUnit);
            };
            CViewGroupControl.prototype.setHorizontalScrollScheme = function (aScheme, aFixedScrollUnit) {
                _super.prototype.setHorizontalScrollScheme.call(this, aScheme, aFixedScrollUnit);
                //this._targetChild.setHorizontalScrollScheme(aScheme, aFixedScrollUnit);
            };
            CViewGroupControl.prototype.setKeepFocus = function (aKeepFocus) {
                _super.prototype.setKeepFocus.call(this, aKeepFocus);
                //this._targetChild.setKeepFocus(aKeepFocus);
            };
            CViewGroupControl.prototype.setShowScrollbar = function (aShowScrollbar) {
                this._showScrollbar = aShowScrollbar;
            };
            CViewGroupControl.prototype.setScrollbarBgUserHeight = function (aHeight) {
                this._scrollBarBgUserHeight = aHeight;
            };
            CViewGroupControl.prototype.setScrollbarTrackUserHeight = function (aHeight) {
                this._scrollBarTrackUserHeight = aHeight;
            };
            CViewGroupControl.prototype.setViewSize = function (aRect) {
                this._element.style.left = aRect.left + "px";
                this._element.style.top = aRect.top + "px";
                this._element.style.height = aRect.getHeight() + "px";
                this._element.style.width = aRect.getWidth() + "px";
            };
            CViewGroupControl.prototype.moveFocusOutOfKeymap = function (aIndex) {
                var focusedChild = this.getFocusedChild();
                var customScrollSize = focusedChild.getCustomScrollSizeByIndex(aIndex);
                if (customScrollSize && customScrollSize.top) {
                    if (this._drawnRect.top < customScrollSize.top) {
                        customScrollSize.top = this._drawnRect.top;
                    }
                    var incrementRow = Math.floor(customScrollSize.top / focusedChild.getItemHeight());
                    this._handleViewMoved(this, customScrollSize);
                    this.moveDrawPosition(customScrollSize);
                    if (!focusedChild._keyMapHint) {
                        var i, len = focusedChild._focusableElement ? focusedChild._focusableElement.length : 0;
                        for (i = 0; i < len; i++) {
                            var item = focusedChild._focusableElement[i];
                            if (aIndex === parseInt(item.getAttribute('data'))) {
                                aIndex = i;
                                break;
                            }
                        }
                    }
                    focusedChild.setFocusedElementByIndex(aIndex);
                    focusedChild.setActiveFocus();
                }
            };
            CViewGroupControl.prototype._doDraw = function (aRect, aDrawParam) {
                if (this._element.offsetWidth == 0 || this._element.offsetHeight == 0) {
                    throw "drawing view group without size meaning-less";
                }
                var focusableElements = [];
                var drawPosTop;
                var drawPosLeft;
                if (this.getAnimation() && this._getContainerPosForAni()) {
                    drawPosTop = -this._getContainerPosForAni().top;
                    drawPosLeft = -this._getContainerPosForAni().left;
                    this._setContainerPosForAni(undefined);
                }
                else {
                    drawPosTop = -this._container.offsetTop;
                    drawPosLeft = -this._container.offsetLeft;
                }
                var drawRect = new TRect({
                    top: drawPosTop,
                    left: drawPosLeft,
                    right: drawPosLeft + this._element.offsetWidth,
                    bottom: drawPosTop + this._element.offsetHeight
                });
                var i, len, c, el; // looping param
                var childRect = new TRect; // rectangle for child control
                var childSize; // size of child control
                var drawRectForChild; // rectangle param for child control
                for (i = 0, len = this._children.length; i < len; i++) {
                    c = this._children[i];
                    el = c._element;
                    el.style.top = childRect.top + "px";
                    this._container.appendChild(el);
                    childSize = c.getSize(this.getSize());
                    childRect.setHeight(childSize.height);
                    childRect.setWidth(childSize.width);
                    var scrollSchemeVertical = this._getDrawParam(KParamStrScrollSchemeVertical);
                    var scrollSchemeHorizontal = this._getDrawParam(KParamStrScrollSchemeHorizontal);
                    if (scrollSchemeVertical === 3 /* EByFixedFocusRemains */ || scrollSchemeHorizontal === 4 /* EByVariableFocusRemains */) {
                        drawRectForChild = drawRect;
                    }
                    else {
                        drawRectForChild = drawRect.getIntersectedRect(childRect);
                    }
                    c.draw(drawRectForChild || new TRect());
                    if (c.isFocusable()) {
                        focusableElements.push(el);
                    }
                    childRect.moveTop(childRect.getHeight());
                    if (this._showScrollbar && childRect.getHeight() > drawRectForChild.getHeight()) {
                        var barBgHeight = this._scrollBarBgUserHeight ? this._scrollBarBgUserHeight : this._element.offsetHeight;
                        if (!this._scrollBarTrack) {
                            var scrollBarBg = document.createElement('div');
                            scrollBarBg.classList.add('-scroll');
                            scrollBarBg.style.position = 'absolute';
                            scrollBarBg.style.width = '5px';
                            scrollBarBg.style.height = barBgHeight + 'px';
                            scrollBarBg.style.left = this._element.offsetWidth - 5 + 'px';
                            scrollBarBg.style.backgroundColor = 'rgba(255, 255, 255, .2)';
                            var scrollBarTrack = document.createElement('div');
                            scrollBarTrack.classList.add('-scroll-track');
                            scrollBarTrack.style.position = 'absolute';
                            scrollBarTrack.style.width = 'inherit';
                            scrollBarTrack.style.transition = KCssTransitionParamPos;
                            scrollBarBg.appendChild(scrollBarTrack);
                            this._element.appendChild(scrollBarBg);
                            this._scrollBarBg = scrollBarBg;
                            this._scrollBarTrack = scrollBarTrack;
                        }
                        else if (this._scrollBarTrack.style.display === 'none') {
                            this._scrollBarBg.style.display = 'block';
                            this._scrollBarTrack.style.display = 'block';
                        }
                        if (this._scrollBarTrackUserHeight) {
                            this._scrollBarTrack.style.height = this._scrollBarTrackUserHeight + 'px';
                            this._scrollBarTrack.style.top = Math.floor(((barBgHeight - this._scrollBarTrackUserHeight) * drawRectForChild.top) / (childRect.getHeight() - drawRectForChild.getHeight())) + 'px';
                        }
                        else {
                            this._scrollBarTrack.style.height = Math.floor((drawRectForChild.getHeight() * barBgHeight) / childRect.getHeight()) + 'px';
                            this._scrollBarTrack.style.top = Math.floor((drawRectForChild.top * barBgHeight) / childRect.getHeight()) + 'px';
                        }
                    }
                    else if (this._scrollBarTrack) {
                        this._scrollBarBg.style.display = 'none';
                        this._scrollBarTrack.style.display = 'none';
                    }
                }
                this._drawnRect = drawRect;
                return focusableElements;
            };
            CViewGroupControl.prototype._getNextMove = function (aKeyStr, aControl) {
                var ret = {
                    top: 0,
                    left: 0
                };
                var contentAvail = aControl.getContentAvail();
                var size = this.getSize();
                if (contentAvail) {
                    switch (aKeyStr) {
                        case KKeyStrUp:
                            if (contentAvail.up) {
                                ret.top = Math.min(contentAvail.up, size.height);
                            }
                            break;
                        case KKeyStrDown:
                            if (contentAvail.down) {
                                ret.top = -Math.min(contentAvail.down, size.height);
                            }
                            break;
                        case KKeyStrLeft:
                            if (contentAvail.left) {
                                ret.left = Math.min(contentAvail.left, size.width);
                            }
                            break;
                        case KKeyStrRight:
                            if (contentAvail.right) {
                                ret.left = -Math.min(contentAvail.right, size.width);
                            }
                            break;
                    }
                }
                return ret;
            };
            CViewGroupControl.prototype._setContainerPosForAni = function (aPosition) {
                this._containerPosForAni = aPosition;
            };
            CViewGroupControl.prototype._getContainerPosForAni = function () {
                return this._containerPosForAni;
            };
            CViewGroupControl.prototype._getContainerPos = function () {
                var containerTop = this._container.offsetTop;
                var containerLeft = this._container.offsetLeft;
                return {
                    top: containerTop,
                    left: containerLeft
                };
            };
            CViewGroupControl.prototype.moveDrawPosition = function (aPosition) {
                var contPos = this._getContainerPos();
                var param = {
                    top: contPos.top + aPosition.top,
                    left: contPos.left + aPosition.left
                };
                this.setDrawPosition(param);
                //            var bDraw: boolean = true;
                //            var scrollScheme = this.getVerticalScrollScheme();
                //
                //            if ((<CListDataControl>this.getFocusedChild()).getDataRolling()){
                //                //debugger;
                //            } else {
                //                if (scrollScheme === CtrlBase.TParamScrollScheme.EByFixedFocusRemains || scrollScheme === CtrlBase.TParamScrollScheme.EByVariableFocusRemains) {
                //                    if (!(param.top <= 0 && param.left <= 0)) {
                //                        bDraw = false;
                //                    }
                //                }
                //            }
                //            if (bDraw) {
                //                this.setDrawPosition(param);
                //            }
            };
            CViewGroupControl.prototype.initDrawPosition = function (aPosition) {
                var pos = (aPosition) ? aPosition : { top: 0, left: 0 };
                this._setContainerPosForAni(pos);
                this._container.style.top = pos.top + "px";
                this._container.style.left = pos.left + "px";
                this.draw();
            };
            CViewGroupControl.prototype.setDrawPosition = function (aPosition) {
                var _this = this;
                if (this.getAnimation()) {
                    Util.afterTransition(this._container, function () {
                        _this.setTransition(false);
                    }, true);
                    this._setContainerPosForAni(aPosition);
                    this.draw();
                    this.setTransition(true);
                    this._container.style.top = aPosition.top + "px";
                    this._container.style.left = aPosition.left + "px";
                }
                else {
                    this._container.style.top = aPosition.top + "px";
                    this._container.style.left = aPosition.left + "px";
                    this.draw();
                }
            };
            CViewGroupControl.prototype.setAnimation = function (aAnimation) {
                _super.prototype.setAnimation.call(this, aAnimation);
                if (aAnimation) {
                    this._container.style[KCssPropTransition] = KCssTransitionParamPos;
                }
                else {
                    this._container.style[KCssPropTransition] = '';
                }
            };
            CViewGroupControl.prototype._getScrollSizeVertical = function (aUp, aPage) {
                var ret = {
                    top: 0,
                    left: 0
                };
                var focusedChild = this.getFocusedChild();
                var contentAvail = focusedChild.getContentAvail();
                if (aUp && contentAvail.up === 0) {
                    return ret;
                }
                else if (!aUp && contentAvail.down === 0) {
                    return ret;
                }
                var scrollScheme = this.getVerticalScrollScheme();
                var itemHeight = focusedChild.getItemHeight();
                switch (scrollScheme) {
                    case 1 /* EByItem */:
                        if (aPage) {
                            ret.top = Math.floor(this._drawnRect.getHeight() / itemHeight) * itemHeight;
                        }
                        else {
                            if (focusedChild._drawerHint) {
                                var customSrollSize = focusedChild.getCustomScrollSize(aUp ? KKeyStrUp : KKeyStrDown, this._drawnRect);
                                ret.top = customSrollSize ? customSrollSize.up : itemHeight;
                            }
                            else {
                                ret.top = itemHeight;
                            }
                        }
                        break;
                    case 2 /* EByPage */:
                        ret.top = this._drawnRect.getHeight();
                        break;
                    case 5 /* EByFixed */:
                        var unit = this._getDrawParam(KParamStrScrollSchemeFixedUnitVertical);
                        if (aPage) {
                            ret.top = Math.floor(this._drawnRect.getHeight() / unit) * unit;
                        }
                        else {
                            ret.top = unit;
                        }
                        break;
                    case 3 /* EByFixedFocusRemains */:
                    case 4 /* EByVariableFocusRemains */:
                        var containerPos = this._getContainerPos();
                        if (aPage) {
                            if (aUp) {
                                var firstFocusableElement = focusedChild.getElementByIndex(0);
                                ret.top = this._drawnRect.getHeight() - (containerPos.top + firstFocusableElement.offsetTop + itemHeight);
                            }
                            else {
                                var lastFocusableElement = focusedChild.getElementByIndex(focusedChild.getFocusableElementCount() - 1);
                                ret.top = containerPos.top + lastFocusableElement.offsetTop;
                            }
                        }
                        else {
                            var focusedElement = focusedChild.getFocusedElement();
                            if (aUp) {
                                ret.top = this._drawnRect.getHeight() - (containerPos.top + focusedElement.offsetTop + itemHeight);
                            }
                            else {
                                ret.top = containerPos.top + focusedElement.offsetTop;
                            }
                        }
                        break;
                    default:
                        break;
                }
                if (aUp) {
                    if (ret.top > contentAvail.up) {
                        ret.top = contentAvail.up;
                    }
                }
                else {
                    if (ret.top > contentAvail.down) {
                        if (scrollScheme === 4 /* EByVariableFocusRemains */) {
                            ret.top = contentAvail.down;
                        }
                        else if (aPage && scrollScheme === 1 /* EByItem */) {
                            ret.top = Math.ceil(contentAvail.down / itemHeight) * itemHeight;
                        }
                        else if (this._drawnRect.getHeight() < ret.top) {
                            ret.top = this._drawnRect.getHeight();
                        }
                    }
                    ret.top = -ret.top;
                }
                console.log('ret.top=' + ret.top);
                return ret;
            };
            CViewGroupControl.prototype._getScrollSizeHorizontal = function (aLeft) {
                var ret = {
                    top: 0,
                    left: 0
                };
                var focusedChild = this.getFocusedChild();
                var contentAvail = focusedChild.getContentAvail();
                var oriental = focusedChild._getDrawParam(KParamStrOrientation);
                var scrollScheme = this.getHorizontalScrollScheme();
                var itemWidth = focusedChild.getItemWidth();
                if (oriental === 2 /* EHorizontal */) {
                    var containerPos = this._getContainerPos();
                    var focusedElement = focusedChild.getFocusedElement();
                    if (aLeft && contentAvail.left === 0) {
                        return ret;
                    }
                    else if (!aLeft && contentAvail.right === 0) {
                        return ret;
                    }
                    switch (scrollScheme) {
                        case 1 /* EByItem */:
                            ret.left = itemWidth;
                            break;
                        case 2 /* EByPage */:
                            ret.left = this._drawnRect.getWidth();
                            break;
                        case 5 /* EByFixed */:
                            if (aLeft) {
                                if (contentAvail.left > 0) {
                                    ret.left = itemWidth;
                                }
                            }
                            else if (contentAvail.right > itemWidth) {
                                ret.left = itemWidth;
                            }
                            break;
                        case 3 /* EByFixedFocusRemains */:
                        case 4 /* EByVariableFocusRemains */:
                            if (aLeft) {
                                ret.left = this._drawnRect.getWidth() - (containerPos.left + focusedElement.offsetLeft + itemWidth);
                            }
                            else {
                                ret.left = containerPos.left + focusedElement.offsetLeft;
                            }
                            break;
                        default:
                            break;
                    }
                    if (aLeft) {
                        if (contentAvail.left !== 0 && ret.left > contentAvail.left) {
                            ret.left = contentAvail.left;
                        }
                    }
                    else {
                        if (ret.left > contentAvail.right) {
                            if (scrollScheme === 4 /* EByVariableFocusRemains */) {
                                ret.left = contentAvail.right;
                            }
                            else {
                                if (this._drawnRect.getWidth() < ret.left) {
                                    ret.left = this._drawnRect.getWidth();
                                }
                            }
                        }
                        ret.left = -ret.left;
                    }
                    console.log('ret.left=' + ret.left);
                }
                else {
                    var customSrollSize = focusedChild.getCustomScrollSize(aLeft ? KKeyStrLeft : KKeyStrRight, this._drawnRect);
                    if (customSrollSize) {
                        ret.top = customSrollSize.up;
                    }
                }
                return ret;
            };
            CViewGroupControl.prototype._getNextFocusedIndex = function (aKeyStr, aIncrement) {
                var focusedChild = this.getFocusedChild();
                var ret = focusedChild.getFocusedElementIndex();
                var scrollScheme = this.getVerticalScrollScheme();
                var oriental = focusedChild._getDrawParam(KParamStrOrientation);
                switch (aKeyStr) {
                    case KKeyStrUp:
                    case KKeyStrDown:
                        var incrementRow = Math.floor(aIncrement.top / focusedChild.getItemHeight());
                        if (scrollScheme === 2 /* EByPage */ || scrollScheme === 3 /* EByFixedFocusRemains */ || scrollScheme === 4 /* EByVariableFocusRemains */) {
                            ret = focusedChild.getNextFocusIndex(incrementRow, true);
                        }
                        else if (scrollScheme === 5 /* EByFixed */) {
                            ret = focusedChild.getNextFocusIndex(incrementRow, false);
                        }
                        else if (focusedChild._drawerHint) {
                            ret = focusedChild.getNextFocusIndex(incrementRow, false);
                        }
                        break;
                    case KKeyStrChannelUp:
                    case KKeyStrChannelDown:
                        var incrementRow = Math.floor(aIncrement.top / focusedChild.getItemHeight());
                        if (focusedChild._drawerHint) {
                            ret = focusedChild.getNextFocusIndex(incrementRow, false);
                        }
                        break;
                    case KKeyStrLeft:
                    case KKeyStrRight:
                        if (oriental === 1 /* EVertical */) {
                            if (focusedChild._drawerHint) {
                                ret = focusedChild.getNextFocusIndex(null, false);
                            }
                        }
                        break;
                    default:
                        break;
                }
                return ret;
            };
            CViewGroupControl.prototype._fnScrolling2 = function (aKeyStr) {
                var handled = false;
                var focusedChild = this.getFocusedChild();
                var increment;
                switch (aKeyStr) {
                    case KKeyStrUp:
                        increment = this._getScrollSizeVertical(true, false);
                        break;
                    case KKeyStrDown:
                        increment = this._getScrollSizeVertical(false, false);
                        break;
                    case KKeyStrChannelUp:
                        increment = this._getScrollSizeVertical(true, true);
                        break;
                    case KKeyStrChannelDown:
                        increment = this._getScrollSizeVertical(false, true);
                        break;
                    case KKeyStrLeft:
                        increment = this._getScrollSizeHorizontal(true);
                        break;
                    case KKeyStrRight:
                        increment = this._getScrollSizeHorizontal(false);
                        break;
                    default:
                        break;
                }
                if (increment.top !== 0 || increment.left !== 0) {
                    var nextIndex = this._getNextFocusedIndex(aKeyStr, increment);
                    this._handleViewMoved(this, increment);
                    this.moveDrawPosition(increment);
                    var focusableCount = focusedChild.getFocusableElementCount();
                    focusedChild.setFocusedElementByIndex(nextIndex);
                    focusedChild.setActiveFocus();
                    if (focusedChild.getDataRolling()) {
                        if (nextIndex < 0) {
                            nextIndex = focusableCount - 1;
                        }
                        else if (nextIndex > focusableCount - 1) {
                            nextIndex = 0;
                        }
                    }
                    else {
                        if (nextIndex < 0) {
                            nextIndex = 0;
                        }
                        else if (nextIndex > focusableCount - 1) {
                            nextIndex = focusableCount - 1;
                        }
                    }
                    focusedChild.setFocusedElementByIndex(nextIndex);
                    focusedChild.setActiveFocus();
                    handled = true;
                }
                return handled;
            };
            CViewGroupControl.prototype._doKeyUpLatent = function (aKeyCode) {
                //return this._fnScrolling(KKeyStrUp);
                return this._fnScrolling2(KKeyStrUp);
            };
            CViewGroupControl.prototype._doKeyDownLatent = function (aKeyCode) {
                //return this._fnScrolling(KKeyStrDown);
                return this._fnScrolling2(KKeyStrDown);
            };
            CViewGroupControl.prototype._doKeyChannelUpLatent = function (aKeyCode) {
                //return this._fnScrolling(KKeyStrUp, this.getChannelUpIndex());
                return this._fnScrolling2(KKeyStrChannelUp);
            };
            CViewGroupControl.prototype._doKeyChannelDownLatent = function (aKeyCode) {
                //return this._fnScrolling(KKeyStrDown, this.getChannelDownIndex());
                return this._fnScrolling2(KKeyStrChannelDown);
            };
            CViewGroupControl.prototype._doKeyLeftLatent = function (aKeyCode) {
                return this._fnScrolling2(KKeyStrLeft);
            };
            CViewGroupControl.prototype._doKeyRightLatent = function (aKeyCode) {
                return this._fnScrolling2(KKeyStrRight);
            };
            return CViewGroupControl;
        })(CGroupControl);
        CtrlBase.CViewGroupControl = CViewGroupControl;
        var CAbsCommonDataOperationControl = (function (_super) {
            __extends(CAbsCommonDataOperationControl, _super);
            function CAbsCommonDataOperationControl(aElement) {
                _super.call(this, aElement);
                this.setupDataControl();
                this._element.appendChild(this._dataControl._element);
                this.setOwnedChildControls([this._dataControl]);
            }
            CAbsCommonDataOperationControl.prototype.setupDataControl = function () {
                this.doSetupDataControl();
                this._dataControl.connectKeyMapUpdated(this, "_slKeyMapUpdated", this._slKeyMapUpdated);
            };
            CAbsCommonDataOperationControl.prototype.doSetupDataControl = function () {
            };
            // setting
            CAbsCommonDataOperationControl.prototype.setDataRolling = function (aRolling) {
                this._dataControl.setDataRolling(aRolling);
            };
            CAbsCommonDataOperationControl.prototype.getDataRolling = function () {
                return this._dataControl.getDataRolling();
            };
            CAbsCommonDataOperationControl.prototype.setItemHeight = function (aItemHeight) {
                this._dataControl.setItemHeight(aItemHeight);
            };
            CAbsCommonDataOperationControl.prototype.setItemWidth = function (aItemWidth) {
                this._dataControl.setItemWidth(aItemWidth);
            };
            CAbsCommonDataOperationControl.prototype.setOrientation = function (aLayout) {
                this._dataControl._setDrawParam(KParamStrOrientation, aLayout, false);
            };
            CAbsCommonDataOperationControl.prototype.setOwnedDataProvider = function (aDataProvider) {
                this._dataControl.setOwnedDataProvider(aDataProvider);
                aDataProvider.connectItemInserted(this, "_slItemInserted", this._slItemInserted);
                aDataProvider.connectItemRemoved(this, "_slItemRemoved", this._slItemRemoved);
                aDataProvider.connectItemUpdated(this, "_slItemUpdated", this._slItemUpdated);
            };
            CAbsCommonDataOperationControl.prototype.setDataDrawer = function (aDrawer) {
                this._dataControl.setDataDrawer(aDrawer);
            };
            CAbsCommonDataOperationControl.prototype.setListData = function (aData, aDataRolling) {
                this.setDataRolling(aDataRolling);
                this.setOwnedDataProvider(new CListDataProvider(aData, aDataRolling));
            };
            // event connection API
            CAbsCommonDataOperationControl.prototype.connectFocusChanged = function (aHolder, aSlotName, aSlot) {
                this._dataControl.connectFocusChanged(aHolder, aSlotName, aSlot);
            };
            CAbsCommonDataOperationControl.prototype.connectFocusGained = function (aHolder, aSlotName, aSlot) {
                this._dataControl.connectFocusGained(aHolder, aSlotName, aSlot);
            };
            CAbsCommonDataOperationControl.prototype.connectFocusLost = function (aHolder, aSlotName, aSlot) {
                this._dataControl.connectFocusLost(aHolder, aSlotName, aSlot);
            };
            CAbsCommonDataOperationControl.prototype.connectFocusedDataItemChanged = function (aHolder, aSlotName, aSlot) {
                this._dataControl.connectFocusedDataItemChanged(aHolder, aSlotName, aSlot);
            };
            CAbsCommonDataOperationControl.prototype.connectDataItemSelected = function (aHolder, aSlotName, aSlot) {
                this._dataControl.connectDataItemSelected(aHolder, aSlotName, aSlot);
            };
            CAbsCommonDataOperationControl.prototype.connectItemSelected = function (aHolder, aSlotName, aSlot) {
                this._dataControl.connectItemSelected(aHolder, aSlotName, aSlot);
            };
            CAbsCommonDataOperationControl.prototype.connectDataItemOrderChanged = function (aHolder, aSlotName, aSlot) {
                this._dataControl.connectDataItemOrderChanged(aHolder, aSlotName, aSlot);
            };
            // data control API
            CAbsCommonDataOperationControl.prototype.prependItems = function (aItems) {
                this._dataControl.insertItems(0, aItems);
            };
            CAbsCommonDataOperationControl.prototype.insertItems = function (aPosition, aItems) {
                this._dataControl.insertItems(aPosition, aItems);
            };
            CAbsCommonDataOperationControl.prototype.appendItems = function (aItems) {
                this._dataControl.appendItems(aItems);
            };
            CAbsCommonDataOperationControl.prototype.removeItems = function (aIndex) {
                this._dataControl.removeItems(aIndex);
            };
            CAbsCommonDataOperationControl.prototype.updateItems = function (aKeys, aItems) {
                this._dataControl.updateItems(aKeys, aItems);
            };
            CAbsCommonDataOperationControl.prototype.getItemLength = function () {
                return this._dataControl.getItemLength();
            };
            CAbsCommonDataOperationControl.prototype.getItemHeight = function () {
                return this._dataControl.getItemHeight();
            };
            CAbsCommonDataOperationControl.prototype.getCountOnPage = function () {
                //TODO: consider horizontal or vertical, currently support vertical scheme only.
                return this._element.offsetHeight / this.getItemHeight();
            };
            CAbsCommonDataOperationControl.prototype.getCheckedItems = function () {
                return this._dataControl.getCheckedItems();
            };
            CAbsCommonDataOperationControl.prototype.deleteCheckedItem = function () {
                return this._dataControl.deleteCheckedItem();
            };
            CAbsCommonDataOperationControl.prototype.setItemCheck = function (aIndex) {
                this._dataControl.setItemCheck(aIndex);
                this.update();
            };
            CAbsCommonDataOperationControl.prototype.setItemUnCheck = function (aIndex) {
                this._dataControl.setItemUnCheck(aIndex);
                //XXXX
                this.update();
            };
            CAbsCommonDataOperationControl.prototype.setItemCheckClear = function () {
                this._dataControl.setItemCheckClear();
                this.update();
            };
            CAbsCommonDataOperationControl.prototype.isItemCheck = function (aIndex) {
                this._dataControl.isItemCheck(aIndex);
            };
            // event handler
            CAbsCommonDataOperationControl.prototype._slItemInserted = function (drawnElements, aNeedFocus) {
                this.update();
            };
            CAbsCommonDataOperationControl.prototype._slItemRemoved = function (drawnElements, aUnsetFocus) {
                this.update();
            };
            CAbsCommonDataOperationControl.prototype._slItemUpdated = function (aKeys, aItems) {
                this.update();
            };
            CAbsCommonDataOperationControl.prototype.getFocusedItemInfo = function () {
                return this._dataControl.getFocusedItemInfo();
            };
            CAbsCommonDataOperationControl.prototype.getFocusedElementIndex = function () {
                return this._dataControl.getFocusedElementIndex();
            };
            CAbsCommonDataOperationControl.prototype.getFocusedElement = function () {
                return this._dataControl.getFocusedElement();
            };
            /* unsed
            setFocusedItem(aKey: any) {
                this._dataControl.setFocusedItem(aKey);
            }
            setRedrawAfterOperation(aRedraw: boolean) {
                this._dataControl.setRedrawAfterOperation(aRedraw);
            }
            */
            CAbsCommonDataOperationControl.prototype.isMovable = function (aKeyStr) {
                //TODO: this function need additional implementation, consider scroll scheme & data rolling
                var focusedItem = this.getFocusedElement();
                var ret = false;
                if (focusedItem) {
                    var onePage = this.getCountOnPage();
                    var count = this.getItemLength();
                    var pos = focusedItem.attributes['data'];
                    var scheme = this.getVerticalScrollScheme();
                    var container = this._container;
                    if (scheme === 3 /* EByFixedFocusRemains */) {
                        if (aKeyStr === KKeyStrUp) {
                            ret = (pos !== 0);
                        }
                        else if (aKeyStr === KKeyStrDown) {
                            ret = (pos !== count - 1);
                        }
                        else if (aKeyStr === KKeyStrChannelUp) {
                            ret = (pos >= onePage - 1 && container.offsetTop !== 0);
                        }
                        else if (aKeyStr === KKeyStrChannelDown) {
                            if (count < onePage) {
                                ret = false;
                            }
                            else {
                                var heightScroll = this.getItemHeight() * (onePage - 1);
                                var heightList = this._dataControl.getSize().height;
                                var endOfList = Math.floor(heightList / heightScroll) * heightScroll;
                                if (endOfList === heightList) {
                                    endOfList -= heightScroll;
                                }
                                ret = (Math.abs(container.offsetTop) !== endOfList);
                            }
                        }
                    }
                    else {
                        throw 'do implement for control other scroll scheme';
                    }
                }
                return ret;
            };
            CAbsCommonDataOperationControl.prototype._slKeyMapUpdated = function (aKeyMap) {
                throw "do implement case 'this._keyMap === null', occurred by remove operation";
            };
            return CAbsCommonDataOperationControl;
        })(CViewGroupControl);
        CtrlBase.CAbsCommonDataOperationControl = CAbsCommonDataOperationControl;
        var CListControl = (function (_super) {
            __extends(CListControl, _super);
            function CListControl(aElement) {
                _super.call(this, aElement);
                this.name = "CListControl";
                this.setListData([]);
            }
            CListControl.prototype.setAnimation = function (aAni) {
                _super.prototype.setAnimation.call(this, aAni);
                this._dataControl.setAnimation(aAni);
                this.connectTransitioningChanged(this, '_slTransitioning', this._slTransitioning);
            };
            CListControl.prototype._slTransitioning = function (aTransitioning) {
                if (!aTransitioning) {
                    this._dataControl.endTransition();
                }
            };
            CListControl.prototype.doSetupDataControl = function () {
                this._dataControl = new CListDataControl(document.createElement('div'));
            };
            CListControl.prototype._slKeyMapUpdated = function (aKeyMap) {
                var _this = this;
                if (aKeyMap === null) {
                    var scheme = this.getVerticalScrollScheme();
                    var topContainer = this._container.style.top;
                    if (scheme === 3 /* EByFixedFocusRemains */) {
                        if (topContainer && parseInt(topContainer) !== 0) {
                            setTimeout(function () {
                                if (!_this.isTransitioning()) {
                                    _this.moveDrawPosition({
                                        top: (_this.getCountOnPage() - 1) * _this.getItemHeight(),
                                        left: 0
                                    });
                                    var focusedChild = _this.getFocusedChild();
                                    var keyMap = focusedChild._keyMap;
                                    if (keyMap) {
                                        var index = keyMap.getMapCount() - 1 || 0;
                                        focusedChild.setActiveFocus();
                                        focusedChild.setFocusedElementByIndex(index);
                                    }
                                    _this.setActiveFocus();
                                }
                            }, 300);
                        }
                    }
                }
            };
            return CListControl;
        })(CAbsCommonDataOperationControl);
        CtrlBase.CListControl = CListControl;
        var CGridControl = (function (_super) {
            __extends(CGridControl, _super);
            function CGridControl(aElement) {
                _super.call(this, aElement);
                this.name = "CGridControl";
                this.setListData([]);
            }
            CGridControl.prototype.doSetupDataControl = function () {
                this._dataControl = new CGridDataControl(document.createElement('div'));
            };
            CGridControl.prototype.setStartIndex = function (aIndex) {
                this._dataControl.setStartIndex(aIndex);
            };
            CGridControl.prototype.setMaxRowCount = function (aMax) {
                this._dataControl.setMaxRowCount(aMax);
            };
            CGridControl.prototype.setMaxColCount = function (aMax) {
                this._dataControl.setMaxColCount(aMax);
            };
            CGridControl.prototype.setAnimation = function (aAni) {
                _super.prototype.setAnimation.call(this, aAni);
                this._dataControl.setAnimation(aAni);
                this.connectTransitioningChanged(this, '_slTransitioning', this._slTransitioning);
            };
            CGridControl.prototype.setDrawHint = function (aHint) {
                this._dataControl.setDrawnHint(aHint);
            };
            CGridControl.prototype.getRowElemnt = function (aRow, bDuplicate) {
                return this._dataControl.getRowElement(aRow, bDuplicate);
            };
            CGridControl.prototype.getColElemnt = function (aCol, bDuplicate) {
                return this._dataControl.getColElement(aCol, bDuplicate);
            };
            CGridControl.prototype._slTransitioning = function (aTransitioning) {
                if (!aTransitioning) {
                    this._dataControl.endTransition();
                }
            };
            CGridControl.prototype._slKeyMapUpdated = function (aKeyMap) {
                //TODO
            };
            CGridControl.prototype._doKeyDownLatent = function (aKeyCode) {
                var handled = _super.prototype._doKeyDownLatent.call(this, aKeyCode);
                if (!handled) {
                    var maxCol = this._dataControl._getDrawParam(KParamStrMaxColCount);
                    var focusedIndex = this.getFocusedElementIndex();
                    if (this._dataControl._drawerHint) {
                        var itemCount = this._dataControl._ownedDataProvider.getLength();
                        var focusedItemInfo = this._dataControl.getPositionFromHintArray(focusedIndex);
                        var lastItemInfo = this._dataControl.getPositionFromHintArray(itemCount - 1);
                        if (focusedItemInfo.row < lastItemInfo.row) {
                            handled = this._dataControl.setFocusedElementByIndex(itemCount - 1);
                        }
                    }
                    else {
                        var focusableCount = this._dataControl.getFocusableElementCount();
                        var currentRow = Math.ceil((focusedIndex + 1) / maxCol);
                        var lastRow = Math.ceil(focusableCount / maxCol);
                        if (currentRow < lastRow) {
                            handled = this._dataControl.setFocusedElementByIndex(focusableCount - 1);
                        }
                    }
                }
                return handled;
            };
            CGridControl.prototype._doKeyRightLatent = function (aKeyCode) {
                var handled = _super.prototype._doKeyRightLatent.call(this, aKeyCode);
                if (!handled) {
                    var maxCol = this._dataControl._getDrawParam(KParamStrMaxColCount);
                    var focusedIndex = this.getFocusedElementIndex();
                    if (this._dataControl._drawerHint) {
                        var itemCount = this._dataControl._ownedDataProvider.getLength();
                        var focusedItemInfo = this._dataControl.getPositionFromHintArray(focusedIndex);
                        var lastItemInfo = this._dataControl.getPositionFromHintArray(itemCount - 1);
                        if (focusedItemInfo.row === lastItemInfo.row) {
                            var nextIndex = this._dataControl._keyMapHint[focusedItemInfo.row - 1][focusedItemInfo.col + 1];
                            handled = this._dataControl.setFocusedElementByIndex(nextIndex);
                        }
                    }
                    else {
                        var focusableCount = this._dataControl.getFocusableElementCount();
                        var currentRow = Math.ceil((focusedIndex + 1) / maxCol);
                        var lastRow = Math.ceil(focusableCount / maxCol);
                        if ((currentRow === lastRow) && ((focusedIndex + 1) % maxCol !== 0)) {
                            handled = this._dataControl.setFocusedElementByIndex(focusedIndex - maxCol + 1);
                        }
                    }
                }
                return handled;
            };
            return CGridControl;
        })(CAbsCommonDataOperationControl);
        CtrlBase.CGridControl = CGridControl;
        var CMediaListDataProvider = (function (_super) {
            __extends(CMediaListDataProvider, _super);
            function CMediaListDataProvider(aArray, aRolling) {
                _super.call(this, aArray, aRolling);
                this._arrayFile = [];
                this._fileMap = {};
                this._fnPushFileToArray(aArray);
            }
            CMediaListDataProvider.prototype._fnPushFileToArray = function (aArray) {
                for (var i = 0; i < aArray.length; i++) {
                    if (aArray[i].fileType === 0) {
                        this._arrayFile.push(aArray[i]);
                        this._fileMap[i] = this._arrayFile.length - 1;
                    }
                }
            };
            CMediaListDataProvider.prototype._fnRemoveFileToArray = function (aKeys) {
                var i, len, realKey, listDel = [], map = this._fileMap;
                for (i = 0, len = aKeys.length; i < len; i++) {
                    realKey = map[aKeys[i]];
                    if (typeof realKey === 'number') {
                        listDel.push(realKey);
                    }
                }
                listDel.sort(function (a, b) {
                    return a < b ? -1 : a > b ? 1 : 0;
                });
                for (i = listDel.length - 1; i >= 0; i--) {
                    this._arrayFile.splice(listDel[i], 1);
                }
            };
            CMediaListDataProvider.prototype.insertItems = function (aKey, aItems) {
                this._fnPushFileToArray(aItems);
                _super.prototype.insertItems.call(this, aKey, aItems);
            };
            CMediaListDataProvider.prototype.removeItems = function (aKeys) {
                this._fnRemoveFileToArray(aKeys);
                _super.prototype.removeItems.call(this, aKeys);
            };
            CMediaListDataProvider.prototype.getContents = function (aContentType) {
                return this._arrayFile;
            };
            return CMediaListDataProvider;
        })(CListDataProvider);
        CtrlBase.CMediaListDataProvider = CMediaListDataProvider;
        var CMediaMediaShareDataProvider = (function (_super) {
            __extends(CMediaMediaShareDataProvider, _super);
            function CMediaMediaShareDataProvider() {
                _super.apply(this, arguments);
                this._contents = {
                    'video': [],
                    'photo': [],
                    'music': []
                };
            }
            CMediaMediaShareDataProvider.prototype._fnPushFileToArray = function (aArray) {
                var item, i;
                var type;
                for (i = 0; i < aArray.length; i++) {
                    item = aArray[i];
                    type = item.objectType;
                    if (CMediaMediaShareDataProvider.KTypeObject.indexOf(type) !== -1) {
                        if (type === 'video' || type === 'music') {
                            //                        item.importURI = item.importURI.replace(CMediaMediaShareDataProvider.KURIPrifix, CMediaMediaShareDataProvider.KDlnaPrifix);
                            item.importURI = CMediaMediaShareDataProvider.KDlnaPrifix + item.importURI;
                            console.log('CMediaMediaShareDataProvider: item.importURI: ' + item.importURI);
                        }
                        this._contents[item.objectType].push(item);
                    }
                }
            };
            CMediaMediaShareDataProvider.prototype.insertItems = function (aKey, aItems) {
                this._fnPushFileToArray(aItems);
                _super.prototype.insertItems.call(this, aKey, aItems);
            };
            CMediaMediaShareDataProvider.prototype.getContents = function (aContentType) {
                if (CMediaMediaShareDataProvider.KTypeObject.indexOf(aContentType) === -1) {
                    return null;
                }
                return this._contents[aContentType];
            };
            CMediaMediaShareDataProvider.KTypeObject = ['video', 'photo', 'music'];
            CMediaMediaShareDataProvider.KDlnaPrifix = 'hdlna://';
            CMediaMediaShareDataProvider.KURIPrifix = 'http://';
            return CMediaMediaShareDataProvider;
        })(CListDataProvider);
        CtrlBase.CMediaMediaShareDataProvider = CMediaMediaShareDataProvider;
        var CMediaListDataControl = (function (_super) {
            __extends(CMediaListDataControl, _super);
            function CMediaListDataControl(aElement, aRolling) {
                _super.call(this, aElement, aRolling);
            }
            CMediaListDataControl.prototype.getContents = function (aContentType) {
                return this._ownedDataProvider.getContents(aContentType);
            };
            return CMediaListDataControl;
        })(CListDataControl);
        CtrlBase.CMediaListDataControl = CMediaListDataControl;
        var CMediaListControl = (function (_super) {
            __extends(CMediaListControl, _super);
            function CMediaListControl(aElement) {
                _super.call(this, aElement);
                this.setAnimation(true);
            }
            CMediaListControl.prototype.doSetupDataControl = function () {
                this._dataControl = new CMediaListDataControl(document.createElement('div'));
            };
            CMediaListControl.prototype.setListData = function (aData, aDataRolling) {
                this.setDataRolling(aDataRolling);
                this.setOwnedDataProvider(new CMediaListDataProvider(aData, aDataRolling));
            };
            CMediaListControl.prototype.getContents = function (aContentType) {
                return this._dataControl.getContents(aContentType);
            };
            return CMediaListControl;
        })(CListControl);
        CtrlBase.CMediaListControl = CMediaListControl;
        var CShowcaseTileCarousel = (function (_super) {
            __extends(CShowcaseTileCarousel, _super);
            function CShowcaseTileCarousel(aElement, aData) {
                _super.call(this, $(aElement));
                this._title = 'discover showcase';
                this._description = 'Browse top content recommended by Freesat editors.';
                this._data = aData;
                this._index = 0;
                this._element.classList.add('-showcase-tile');
            }
            CShowcaseTileCarousel.prototype._doDraw = function () {
                var ret;
                var elTitle = document.createElement('p');
                elTitle.classList.add('-title');
                elTitle.innerHTML = this._title;
                var elDescription = document.createElement('p');
                elDescription.classList.add('-description');
                //            elDescription.innerHTML = this._description;
                this._elDescription = elDescription;
                var elContent = document.createElement('div');
                elContent.classList.add('-content');
                this._elContent = elContent;
                this._drawContent(elContent);
                this._element.appendChild(elTitle);
                this._element.appendChild(elContent);
                this._element.appendChild(elDescription);
                return ret;
            };
            CShowcaseTileCarousel.prototype._drawContent = function (aContainer) {
                aContainer.innerHTML = '';
                var item = this._data[this._index];
                var eThumbnail = document.createElement('div');
                eThumbnail.classList.add('-thumbnail');
                var elThumbnailImg = document.createElement('img');
                elThumbnailImg.classList.add('-img');
                elThumbnailImg.src = item.thumbnail_image;
                elThumbnailImg.alt = '';
                eThumbnail.appendChild(elThumbnailImg);
                var eBorder = document.createElement('div');
                eBorder.classList.add('-border');
                var eLogo = document.createElement('div');
                eLogo.classList.add('-logo');
                var eLogoImg = document.createElement('img');
                eLogoImg.src = item.channel_logo;
                eLogo.appendChild(eLogoImg);
                //            var eTitle = document.createElement('p');
                //            eTitle.classList.add('-content-title');
                //            eTitle.innerHTML = item.title;
                var eSubtitle = document.createElement('p');
                eSubtitle.classList.add('-subtitle');
                eSubtitle.innerHTML = item.subtitle;
                if (item.ctaText) {
                    this._elDescription.innerHTML = item.ctaText;
                }
                else {
                    this._elDescription.innerHTML = this._description;
                }
                aContainer.appendChild(eThumbnail);
                aContainer.appendChild(eBorder);
                aContainer.appendChild(eLogo);
                //            aContainer.appendChild(eTitle);
                aContainer.appendChild(eSubtitle);
                this._index++;
            };
            CShowcaseTileCarousel.prototype._doKeyOk = function () {
                this.emit.call(this, "ItemSelected", this, this._index, null);
                return true;
            };
            return CShowcaseTileCarousel;
        })(CControl);
        CtrlBase.CShowcaseTileCarousel = CShowcaseTileCarousel;
        console.log("CtrlBase End");
    })(CtrlBase || (CtrlBase = {}));
    return CtrlBase;
});
