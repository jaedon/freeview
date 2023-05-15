/// <reference path="../../def/jquery.d.ts"/>
/// <reference path="../../def/require.d.ts"/>
/// <reference path="common.ts"/>

declare var hx;
module CtrlBase {
    console.log("CtrlBase Start");
    class CCircularArray {

        _array;
        _idxCur;
        _idxLast;

        constructor(array?) {
            var a = this;
            if (array) {
                a.setArray(array);
            }
        }
        destroy() {
            delete this._array;
            delete this._idxCur;
            delete this._idxLast;
        }
        getArray() {
            var a = this;
            return a._array;
        }
        setArray(array) {
            var a = this;
            a._array = array;
            a._idxCur = 0;
            a._idxLast = array.length - 1;
        }
        appendArray(array) {
            var a = this;
            a._array = a._array.concat(array);
            a._idxLast = array.length - 1;
        }
        cur(): number {
            var a = this;
            return a._idxCur;
        }
        curItem() {
            var a = this;
            return a._array[a._idxCur];
        }
        setCur(index) {
            var a = this;
            a._idxCur = a.indexing(index);
        }
        inc(count?) {
            var a = this;
            count = typeof count != 'undefined' ? count : 1;
            a._idxCur = a.indexing(a._idxCur + count);
        }
        dec(count?) {
            var a = this;
            count = typeof count != 'undefined' ? count : 1;
            a._idxCur = a.indexing(a._idxCur - count);
        }
        indexing(index) {
            var ret;
            var length = this._array.length;
            if (index >= 0) {
                ret = index % length;
            } else {
                ret = length + index % length;
                if (ret === length) {
                    ret = 0;
                }
            }
            return ret;
        }
        at(index) {
            return this._array[this.indexing(index)];
        }
        get(count, offset) {
            var a = this;
            var CResult = function () {
            };
            CResult.prototype = new Array();
            CResult.prototype.firstAvailIndex = function () {
                var i, a = this;
                for (i = 0; i < a.length; i += 1) {
                    if (a[i]) return i;
                }
                return -1;
            };
            CResult.prototype.lastAvailIndex = function () {
                var i, a = this;
                for (i = a.length - 1; i >= 0; i -= 1) {
                    if (a[i]) return i;
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
                } else if (len - 1 <= offset) { // has enough space in upper index.
                    var start = offset - len - 1;
                    for (i = 0; i < len; i += 1) {
                        ret[i + start] = a.at(a._idxCur + i + 1);
                    }
                } else if (len <= count - offset) { // has enough space in lower index.
                    var start = <number>offset;
                    for (i = 0; i < len; i += 1) {
                        ret[i + start] = a.at(a._idxCur + i);
                    }
                } else {
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
        }
        del(index) {
            var a = this;
            var compromised = a.indexing(index);
            if (isNaN(compromised)) {
                return;
            }
            a._array.splice(compromised, 1);
        }

        each(fn, offset?) {
            var a = this;
            var index = offset ? a.indexing(a._idxCur + offset) : a._idxCur;
            var need = false;
            do {
                need = fn(index, a.at(index));
                index = a.indexing(index + 1);
            } while (need && index != a._idxCur);
        }

        eachReverse(fn) {
            var a = this;
            var index = a._idxCur;
            var need = false;
            do {
                need = fn(index, a.at(index));
                index = a.indexing(index - 1);
            } while (need && index != a._idxCur);
        }

        eachArray(fn) {
            var a = this;
            var index = a._idxCur;
            var need = false;
            do {
                if (index < a._array.length) {
                    need = fn(index, a.at(index));
                } else {
                    need = fn(-1, undefined);
                }
                index++;
            } while (need);
        }
        length() {
            var a = this;
            var ret = 0;
            if (a._array) {
                ret = a._array.length;
            }
            return ret;
        }
        clone() {
            var a = this;
            var ret = new CCircularArray(a._array);
            ret.setCur(a._idxCur);
            return ret;
        }
    }

    var KCssPropTransition: string = 'transition';
    var KCssEvntTransitionEnd: string = 'transitionend';
    var KCssTransitionDuration: string = 'transition-duration';

    var browser = (function () {
        var N = navigator.appName, ua = navigator.userAgent, tem;
        var M = ua.match(/(opera|chrome|safari|firefox|msie)\/?\s*(\.?\d+(\.\d+)*)/i);
        if (M && (tem = ua.match(/version\/([\.\d]+)/i)) != null) M[2] = tem[1];
        M = M ? [M[1], M[2]] : [N, navigator.appVersion, '-?'];
        return M;
    } ());

    if (browser[0] == "chrome") {
        KCssPropTransition = '-webkit-transition';
        KCssEvntTransitionEnd = 'webkitTransitionEnd';
        KCssTransitionDuration = '-webkit-transition-duration';
    } else if (browser[0] == "opera") {
        KCssPropTransition = '-o-transition';
        KCssEvntTransitionEnd = 'oTransitionEnd otransitionend';
        KCssTransitionDuration = '-o-transition-duration';
    } else if (browser[0] == "msie") {
        KCssPropTransition = '-ms-transition';
        KCssEvntTransitionEnd = 'msTransitionEnd mstransitionend';
        KCssTransitionDuration = '-ms-transition-duration';
    }

    var KCssTransitionParamPos = 'top .5s linear, left .5s linear';
    var KCssTransitionParamOpa = 'opacity .3s linear';
    var KCssTransitionParamSize = 'width .5s linear, height .5s linear';

    export var TCssStr = {
        KCssPropTransition: KCssPropTransition,
        KCssEvntTransitionEnd: KCssEvntTransitionEnd,
        KCssTransitionDuration: KCssTransitionDuration,
        KCssTransitionParamPos: KCssTransitionParamPos,
        KCssTransitionParamOpa: KCssTransitionParamOpa,
        KCssTransitionParamSize: KCssTransitionParamSize
    };

    export class Util {
        static afterTransition(aElement: HTMLElement, aCallBack: Function, aWithEvent?: boolean) {
            var durations = ['0'];
            if (getComputedStyle && getComputedStyle(aElement).transitionDuration) {
                durations = getComputedStyle(aElement).transitionDuration.split(',');
            } else {
                durations = aElement.style.transitionDuration.split(',');
            }
            var delay = parseFloat(durations[0].replace('s', '')) * 1000 || 300;
            if(delay) {
                if (aWithEvent) {
                    var bRunCallback = false;
                    var timeout;
                    var transitionEndedHandler: (e) => void;
                    timeout = setTimeout(function() {
                        aCallBack();
                        bRunCallback = true;
                    }, delay * 2);
                    aElement.addEventListener(KCssEvntTransitionEnd,
                        transitionEndedHandler = function(e) {
                            if(!bRunCallback) {
                                aCallBack();
                            }
                            clearTimeout(timeout);
                            aElement.removeEventListener(KCssEvntTransitionEnd, transitionEndedHandler);
                        });
                } else {
                    setTimeout(aCallBack, delay);
                }
            } else {
                throw "Invalid transiton duration";
            }
        }

        static remove(aElement: HTMLElement) {
            var parent = aElement.parentElement;
            if (parent) {
                parent.removeChild(aElement);
            }
        }

        static prepend(aElement: HTMLElement, aNewChild: HTMLElement) {
            aElement.insertBefore(aNewChild, aElement.firstElementChild);
        }

        static getRect(aElement: HTMLElement): TRect {
            return new TRect({
                top: aElement.offsetTop,
                left: aElement.offsetLeft,
                right: aElement.offsetLeft + aElement.offsetWidth,
                bottom: aElement.offsetTop + aElement.offsetHeight,
            });
        }
    }

    export class TRect {
        top: number = 0;
        left: number = 0;
        right: number = 0;
        bottom: number = 0;
        centerX: number;
        centerY: number;
        constructor(aParam?: { top: number; left: number; right: number; bottom: number; }) {
            if (aParam) {
                this.top = aParam.top;
                this.left = aParam.left;
                this.right = aParam.right;
                this.bottom = aParam.bottom;
            }
        }
        getHeight(): number {
            return this.bottom - this.top;
        }
        getWidth(): number {
            return this.right - this.left;
        }
        getCenterX(): number {
            return this.left + Math.floor(this.getWidth() / 2);
        }
        getCenterY(): number {
            return this.top + Math.floor(this.getHeight() / 2);
        }
        hasSize(): boolean {
            return this.getHeight() > 0 || this.getWidth() > 0;
        }
        setHeight(aHeight: number) {
            this.bottom = this.top + aHeight;
        }
        setWidth(aWidth: number) {
            this.right = this.left + aWidth;
        }
        moveTop(aIncrement: number) {
            this.top += aIncrement;
            this.bottom += aIncrement;
        }
        moveLeft(aIncrement: number) {
            this.left += aIncrement;
            this.right += aIncrement;
        }
        getIntersectedRect(aRect: TRect): TRect {
            var ret: TRect = new TRect;
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
        }
    }

    var KClassFocusable = "-f";
    var KClassFocusDisable = "-f-d";
    var KClassFocused = "-fd";
    var KClassActiveFocused = "-afd";
    var KClassActiveFocusedLeaf = "-afd-leaf";

    export var TClassStr = {
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

    export var TKeycode = {
        KKeyUp : 38,
        KKeyDown : 40,
        KKeyLeft : 37,
        KKeyRight : 39,
        KKeyEnter : 13,
        KKeyChannelUp : 427,
        KKeyChannelDown : 428,
        KKeyBack : 8,
        KKeyEscape : 27
    };

    var keyStrTable = <any>{}; // Workaround: http://stackoverflow.com/questions/14994406/typescript-using-number-as-key-in-map
    keyStrTable[KKeyUp] = KKeyStrUp;
    keyStrTable[KKeyDown] = KKeyStrDown;
    keyStrTable[KKeyLeft] = KKeyStrLeft;
    keyStrTable[KKeyRight] = KKeyStrRight;
    keyStrTable[KKeyEnter] = KKeyStrEnter;
    keyStrTable[KKeyChannelUp] = KKeyStrChannelUp;
    keyStrTable[KKeyChannelDown] = KKeyStrChannelUp;
    keyStrTable[KKeyBack] = KKeyStrBack;
    keyStrTable[KKeyEscape] = KKeyStrEscape;

    export interface TKeyMapItem {
        el: HTMLElement;
        l?: number;
        r?: number;
        u?: number;
        d?: number;
    }

    export interface FFocusChanged {
        (aOld: HTMLElement, aNew: HTMLElement): void;
    }
    export interface FChildFocusChanged {
        (aOld: CControl, aNew: CControl): void;
    }
    export interface FFocusGained {
        (aControl: CControl): void;
    }
    export interface FFocusLost {
        (aControl: CControl): void;
    }
    //TODO: need to confirm below functions used by listcontrol
    export interface FItemSelected {
        (aControl: CControl, aIndex: number, aEl: HTMLElement): void;
    }
    export interface FItemInserted {
        (aDrawnElements: CDrawnElements, aNeedFocus?: boolean): void;
    }
    export interface FItemRemoved {
        (aDrawnElements: CDrawnElements, aUnsetFocus?: boolean): void;
    }

    enum eModeKeyMap {
        NORMAL,
        MOVE,
    }

    export class CKeyMap {
        private _index: number = -1;
        private _map: TKeyMapItem[] = [];
        private _focusChanged: FFocusChanged;
        private _activeFocusClass: string;
        private _mode: eModeKeyMap = eModeKeyMap.NORMAL;

        constructor(aFocusChanged: FFocusChanged, aActiveFocusClass: string) {
            this._focusChanged = aFocusChanged;
            this._activeFocusClass = aActiveFocusClass;
        }
        destroy() {
            this._map = null;
        }
        addMapItem(aMapItem: TKeyMapItem) {
            this._map.push(aMapItem);
        }
        getMapItem(aIndex: number) {
            return this._map[aIndex];
        }
        setActiveFocus(aIndex: number) {
            this._index = aIndex;
        }
        getFocusedElement(): HTMLElement {
            if (this._map && this._map.length) {
                return this._map[this._index].el;
            }
            return null;
        }
        getFocusedIndex(): number {
            return this._index;
        }
        getMapCount(): number {
            return this._map.length;
        }
        getIndex(aElement: HTMLElement): number {
            var index = -1;
            var i, len, item: TKeyMapItem;
            for (i = 0, len = this._map.length; i < len; i++) {
                item = this._map[i];
                if (item.el == aElement) {
                    index = i;
                    break;
                }
            }
            return index;
        }
        changeFocus(aNewIndex: number) {
            var oldMapItem: TKeyMapItem = this._map[this._index];
            var newMapItem: TKeyMapItem = this._map[aNewIndex];
            var oldEl: HTMLElement = oldMapItem.el;
            var newEl: HTMLElement = newMapItem.el;
            oldEl.classList.remove(KClassFocused);
            newEl.classList.add(KClassFocused);
            if (oldEl.classList.contains(this._activeFocusClass)) {
                oldEl.classList.remove(this._activeFocusClass);
                newEl.classList.add(this._activeFocusClass);
            }
            this._index = aNewIndex;
            this._focusChanged(oldEl, newEl);
        }
        doKey(aKeyStr: string): boolean {
            var handlers: { [keyStr: string]: Function; } = {};
            var oldMapItem: TKeyMapItem = this._map[this._index];
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
                    if (this._mode === eModeKeyMap.MOVE) {
                        this.swap(newIndex);
                    } else {
                        this.changeFocus(newIndex);
                    }
                    return true;
                }
            }
            return false;
        }
        setMode(aMode: eModeKeyMap) {
            var dirty = this._mode !== aMode;
            this._mode = aMode;
            return dirty;
        }
        swap(aNewIndex: number) {
            var oldMapItem: TKeyMapItem = this._map[this._index];
            var newMapItem: TKeyMapItem = this._map[aNewIndex];
            var tempEl = newMapItem.el;
            var tempPos = newMapItem.el.style.left;
            newMapItem.el.style.left = oldMapItem.el.style.left;
            oldMapItem.el.style.left = tempPos;
            newMapItem.el = oldMapItem.el;
            oldMapItem.el = tempEl;
            this._index = aNewIndex;
        }
        isMovable(aKeyStr: string): boolean {
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
        }
        getNextMovableIndex(aKeyStr: string): number {
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
        }
        getNextMovableElement(aKeyStr: string): HTMLElement {
            var index = this.getNextMovableIndex(aKeyStr);

            if (index != -1) {
                return this._map[index].el;
            }
            return null;
        }
        setMovableItem(aIndex: number, aDataIndex: number, aMaxRow: number, aDirection: string): number {
            var row = aDataIndex % aMaxRow;
            var col = Math.floor(aDataIndex / aMaxRow);
            var item;
            var i;
            var ret = -1;
            var el: HTMLElement;
            var listCandidate = [];
            var cloneDataIndex = aDataIndex;
            if (aDirection === "l") {
                while (cloneDataIndex >= aMaxRow) {
                    listCandidate.push(cloneDataIndex -= aMaxRow);
                }
            } else {
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
                        } else {
                            this._map[i].d = aIndex;
                        }
                        ret = i;
                        break;
                    }
                }
            }
            return ret;
        }
    }

    export interface TPrevFocusInfo {
        rect: TRect;
        activeFocus: boolean;
    }
    export interface FKeyMapBuilder {
        (aKeyMap: CKeyMap, aFocusable: HTMLElement[], aPrevFocusInfo?: TPrevFocusInfo, aPrevKeyStr?: string, aHint?: any): void;
    }

    export var KBuilderTopDown: FKeyMapBuilder = function (
        aKeyMap: CKeyMap,
        aFocusable: HTMLElement[],
        aPrevFocusInfo?: TPrevFocusInfo,
        aPrevKeyStr?: string
        ) {
            var i, len, el: HTMLElement, mapItem: TKeyMapItem, prevMapItem: TKeyMapItem = null;
            var startIndex: number = -1;
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

    export var KBuilderLeftRight: FKeyMapBuilder = function (
        aKeyMap: CKeyMap,
        aFocusable: HTMLElement[],
        aPrevFocusInfo?: TPrevFocusInfo,
        aPrevKeyStr?: string
        ) {
            var i, j, len, el: HTMLElement, mapItem: TKeyMapItem, prevMapItem: TKeyMapItem = null;
            var startIndex: number = 0;
            if (aFocusable.length) {
                for (i = 0, len = aFocusable.length; i < len; i++) {
                    el = aFocusable[i];
                    mapItem = {
                        el: el
                    }
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

    export var KBuilderGrid: FKeyMapBuilder = function (
        aKeyMap: CKeyMap,
        aFocusable: HTMLElement[],
        aPrevFocusInfo?: TPrevFocusInfo,
        aPrevKeyStr?: string,
        aHint?: any[]
        ) {
        if(aHint) {
            var temporary = true;
            var focusableLen = aFocusable.length;
            var itemIndex = 0;
            var itemData = parseInt(aFocusable[itemIndex].getAttribute('data'));
            var startData = parseInt(aFocusable[itemIndex].getAttribute('data'));
            var endData = parseInt(aFocusable[focusableLen-1].getAttribute('data'));
            var startIndex = 0;
            var startDataIndex = startData;
            var i, j, rowLen=aHint.length;
            var findindex = function (aData: number) {
                var ret = -1;
                var len = aFocusable.length;
                var i;
                for(i=0; i<len; i++) {
                    if(parseInt(aFocusable[i].getAttribute('data')) === aData) {
                        ret = i;
                        break;
                    }
                }
                return ret;
            }
            if(temporary) {
                for(i=0; i<startData; i++) {
                    aKeyMap.addMapItem(null);
                }
            }
            for(i=0; i<rowLen; i++) {
                var colBlock = aHint[i];
                var colLen = colBlock.length;
                for(j=0; j<colLen; j++) {
                    if(itemData === colBlock[j]) {
                        var mapItem: TKeyMapItem = {
                            el : aFocusable[itemIndex]
                        }
                        if (mapItem.el.classList.contains(KClassFocused)) {
                            startIndex = itemIndex;
                            startDataIndex = itemData;
                        }
                        // up
                        if(i > 0 && aHint[i-1][j] !== undefined) {
                            var t = 1;
                            while(i-t >= 0) {
                                var temp = aHint[i-t][j];
                                if(temp >= startData && temp <= endData && temp !== itemData) {
                                    if(temporary) {
                                        mapItem.u = temp;
                                    } else {
                                        mapItem.u = findindex(temp);
                                    }
                                    break;
                                } else {
                                    t++;
                                    continue;
                                }
                            }
                        }
                        // down
                        if(i < rowLen-1 && aHint[i+1][j] !== undefined) {
                            var t = 1;
                            while(i+t <= rowLen-1) {
                                var temp = aHint[i+t][j];
                                if(temp >= startData && temp <= endData && temp !== itemData) {
                                    if(temporary) {
                                        mapItem.d = temp;
                                    } else {
                                        mapItem.d = findindex(temp);
                                    }
                                    break;
                                } else {
                                    t++;
                                    continue;
                                }
                            }
                        }
                        // left
                        if(j > 0 && aHint[i][j-1] !== undefined) {
                            var t = 1;
                            while(j-t >= 0) {
                                var temp = aHint[i][j-t];
                                if(temp >= startData && temp <= endData && temp !== itemData) {
                                    if(temporary) {
                                        mapItem.l = temp;
                                    } else {
                                        mapItem.l = findindex(temp);
                                    }
                                    break;
                                } else {
                                    t++;
                                    continue;
                                }
                            }
                        }
                        // right
                        if(j < colLen-1 && aHint[i][j+1] !== undefined) {
                            var t = 1;
                            while(j+t <= colLen-1) {
                                var temp = aHint[i][j+t];
                                if(temp >= startData && temp <= endData && temp !== itemData) {
                                    if(temporary) {
                                        mapItem.r = temp;
                                    } else {
                                        mapItem.r = findindex(temp);
                                    }
                                    break;
                                } else {
                                    t++;
                                    continue;
                                }

                            }
                        }
                        aKeyMap.addMapItem(mapItem);
                        if(itemIndex < focusableLen - 1) {
                            itemData = parseInt(aFocusable[++itemIndex].getAttribute('data'));
                        }
                    }
                }
            }
            aFocusable[startIndex].classList.add(KClassFocused);
            if(temporary) {
                aKeyMap.setActiveFocus(startDataIndex);
            } else {
                aKeyMap.setActiveFocus(startIndex);
            }
        } else {
            var i, j, len;
            var el: HTMLElement;
            var mapItem: TKeyMapItem;
            var prevMapItem: TKeyMapItem = null;
            var aboveMapItem: TKeyMapItem = null;
            var startIndex: number = 0;
            var rowCount: number = 0;
            var colCount: number = 0;
            var posY: number = -1;
            for (i = 0, len = aFocusable.length; i < len; i++) {
                el = aFocusable[i];
                mapItem = {
                    el: el
                }
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
                } else {
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

    export var KBuilderGridHorizontal: FKeyMapBuilder = function (
        aKeyMap: CKeyMap,
        aFocusable: HTMLElement[],
        aPrevFocusInfo?: TPrevFocusInfo,
        aPrevKeyStr?: string,
        aMAX_ROW?: number
        ) {
        var i, len;
        var el: HTMLElement;
        var mapItem: TKeyMapItem;
        var prevMapItem: TKeyMapItem = null;
        var aboveMapItem: TKeyMapItem = null;
        var startIndex: number = 0;
        var rowCount: number = 0;
        var colCount: number = 0;
        var posX: number = -1;
        var posY: number = -1;
        var dataIndex: number;
        var row: number;
        var col: number;
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

    export var KBuilderWeightDistance: FKeyMapBuilder = function (
        aKeyMap: CKeyMap,
        aFocusable: HTMLElement[],
        aPrevFocusInfo?: TPrevFocusInfo,
        aPrevKeyStr?: string
        ) {
        var i, j, cnt: number = aFocusable.length, el: HTMLElement;
        var getPos = function (aElement: HTMLElement) {
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
        var getPosByRect = function (aRect: TRect) {
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
            var item: TKeyMapItem = {
                el: el
            };
            var me = rectInfoList[i];
            fillKeyMapItem(item, me);
            aKeyMap.addMapItem(item);
        }
        aKeyMap.setActiveFocus(startIndex);
        if (aPrevFocusInfo) {
            var prevPosData = getPosByRect(aPrevFocusInfo.rect);
            var item: TKeyMapItem = {
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
    }

    interface TSignalHandlerInfo {
        holder: any;
        slotName: string;
    }
    export class CSignalSource {
        private _signalTypes: { [signalName: string]: TSignalHandlerInfo[]; } = {};
        constructor() {
        }
        destroy() {
            this._signalTypes = null;
        }
        registerSignal(aSignalList: string[]) {
            var i, len, signalName: string;
            for (i = 0, len = aSignalList.length; i < len; i++) {
                signalName = aSignalList[i];
                if (this._signalTypes[signalName]) {
                    throw "Event [" + signalName + "] already exists";
                }

                this._signalTypes[signalName] = [];
            }
        }
        connect(aSignalName: string, aHolder: any, aSlotName: string) {

            if (!(aHolder[aSlotName] instanceof Function)) {
                throw "holder does not have the slot";
            }

            var signalHandlers: TSignalHandlerInfo[];
            var signalHandler: TSignalHandlerInfo;
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
        }
        disconnect(aHolder: {}) {
            var signalName: string;
            var signalHandlers: TSignalHandlerInfo[];
            var signalHandler: TSignalHandlerInfo;
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
        }
        length(aSignalName: string) {
            return this._signalTypes[aSignalName].length;
        }
        emit(...args: any[]): boolean {
            var ret = false;
            var signalName: string;
            var i, len, handlerInfoList: TSignalHandlerInfo[], handlerInfo: TSignalHandlerInfo;
            var holder;
            signalName = args.shift();
            handlerInfoList = this._signalTypes[signalName];
            for (i = 0, len = handlerInfoList.length; i < len; i++) {
                handlerInfo = handlerInfoList[i];
                handlerInfo.holder[handlerInfo.slotName].apply(handlerInfo.holder, args);
                ret = true;
            }
            return ret;
        }
    }

    export enum TParamOrientation {
        EUnknown,
        EVertical,
        EHorizontal
    }

    export class CClockTimer extends CSignalSource {

        private _clockTimer;
        private _clockDate;
        private _clockSec;
        private _clockMin;
        private _clockHour;
        private _clockDay;

        constructor() {
            super();
            this._clockTimer = setInterval(() => {
                this._clockTimerCallback();
            }, 1000);
            var now = new Date();
            this._clockDate = now;
            this._clockSec = now.getSeconds();
            this._clockMin = now.getMinutes();
            this._clockHour = now.getHours();
            this._clockDay = now.getDate();
            this.registerSignal(["MinuteChanged"]);
        }

        _clockTimerCallback() {
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
                            //this.fireEvent(CClockTimer.THandlerType.EDay, this._clockDay);
                        }
                        this._clockHour = now.getHours();
                        //this.fireEvent(CClockTimer.THandlerType.EHour, this._clockDay, this._clockHour);
                    }
                    this._clockMin = now.getMinutes();
                    //this.fireEvent(CClockTimer.THandlerType.EMin, this._clockDay, this._clockHour, this._clockMin);
                    this._emitMinuteChanged();
                }
                this._clockSec = now.getSeconds();
                //this.fireEvent(CClockTimer.THandlerType.ESec, this._clockDay, this._clockHour, this._clockMin, this._clockSec);
                //console.log('CClockTimer length=' + this.length('MinuteChanged'));
            }
        }

        connectMinuteChanged(aHolder: any, aSlotName: string, aSlot: () => void) {
            this.connect("MinuteChanged", aHolder, aSlotName);
        }
        private _emitMinuteChanged() {
            this.emit.call(this, "MinuteChanged");
        }


    }

    export class CControl extends CSignalSource {
        static KClassFocusable = "-f";
        static KClassFocused = "-fd";

        private _$self: JQuery;
        _element: HTMLElement;
        _keyMap: CKeyMap; // @protected
        _keyMapBuilder: FKeyMapBuilder;
        private _transitioning: boolean;
        _id: string;
        name: string;
        _focusableClassName;
        _focusedClassName;
        _shown: boolean;
        _hideTimer: number;
        _children: CControl[];
        _naviMap;
        //XXX: added
        _group: boolean;
        _parent: CControl;
        _focusableElement: HTMLElement[];

        constructor(aEl: JQuery, focusableClassName?, focusedClassName?, focusChangedEventHandler?) {
            super();
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
        destroy() {
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
            } else {
                throw "control double freed";
            }
            delete this._children;
            this._$self.remove();
            delete this._$self;
            delete this._element;
            delete this._parent;
        }
        $self(): JQuery {
            return this._$self;
        }
        setId(aId: string) {
            this._$self.attr("id", aId);
        }
        addClass(aClassName: string) {
            this._$self.addClass(aClassName);
        }
        getName() {
            return this.name;
        }

        /*
         * Focus management
         */

        setActiveFocus() {
            this._setActiveFocus(true);
        }
        getFocusedElement(): HTMLElement {
            if (this._keyMap) {
                return this._keyMap.getFocusedElement();
            }
            return null;
        }
        getFocusedElementIndex(): number {
            if (this._keyMap) {
                return this._keyMap.getFocusedIndex();
            }
            return -1;
        }
        getFocusableElementCount(): number {
            var ret = 0;
            if (this._keyMap) {
                ret = this._keyMap.getMapCount();
            }
            return ret;
        }
        getElementByIndex(aIndex: number): HTMLElement {
            if (this._keyMap) {
                var ret = this._keyMap.getMapItem(aIndex);
                if (ret) {
                    return ret.el;
                }
            }
            return null;
        }
        getIndexByElement(aElement: HTMLElement) {
            if (this._keyMap) {
                return this._keyMap.getIndex(aElement);
            }
            return -1;
        }
        getFocusedChild(): CControl {
            if (this._children.length) {
                var index = $(this._keyMap.getFocusedElement()).data("index");
                return this._children[index];
            }
            return null;
        }
        getChildIndex(aChild: CControl) {
            var i, len, c: CControl;
            for (i=0, len=this._children.length; i<len; i++) {
                c = this._children[i];
                if (c == aChild) {
                    return i;
                }
            }
            return -1;
        }
        getChildCount(): number {
            return this._children.length;
        }
        getChild(aIndex: number) {
            return this._children[aIndex];
        }
        setFocusedElement(aElement: HTMLElement): boolean {
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
        }
        setFocusedElementByIndex(aIndex: number, aForce?: boolean) {
            var ret = false;
            if (0 <= aIndex && this._keyMap && aIndex < this._keyMap.getMapCount()) {
                var focusedIndex = this._keyMap.getFocusedIndex();
                if ((aForce || focusedIndex != aIndex) && this._keyMap.getMapItem(aIndex)) {
                    this._keyMap.changeFocus(aIndex);
                    ret = true;
                }
            }
            return ret;
        }
        setFocusedChild(aControl: CControl) {
            var index = this.getChildIndex(aControl);
            if (index != -1) {
                this.setFocusedElementByIndex(index);
            } else {
                throw "child not found something wrong?";
            }
        }
        setFocus(focused) {
            try {
                this.doSetFocus(focused);
            } catch (e) {
                console.log('CControl[' + this.getName() + '].setFocus FAILED with ' + e);
            }
        }
        isFocused(): boolean {
            return this._$self.hasClass(KClassFocused);
        }
        isActiveFocused(): boolean {
            var i, len, c: CControl;
            for (i=0, len=this._children.length; i<len; i++) {
                c = this._children[i];
                if (c.isActiveFocused()) {
                    return true;
                }
            }
            if (this._keyMap) {
                return $(this._keyMap.getFocusedElement()).hasClass("-afd-leaf");
            }
            return false;
        }
        hasFocus() {
            return this.doHasFocus();
        }
        hasFocusable() {
            return this.doHasFocusable();
        }

        /*
         * Animation
         */
        isTransitioning(): boolean {
            return this._transitioning;
        }
        isChildTransitioning(): boolean {
            var i, len, c: CControl;
            for (i = 0, len = this._children.length; i < len; i++) {
                c = this._children[i];
                if (c.isTransitioning()) {
                    return true;
                }
            }
            return false;
        }
        _setTransitioning(aTransitioning: boolean, aIgnoreCheck: boolean = false) {
            if (!aIgnoreCheck && this._transitioning == aTransitioning) {
                console.error("settting transitioning error " + aTransitioning);
                throw "something wrong?";
            }
            this._transitioning = aTransitioning;
            this._emitTransitioningChanged(aTransitioning);
        }
        setTransition(aTransition: boolean) {
            this._setTransitioning(aTransition);
        }
        /*
         * Control visibility utility
         */
        isShown() {
            return this._shown;
        }
        show(param?): boolean {
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
        }
        doShow(param?): boolean {
            throw 'doShow is not implemented!';
        }
        hide(param?): boolean {
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
        }
        doHide(param?): boolean {
            throw 'doHide is not implemented!';
        }
        hideAfter(milisecond) {
            if (this._hideTimer) {
                clearTimeout(this._hideTimer);
            }
            this._hideTimer = setTimeout(() => {
                this.hide({ timedOut: true });
            }, milisecond);
        }
        clearHideAfter() {
            if (this._hideTimer) {
                clearTimeout(this._hideTimer);
            }
        }

        /*
         * Child control utilities
         */
        addControl(control, bFirst = false): void {
            if (bFirst) {
                this._children.unshift(control);
            } else {
                this._children.push(control);
            }
        }
        removeControl(control) {
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
        }
        removeAllControl(buildNaviMap?) {
            this._children = [];
            if (buildNaviMap) {
                this.buildNaviMapForChild(KBuilderTopDown);
            }
        }
        hasControl(name) {
            var rv = false;
            var i, max;
            max = this._children.length;
            for (i = 0; i < max; i++) {
                if (this._children[i].getName() == name)
                    rv = true;
            }
            return rv;
        }
        doKeyForChild(aKeyStr: string, args?) {

            var i, childList: CControl[] = [], child: CControl;
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
        }
        setFocusedControl(aChildControl: CControl) {
            this.setFocusedElement(aChildControl.$self().get(0));
        }
        removeControlAll() {
            this._children.length = 0;
        }
        releaseKey(key, args?) {
            var handler = 'releaseKey' + key;
            if (typeof this[handler] != 'undefined') {
                return this[handler](args);
            }
            return false;
        }
        doKey(aKeyStr: string, aArgs?: any): boolean {
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
                //debugger;
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
        }

        /*
         * intrrupt
         */
        interrupt(type, args) {
            console.log("INTERRUPT", "[" + type + "] {" + args + "}");
            return this.doInterrupt(type, args);
        }
        doInterrupt(type, args) {
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
        }
        hasChild(): boolean {
            return (this._children && this._children.length) ? true: false;
        }
        _doFocusChanged(aElOld: HTMLElement, aElNew: HTMLElement) {
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
        }
        _setActiveFocus(aFocused: boolean) {
            if (this._keyMap) {
                if (this.hasChild()) {
                    var focusedChild = this.getFocusedChild();
                    if (focusedChild) {
                        focusedChild._setActiveFocus(aFocused);
                    }
                } else {
                    var focused = this._keyMap.getFocusedElement();
                    if (focused) {
                        if (aFocused) {
                            focused.classList.add(KClassActiveFocusedLeaf);
                        } else {
                            focused.classList.remove(KClassActiveFocusedLeaf);
                        }
                    }
                }
            }

            if (aFocused) {
                this._doFocusGained();
            } else {
                this._doFocusLost();
            }
        }
        _doFocusGained() {
//            console.info(this.name + "._doFocusGained");
            this._emitFocusGained();
        }
        _doFocusLost() {
//            console.info(this.name + "._doFocusLost");
            this._emitFocusLost();
        }
        _doChildFocusChanged(aOld: CControl, aNew: CControl) {
//            console.info(this.name + "._doChildFocusChanged");
            this._emitChildFocusChanged(aOld, aNew);
        }

        connectFocusChanged(aHolder: any, aSlotName: string, aSlot: FFocusChanged) {
            this.connect("FocusChanged", aHolder, aSlotName);
        }
        _emitFocusChanged(aOld: HTMLElement, aNew: HTMLElement) {
            this.emit.call(this, "FocusChanged", aOld, aNew);
        }
        connectChildFocusChanged(aHolder: any, aSlotName: string, aSlot: FChildFocusChanged) {
            this.connect("ChildFocusChanged", aHolder, aSlotName);
        }
        private _emitChildFocusChanged(aOld: CControl, aNew: CControl) {
            this.emit.call(this, "ChildFocusChanged", aOld, aNew);
        }
        connectFocusGained(aHolder: any, aSlotName: string, aSlot: FFocusGained) {
            this.connect("FocusGained", aHolder, aSlotName);
        }
        private _emitFocusGained() {
            this.emit.call(this, "FocusGained", this);
        }
        connectFocusLost(aHolder: any, aSlotName: string, aSlot: FFocusLost) {
            this.connect("FocusLost", aHolder, aSlotName);
        }
        private _emitFocusLost() {
            this.emit.call(this, "FocusLost", this);
        }
        connectItemSelected(aHolder: any, aSlotName: string, aSlot: FItemSelected) {
            this.connect("ItemSelected", aHolder, aSlotName);
        }
        private _emitItemSelected(aIndex: number, aEl: HTMLElement) {
            this.emit.call(this, "ItemSelected", this, aIndex, aEl);
        }
        connectTransitioningChanged(aHolder: any, aSlotName: string, aSlot: (aTransitioning: boolean) => void) {
            this.connect("TransitioningChanged", aHolder, aSlotName);
        }
        private _emitTransitioningChanged(aTransitioning: boolean) {
            this.emit.call(this, "TransitioningChanged", aTransitioning);
        }

        //FIXME added for error
        connectItemInserted(aHolder: any, aSlotName: string, aSlot: FItemInserted) {
            this.connect("ItemInserted", aHolder, aSlotName);
        }
        connectItemRemoved(aHolder: any, aSlotName: string, aSlot: FItemRemoved) {
            this.connect("ItemRemoved", aHolder, aSlotName);
        }
        connectKeyMapUpdated(aHolder: any, aSlotName: string, aSlot: (aKeyMap: CKeyMap) => void) {
            this.connect("KeyMapUpdated", aHolder, aSlotName);
        }
        private _emitKeyMapUpdated(aKeyMap: CKeyMap) {
            this.emit.call(this, "KeyMapUpdated", aKeyMap);
        }
        /*
         * Key navigation map utilities
         */
        isMovable(aKeyStr: string): boolean {
            if (this._keyMap) {
                return this._keyMap.isMovable(aKeyStr);
            }
            return false;
        }
        getNextMovableIndex(aKeyStr: string): number {
            if (this._keyMap) {
                return this._keyMap.getNextMovableIndex(aKeyStr);
            }
            return -1;
        }
        getNextMovableElement(aKeyStr: string): HTMLElement {
            if (this._keyMap) {
                return this._keyMap.getNextMovableElement(aKeyStr);
            }
            return null;
        }
        getNextMovableChild(aKeyStr: string): CControl {
            if (this._keyMap && this._children.length) {
                var index = this._keyMap.getNextMovableIndex(aKeyStr);
                return this._children[index];
            }
            return null;
        }
        buildNaviMap(aBuilder: FKeyMapBuilder) {
            var keyMap = new CKeyMap((aOld: HTMLElement, aNew: HTMLElement) => {
                this._doFocusChanged(aOld, aNew);
            }, this._children.length ? KClassActiveFocused : KClassActiveFocusedLeaf);

            var $focusables = this._$self.find("." + KClassFocusable);
            aBuilder(keyMap, <HTMLElement[]><any>$focusables);
            this._keyMap = keyMap;
            this._keyMapBuilder = aBuilder;
        }
        buildNaviMapForChild(aBuilder: FKeyMapBuilder, aFocusedChild?: CControl) {
            if (this._keyMap) {
                this._keyMap.destroy();
                this._keyMap = null;
            }

            var i, len = this._children.length;
            if (len) {
                var child: CControl, $child, element: HTMLElement, focusables: HTMLElement[] = [];
                var focusedIndex = this._children.indexOf(aFocusedChild);
                focusedIndex = focusedIndex == -1 ? 0: focusedIndex;
                for (i = 0; i < len; i++) {
                    child = this._children[i];
                    $child = child.$self();
                    $child.addClass(KClassFocusable);
                    if (i == focusedIndex) {
                        $child.addClass(KClassFocused);
                    } else {
                        $child.removeClass(KClassFocused);
                    }
                    $child.data("index", "" + i);
                    element = $child[0];
                    focusables.push(element);
                }

                var keyMap = new CKeyMap((aOld: HTMLElement, aNew: HTMLElement) => {
                    this._doFocusChanged(aOld, aNew);
                }, this._children.length ? KClassActiveFocused : KClassActiveFocusedLeaf);
                aBuilder(keyMap, focusables);
                this._keyMap = keyMap;
                this._keyMapBuilder = aBuilder;
            }
        }
        focusedChild() {
            var i;
            for (i = 0; i < this._children.length; i++) {
                var child = this._children[i].$self();
                if (child.hasClass(this._focusedClassName)) {
                    return this._children[i];
                }
            }
            return null;
        }
        doNaviKey(aKeyStr: string): boolean {
            if (this._keyMap) {
                return this._keyMap.doKey(aKeyStr);
            }
            return false;
        }
        _findChildByJq($jq) {
            var i;
            for (i = 0; i < this._children.length; i++) {
                var child = this._children[i];
                if (child._id == $jq.attr('id')) {
                    return child;
                }
            }
            return null;
        }
        handleFocusChanged($old, $new) {
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
            } else {
                if (this.doHandleFocusChanged) {
                    this.doHandleFocusChanged($old, $new);
                }
            }
        }
        doHandleFocusChanged(childOld, childNew) {
        }

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
        _doKeyOk() {
            var i;
            if (this._focusedClassName) {
                if (this._children.length) {
                    for (i = 0; i < this._children.length; i++) {
                        var focusedChild = this._children[i].$self();
                        if (focusedChild.hasClass(this._focusedClassName)) {
                            return this.doSelect(focusedChild)? true : false;
                        }
                    }
                }
                else {
                    //var focused = $(this._id + ' .' + this._focusedClassName);
                    var focused = this._$self.find("." + this._focusedClassName);
                    return this.doSelect(focused)? true : false;
                }
            }
            return false;
        }
        _doKeyEnter() {
            return this._doKeyOk();
        }
        doSelect($aFocusedChild: JQuery): boolean {
            return false;
        }

        /*
         * Virtual functions for subclasses
         */
        functionClass = null;
        doSetFocus(focused) {
            if (focused) {
                if (!this._$self.hasClass(this._focusedClassName)) {
                    this._$self.addClass(this._focusedClassName);
                }
            } else {
                this._$self.removeClass(this._focusedClassName);
            }
        }
        doHasFocus() {
            return this._$self.hasClass(this._focusedClassName);
        }
        doHasFocusable(): boolean {
            var ret = this._$self.find('.' + this._focusableClassName).length
            console.log('CControl[' + this.getName() + '].prototype.doHasFocusable [' + this._focusableClassName + ']' + ret);
            return ret ? true: false;
        }
        images;
        preLoadImages($imgs, callback?) {
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
                    img.decode(
                        function () {
                            loadedCount++;
                            if (loadedCount >= length) {
                                if (callback) {
                                    callback();
                                }
                            }
                            console.log('CControl.preLoadImages: Success');
                            console.log(StringFormat('CControl.preLoadImages: Total Count - {0}, Loaded Count - {1}', length, loadedCount));
                        },
                        function () {
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
        }

        /*
         from v2 CControl
         */
        private _prevFocusInfo: TPrevFocusInfo;
        private _prevKeyStr: string;
        private _saveFocusInfo() {
            if (this._keyMap) {
                var prevFocusedEl: HTMLElement = this._keyMap.getFocusedElement();
                this._prevFocusInfo = {
                    rect: Util.getRect(prevFocusedEl),
                    activeFocus: prevFocusedEl.classList.contains(KClassActiveFocusedLeaf)
                };
            }
        }
        _handleFocusChanged(aElOld: HTMLElement, aElNew: HTMLElement) {
            this._emitFocusChanged(aElOld, aElNew);
        }
        _keyMapFocusChanged(aOld: HTMLElement, aNew: HTMLElement) {
            this._handleFocusChanged(aOld, aNew);
        }
        _makeKeyMap(drawnElements: HTMLElement[], aKeepFocus: boolean): void {
            /* setKeepFocus => update
            if (aKeepFocus) {
                this._saveFocusInfo();
            }
            */
            if (drawnElements && drawnElements.length) {
                var keyMap = new CKeyMap((aOld: HTMLElement, aNew: HTMLElement) => {
                    this._keyMapFocusChanged(aOld, aNew);
                }, this._group ? KClassActiveFocused : KClassActiveFocusedLeaf);
                this._keyMapBuilder(keyMap, drawnElements, this._prevFocusInfo, aKeepFocus ? null: this._prevKeyStr, this._doGetKeyMapHint());
                this._keyMap = keyMap;
                this._element.classList.add(KClassFocusable);
                if (this._prevFocusInfo && this._prevFocusInfo.activeFocus) {
                    this.setActiveFocus();
                }
            } else {
                this._keyMap = null;
                this._element.classList.remove(KClassFocusable);
            }
            this._emitKeyMapUpdated(this._keyMap);
        }
        _doGetKeyMapHint() {
            return null;
        }
        draw(aRect?: TRect) {
            aRect = aRect || null;
            var param = this._prepareParam();
            var keepFocus = param['keepFocus'];
//            var drawnElements: HTMLElement[]
//            drawnElements = this._doDraw(aRect, param);
            this._focusableElement = this._doDraw(aRect, param);
            this._clearVolitile();
            this._makeKeyMap(this._focusableElement, keepFocus);
            this._saveFocusInfo();
        }
        _doDraw(aRect: TRect, aDrawParam: { [key: string]: any; }): HTMLElement[] {
            var ret: HTMLElement[];
            if (this._drawEngine) {
                ret = this._drawEngine(aRect, aDrawParam);
            }
            return ret;
        }
        setDrawEngine(aFnDrawEngine: (aRect: TRect, aDrawParam: { [key: string]: any; }) => HTMLElement[]): void {
            this._drawEngine = aFnDrawEngine;
        }
        private _drawEngine: (aRect: TRect, aDrawParam: { [key: string]: any; }) => HTMLElement[] = null;
        update(aFirstCalled: boolean = true) { {}
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
        }
        _doUpdate(aKeepFocus: boolean = false) {
            if (aKeepFocus) {
                this._saveFocusInfo();
            }
        }
        //FIXME: consider draw()
        private _drawParam: { [key: string]: any; } = {};
        private _drawParamVolitile: { [key: string]: any; } = {};
        private _clearVolitile() {
            this._drawParamVolitile = {};
        }
        _setDrawParam(aParamName: string, aValue: any, aVolitile: boolean) {
            if (aVolitile) {
                this._drawParamVolitile[aParamName] = aValue;
            } else {
                this._drawParam[aParamName] = aValue;
            }
        }
        _getDrawParam(aParamName: string): any {
            var ret = this._drawParamVolitile[aParamName];
            if (ret == null) {
                ret = this._drawParam[aParamName];
            }
            return ret;
        }
        private _prepareParam() {
            var ret: { [key: string]: any; } = {};
            var key: string;
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
        }
        // Item info
        setItemHeight(aItemHeight: number) {
            this._setDrawParam(KParamStrItemHeight, aItemHeight, false);
        }
        getItemHeight(): number {
            return this._getDrawParam(KParamStrItemHeight) || false;
        }
        setItemWidth(aItemWidth: number) {
            this._setDrawParam(KParamStrItemWidth, aItemWidth, false);
        }
        getItemWidth(): number {
            return this._getDrawParam(KParamStrItemWidth) || false;
        }
        // Animaion
        setAnimation(aAnimation: boolean) {
            this._setDrawParam(KParamAnimation, aAnimation, false);
        }
        getAnimation(): boolean {
            return this._getDrawParam(KParamAnimation) || false;
        }
        // Keep focus
        setKeepFocus(aKeepFocus: boolean) {
            this._setDrawParam(KParamKeepFocus, aKeepFocus, true);
        }
        getKeepFocus() {
            return this._getDrawParam(KParamKeepFocus) || false;
        }
        setStartIndex(aIndex: number) {
            this._setDrawParam(KParamStrStartIndex, aIndex, false);
        }
        getStartIndex(): number {
            return this._getDrawParam(KParamStrStartIndex);
        }
        // Utilities
        isFocusable(): boolean {
            return this._element.classList.contains(KClassFocusable);
        }
        // Scrolling scheme
        setScrollScheme(aScheme: TParamScrollScheme, aFixedScrollUnit?: number) {
            this._setDrawParam(KParamStrScrollSchemeVertical, aScheme, false);
            this._setDrawParam(KParamStrScrollSchemeHorizontal, aScheme, false);
            if (aScheme == TParamScrollScheme.EByFixed) {
                if (aFixedScrollUnit) {
                    this._setDrawParam(KParamStrScrollSchemeFixedUnitVertical, aFixedScrollUnit, false);
                    this._setDrawParam(KParamStrScrollSchemeFixedUnitHorizontal, aFixedScrollUnit, false);
                } else {
                    throw "fixed scroll unit is missiong"
                }
            }
        }
        setVerticalScrollScheme(aScheme: TParamScrollScheme, aFixedScrollUnit?: number) {
            this._setDrawParam(KParamStrScrollSchemeVertical, aScheme, false);
            if (aScheme == TParamScrollScheme.EByFixed) {
                if (aFixedScrollUnit) {
                    this._setDrawParam(KParamStrScrollSchemeFixedUnitVertical, aFixedScrollUnit, false);
                } else {
                    throw "fixed scroll unit is missiong"
                }
            }
        }
        getVerticalScrollScheme(): TParamScrollScheme {
            return this._getDrawParam(KParamStrScrollSchemeVertical) || TParamScrollScheme.EByItem;
        }
        setHorizontalScrollScheme(aScheme: TParamScrollScheme, aFixedScrollUnit?: number) {
            this._setDrawParam(KParamStrScrollSchemeHorizontal, aScheme, false);
            if (aScheme == TParamScrollScheme.EByFixed) {
                if (aFixedScrollUnit) {
                    this._setDrawParam(KParamStrScrollSchemeFixedUnitHorizontal, aFixedScrollUnit, false);
                } else {
                    throw "fixed scroll unit is missiong"
                }
            }
        }
        getHorizontalScrollScheme(): TParamScrollScheme {
            return this._getDrawParam(KParamStrScrollSchemeHorizontal) || TParamScrollScheme.EByItem;
        }

        getSize(aPageSize?: TSize): TSize {
            return {
                width: this._element.offsetWidth,
                height: this._element.offsetHeight
            }
        }

        setParent(aParentControl: CControl) {
            this._parent = aParentControl;
        }

    }
    export interface TCreateLayerParam {
        addClass?: string;
        transition?: {
            custom?: {
                fnCreate: (aPrevLayer: HTMLElement, aNewLayer: HTMLElement) => Function;
                fnRemove: (aPrevLayer: HTMLElement, aNewLayer: HTMLElement) => Function;
            };
            prevLayer?: string;
            newLayer?: string;
            fnBeforeTransition?: Function;
            fnAfterTransition?: Function;
        };
        fnBeforeRemoval?: Function;
        fnAfterRemoval?: Function;
    }
    interface TLayerInfo {
        createParam: TCreateLayerParam;
        elLayer: HTMLElement;
        keyMap: CKeyMap;
        keyMapBuilder: FKeyMapBuilder;
        childControls: CControl[];
    }
    export class CLayeredGroupControl extends CControl {

        private _layerInfoStack: TLayerInfo[] = [];
        private _createParam: TCreateLayerParam;
        private _elLayer: HTMLElement;

        constructor($self: JQuery) {
            super($self);
            this.addClass("-layered");
        }
        destroy() {
            var i, iLen, layerInfo: TLayerInfo, j, jLen, controls: CControl[];
            for (i = 0, iLen = this._layerInfoStack.length; i < iLen; i++) {
                layerInfo = this._layerInfoStack[i];
                controls = layerInfo.childControls;
                for (j = 0, jLen = controls.length; j < jLen; j++) {
                    controls[j].destroy();
                }
                Util.remove(layerInfo.elLayer);
            }
            this._layerInfoStack = null;
            super.destroy();
        }
        getLayoutElement(aClassName: string): HTMLElement {
            var ret: HTMLElement;
            if (this._elLayer) {
                var elements: NodeList = this._elLayer.getElementsByClassName(aClassName);
                if (elements.length == 1) {
                    ret = <HTMLElement> elements.item(0);
                } else if (elements.length == 0) {
                    ret = document.createElement("div");
                    ret.classList.add(aClassName);
                    this._elLayer.appendChild(ret);
                } else {
                    throw "Layout element duplicated";
                }
            }
            return ret;
        }
        getCurrentLayerElement(): HTMLElement {
            return this._elLayer;
        }
        createLayer(aParam: TCreateLayerParam = {}): HTMLElement {
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

            var executeNext: Function[] = [];
            if (aParam.transition) {
                this._setTransitioning(true);
                if (aParam.transition.custom) {
                    executeNext.push(aParam.transition.custom.fnCreate(elLayerPrev, elLayer));
                } else {
                    if (elLayerPrev) {
                        switch (aParam.transition.prevLayer) {
                            case "fadeOut":
                                elLayerPrev.style.transition = KCssTransitionParamOpa;
                                executeNext.push(() => {
                                    elLayerPrev.style.opacity = "0";
                                });
                                break;
                            case "moveLeft":
                                elLayerPrev.style.transition = KCssTransitionParamPos;
                                executeNext.push(() => {
                                    elLayerPrev.style.left = -this._element.offsetWidth + "px";
                                });
                                break;
                        }
                    }
                    switch (aParam.transition.newLayer) {
                        case "fadeIn":
                            elLayer.style.opacity = "0";
                            elLayer.style.transition = KCssTransitionParamOpa;
                            executeNext.push(() => {
                                elLayer.style.opacity = "1";
                            });
                            break;
                        case "moveLeft":
                            elLayer.style.transition = KCssTransitionParamPos;
                            elLayer.style.left = this._element.offsetWidth + "px";
                            executeNext.push(() => {
                                elLayer.style.left = "0px";
                            });
                            break;
                    }
                    if (aParam.transition.fnBeforeTransition) {
                        executeNext.push(
                            function () {
                                aParam.transition.fnBeforeTransition(elLayerPrev, elLayer);
                            }
                        );
                    }
                }
                Util.afterTransition(elLayer, () => {
                    this._setTransitioning(false);
                    if (aParam.transition.fnAfterTransition) {
                        aParam.transition.fnAfterTransition();
                    }
                });
            }

            this._element.appendChild(elLayer);
            this._elLayer = elLayer;
            this._createParam = aParam;

            if (executeNext.length) {
                setTimeout(() => {
                    var i, len;
                    for (i = 0, len = executeNext.length; i < len; i++) {
                        executeNext[i]();
                    }
                }, 1);
            }
            return elLayer;
        }
        removeLayer(): boolean {
            var prevChild = this._children;
            var prevElLayer = this._elLayer;
            var prevCreateParam = this._createParam;

            if (prevCreateParam.fnBeforeRemoval) {
                prevCreateParam.fnBeforeRemoval();
            }

            var destroy = () => {
                var i, len, control: CControl;
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

            var layerInfo: TLayerInfo = this._layerInfoStack.pop();
            var removed = false;
            if (layerInfo) {
                this._children = layerInfo.childControls;
                this._keyMap = layerInfo.keyMap;
                this._keyMapBuilder = layerInfo.keyMapBuilder;
                this._elLayer = layerInfo.elLayer;
                this._elLayer.classList.remove("-stacked");
                this._createParam = layerInfo.createParam;

                var executeNext: Function[] = [];
                var transitionParam = prevCreateParam.transition;
                if (transitionParam) {

                    if (transitionParam.custom) {
                        transitionParam.custom.fnRemove(prevElLayer, this._elLayer);
                    } else {
                        switch (transitionParam.prevLayer) {
                            case "fadeOut":
                                executeNext.push(() => {
                                    this._elLayer.style.opacity = "1";
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
                    Util.afterTransition(prevElLayer, () => {
                        destroy();
                        var i, len;
                        for (i = 0, len = executeNext.length; i < len; i++) {
                            executeNext[i]();
                        }
                        this._setTransitioning(false);
                    });
                } else {
                    destroy();
                }
                removed = true;
            } else {
                destroy();
            }
            return removed;
        }
        getLayerCount(): number {
            if (this._layerInfoStack) {
                return this._layerInfoStack.length;
            }
            return 0;
        }
        setAllLayersHidden() {
            var i, len, el: HTMLElement;
            for (i=0, len=this._layerInfoStack.length; i<len; i++) {
                el = this._layerInfoStack[i].elLayer;
                el.style.visibility = "hidden";
            }
            this._elLayer.style.visibility = "hidden";
        }
        unsetAllLayersHidden() {
            var i, len, el: HTMLElement;
            for (i=0, len=this._layerInfoStack.length; i<len; i++) {
                el = this._layerInfoStack[i].elLayer;
                el.style.visibility = "";
            }
            this._elLayer.style.visibility = "";
        }
        /*
        draw(aRect?: TRect) {
            super.draw(aRect);
            this.setActiveFocus();
        }
        _doDraw(aRect: TRect, aDrawParam: { [key: string]: any; }) {
            if (!this._elLayer) {
                throw "Layer must be created before draw";
            }
            return this._doDrawCommon(this._elLayer, aRect, aDrawParam);
        }

        createLayoutControl(aItemDrawers: FItemDrawer[]): CLayoutControl {
            if (!this._elLayer) {
                throw "Layer must be created before set controls";
            }
            this._keyMapBuilder = KBuilderTopDown;
            var layoutControl = new CLayoutControl(null);
            layoutControl.setItemDrawers(aItemDrawers);
            this.setOwnedChildControls([layoutControl]);
            return layoutControl;
        }

        createLayoutGroupControl(aControls: CControl[]): CLayoutGroupControl {
            if (!this._elLayer) {
                throw "Layer must be created before set controls";
            }
            this._keyMapBuilder = KBuilderTopDown;
            var layoutGroupControl = new CLayoutGroupControl(null);
            layoutGroupControl.setOwnedChildControls(aControls);
            this.setOwnedChildControls([layoutGroupControl]);
            return layoutGroupControl;
        }

        setControl(aControl: CControl) {
            if (!this._elLayer) {
                throw "Layer must be created before set controls";
            }
            this._keyMapBuilder = KBuilderTopDown;
            this.setOwnedChildControls([aControl]);
        }
        */
    }
    export class CDynamicControl extends CLayeredGroupControl {

        private _proxy: CDynamicControlProxy;
        private _cssFiles;
        private _cssLinks;
        private _preloadJs: string[];
        private _activated;

        constructor(aProxy: CDynamicControlProxy, aTarget: JQuery) {
            super(aTarget);
            this._proxy = aProxy;
            this._cssFiles = [];
            this._cssLinks = [];
            this._activated = false;
        }
        setPreloadJs(aJsFiles: string[]) {
            this._preloadJs = aJsFiles;
        }
        getPreloadJs(): string[] {
            return this._preloadJs;
        }
        preLoad(aCallback: () => void) {
            this._loadCssFiles(() => {
                this.doPreActivate();
                aCallback();
            });
        }
        afterLoad(aParam: any) {
            this.doActivate(aParam);
            this.doPostActivate();
        }
        activate(param, aCallback: (aOk: boolean) => void) {
            console.log('APP', this.getName() + '.activate CSS count=' + this._cssFiles.length);
            this._loadCssFiles(() => {
                console.log('PRF', 'DYCTRL.CSS loaded');
                var ok = false;
                try {
                    this.doPreActivate();
                    this.doActivate(param);
                    this.doPostActivate();
                    ok = true;
                    this._activated = true;
                }
                catch (e) {
                }
                console.log('APP', this.getName() + '.activation: ' + (ok ? 'OK' : 'FAILED'));
                if (aCallback) {
                    aCallback(ok);
                }
            }, aCallback);
        }
        doPreActivate() {
        }
        doActivate(param?) {
        }
        doPostActivate() {
        }
        deActivate(param?) {
            this._activated = false;
            this.doPreDeactivate();
            try {
                this.doDeActivate(param);
            } catch (e) {
                console.log('APP', this.getName() + '.deActivate ======================= FAILED =======================with ' + e.message);
            }
            this._unloadCssFiles();
            this.doPostDeactivate();
            console.log('APP', this.getName() + '.deActivate');
        }
        doPreDeactivate() {
        }
        doDeActivate(param) {
            throw 'doDeactivate is not implemented!';
        }
        doPostDeactivate() {
        }
        isActivated() {
            return this._activated;
        }
        suspend() {
            console.log('APP', this.getName() + '.SUSPEND');
            this._unloadCssFiles();
            if (this.doSuspend) {
                this.doSuspend();
            }
        }
        doSuspend() {
        }
        resume(callback) {
            console.log('APP', this.getName() + '.RESUME');
            this._loadCssFiles(() => {
                callback();
                if (this.doResume) {
                    this.doResume();
                }
            });
        }
        doResume() {
        }
        getCssFiles() {
            return this._cssFiles;
        }
        addCssFile(cssFile) {
            this._cssFiles.push(cssFile);
        }
        private _loadCssFiles(fnOk, fnFail?) {
            var i, cssFilesToLoad = [];
            for (i = 0; i < this._cssFiles.length; i += 1) {
                cssFilesToLoad.push(this._cssFiles[i]);
            }
            var addRemoveLinkList = (success, link?) => {
                if (success) {
                    if (link) {
                        this._cssLinks.push(link);
                    }
                    if (cssFilesToLoad.length) {
                        var cssFile = cssFilesToLoad.shift();
                        console.log('APP', this.getName() + '.CSS: "' + cssFile + '"');
                        loadStyleSheet(cssFile, addRemoveLinkList);
                    } else {
                        if (fnOk) {
                            fnOk();
                        }
                    }
                } else {
                    if (link) {
                        console.log('APP', this.getName() + '.CSS: "' + link + '" LOADING FAILED');
                    }
                    fnFail(false);
                }
            };
            addRemoveLinkList(true);
        }
        private _unloadCssFiles() {
            while (this._cssLinks.length) {
                var cssLink = this._cssLinks.pop();
                console.log('APP', this.getName() + '.UNLOAD CSS "' + cssLink.href + '"');
                removeStyleSheet(cssLink);
            }
        }
        _requestCustomReq(aReqName: string, aParam: any) {
            this._proxy._requestCustomReq(aReqName, aParam);
        }
    }
    export interface FActivateCallback {
        (aOk: boolean, aResumed: boolean): void;
    }
    export interface TSuspendInfo {
        $el: JQuery;
        control: CDynamicControl;
        activeDir: string;
        reason: string;
    }
    export class CDynamicControlProxy extends CSignalSource {

        private _id: string;
        private _$holder: JQuery;
        private _activating: boolean;
        private _resuming: boolean;
        private _activeDir: string;
        private _activateParam: any;
        private _targetJS;
        private _dynamicControl: CDynamicControl;
        private _suspendedInfos: TSuspendInfo[];

        static KClassActive = "-dynamic-active";

        constructor(aID: string, $holder: JQuery, aJsFileName: string) {
            super();
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
        destroy() {
            super.destroy();
        }
        private _activateCheck(aActiveDir: string, aActivateParam: any, aCallback: FActivateCallback) {
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
        }
        preActivate(aActiveDir: string, aActivateParam: any, aCallback: FActivateCallback) {
            if (!this._activateCheck(aActiveDir, aActivateParam, aCallback)) {
                return;
            }

            require(["pages/" + this._activeDir + "/" + this._targetJS], (aDynamicControlCreator) => {
                this._createDynamicControl(aDynamicControlCreator, () => {
                    this._dynamicControl.preLoad(() => {
                        this._emitPreActivated(aActiveDir);
                        aCallback(true, false);
                    })
                })
            });
        }
        finalizeActivate(): boolean {
            var ret = false;
            try {
                this._dynamicControl.afterLoad(this._activateParam);
                ret = true;
            } catch (e) {
            }
            this._activating = false;
            this._emitActivated(this._activeDir);
            return ret;
        }
        activate(aActiveDir: string, aActivateParam: any, aCallback: FActivateCallback): void  {
            if (!this._activateCheck(aActiveDir, aActivateParam, aCallback)) {
                return;
            }
            require(["pages/" + this._activeDir + "/" + this._targetJS], (aDynamicControlCreator) => {
                this._createDynamicControl(aDynamicControlCreator, () => {
                    this._dynamicControl.activate(aActivateParam, (ok) => {
                        this._activating = false;
                        aCallback(true, false);
                        this._emitActivated(aActiveDir);
                    });
                })
            });
        }
        deActivate(aParam?) {
            if (this._activating) {
                console.log('APP', this._id + '[' + this._activeDir + '].deActivate WARNNING ACTIVATING CONTROL');
                return false;
            } else if (this._dynamicControl === null) {
                console.log('APP', this._id + '[' + this._activeDir + '].deActivate HAS NO CONTROL');
                return false;
            } else {
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
        }
        control(): CControl {
            return this._dynamicControl;
        }
        isLoading() {
            return this._activating;
        }
        isActive() {
            return this._dynamicControl !== null;
        }
        isResuming() {
            return this._resuming;
        }
        getId(): string {
            return this._activeDir;
        }
        getActiveDir(): string {
            if (this.isActive()) {
                return this._activeDir;
            }
            return null;
        }
        findSuspendedControl(activeDir) {
            var i;
            for (i = 0; i < this._suspendedInfos.length; i += 1) {
                var info = this._suspendedInfos[i];
                if (info.activeDir == activeDir) {
                    return info;
                }
            }
            return null;
        }
        suspendControl(aReason: string) {
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
        }
        resumeControl(aReason: string) {
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
                this._dynamicControl.resume(() => {
                    this._resuming = false;
                    this._$holder.append(item.$el);
                    this._$holder.addClass(CDynamicControlProxy.KClassActive);
                    this._emitResumed(aReason, this._activeDir);
                });
                return true;
            }
            return false;
        }
        private _createDynamicControl(aCreator, aCallback) {
            this._$holder.addClass(CDynamicControlProxy.KClassActive);
            this._dynamicControl = new aCreator(this);
            var preloadJs = this._dynamicControl.getPreloadJs();
            if (preloadJs) {
                require(preloadJs, aCallback);
            } else {
                aCallback();
            }
        }
        private _resumeControlByDir(aActiveDir: string): string {
            var i;
            if (this._activating || this._dynamicControl) {
                console.log('APP', this._id + '[' + this._activeDir + '].resumeControl NOT POSSIBLE');
                return null;
            }
            console.log('APP', this._id + '[' + this._activeDir + '].resumeControl level=' + this._suspendedInfos.length);
            var info: TSuspendInfo;
            var item: TSuspendInfo = null;
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
                this._dynamicControl.resume(() => {
                    this._resuming = false;
                    this._$holder.append(item.$el);
                    this._$holder.addClass(CDynamicControlProxy.KClassActive);
                    this._emitResumed(item.reason, this._activeDir);
                });
                return item.reason;
            }
            return null;
        }
        clearSuspendedControl(aReason: string) {
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
        }
        clearSuspendedPages() {
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
        }
        hasControlStack() {
            return this._suspendedInfos.length > 0;
        }
        private _isPossibleToActivate(aActiveDir): boolean {
            if (this._activating) {
                console.log('APP', this._id + '[' + aActiveDir + '] ALREADY ACTIVATING');
                return false;
            } else if (this._dynamicControl) {
                console.log('APP', this._id + '[' + aActiveDir + '] DEACTIVATE FIRST');
                return false;
            }
            return true;
        }

        connectPreActivated(aHolder: any, aSlotName: string, aSlot: (aId: string) => void) {
            this.connect("PreActivated", aHolder, aSlotName);
        }
        private _emitPreActivated(aId: string) {
            this.emit.call(this, "PreActivated", aId);
        }
        connectActivated(aHolder: any, aSlotName: string, aSlot: (aId: string) => void) {
            this.connect("Activated", aHolder, aSlotName);
        }
        private _emitActivated(aId: string) {
            this.emit.call(this, "Activated", aId);
        }
        connectDeActivated(aHolder: any, aSlotName: string, aSlot: (aId: string) => void) {
            this.connect("DeActivated", aHolder, aSlotName);
        }
        private _emitDeActivated(aId: string) {
            this.emit.call(this, "DeActivated", aId);
        }
        connectSuspended(aHolder: any, aSlotName: string, aSlot: (aReason: string, aId: string) => void) {
            this.connect("Suspended", aHolder, aSlotName);
        }
        private _emitSuspended(aReason: string, aId: string) {
            this.emit.call(this, "Suspended", aReason, aId);
        }
        connectResumed(aHolder: any, aSlotName: string, aSlot: (aReason: string, aId: string) => void) {
            this.connect("Resumed", aHolder, aSlotName);
        }
        private _emitResumed(aReason: string, aId: string) {
            this.emit.call(this, "Resumed", aReason, aId);
        }
        connectCustomEvent(aHolder: any, aSlotName: string, aSlot: (aReqName: string, aParam: any) => void) {
            this.connect("CustomEvent", aHolder, aSlotName);
        }
        private _emitCustomEvent(aReqName: string, aParam: any) {
            this.emit.call(this, "CustomEvent", aReqName, aParam);
        }
        _requestCustomReq(aReqName: string, aParam: any) {
            this._emitCustomEvent(aReqName, aParam);
        }
    }

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

    export enum TFocusInfo {
        KFocusUnknown,
        KFocusNone,
        KFocusAble,
        KFocused
    }
    export enum TParamScrollScheme {
        EUnknown,
        EByItem,
        EByPage,
        EByFixedFocusRemains,
        EByVariableFocusRemains,
        EByFixed
    }
    export interface TSize {
        width: number;
        height: number;
    }
    export interface FChildViewMoved {
        (aControl: CControl, aIncrement: { top: number; left: number; } ): void;
    }
    export interface TContentAvail {
        up?: number;
        down?: number;
        left?: number;
        right?: number;
    }
    export interface FDataDrawer {
        (aKey: any, aItem: any, aEl: HTMLElement): TFocusInfo;
    }
    export interface FDataItemSelected {
        (aKey: any, aItem: any, aEl: HTMLElement): void;
    }
    export interface FFocusedDataItemChanged {
        (aKey: any, aItem: any, aEl: HTMLElement): void;
    }
    export interface FDataItemOrderChanged {
        (aItemKey: number, aItemMovedPosition: number, aDataProvider: CDataProvider): void;
    }
    export interface TFocusedInfo {
        el: HTMLElement;
        key: any;
        item: any;
    }

    export class CDrawnElements {
        _drawnElements: {} = {};
        constructor() {
        }
        destroy(aExcept: string[] = []) {
            this.removeAll(aExcept);
        }
        removeAll(aExcept: string[] = []) {
            var key, el: HTMLElement;
            for (key in this._drawnElements) {
                if (this._drawnElements.hasOwnProperty(key)) {
                    el = this._drawnElements[key];
                    Util.remove(el);
                }
            }
            this._drawnElements = {};
        }
        getCount(): number {
            return Object.keys(this._drawnElements).length;
        }
        getElements(): HTMLElement[] {
            var ret: HTMLElement[] = [], key;
            for (key in this._drawnElements) {
                if (this._drawnElements.hasOwnProperty(key)) {
                    ret.push(this._drawnElements[key]);
                }
            }
            return ret;
        }
        getElement(aKey: any): HTMLElement {
            return this._drawnElements[aKey];
        }
        setElement(aKey: any, aItem: HTMLElement) {
            this._drawnElements[aKey] = aItem;
        }
        pickElement(aKey: any): HTMLElement {
            var ret: HTMLElement = null;
            if (this._drawnElements.hasOwnProperty(aKey)) {
                ret = this._drawnElements[aKey];
                delete this._drawnElements[aKey];
            }
            return ret;
        }
        remove(aKey: any) {
            var el: HTMLElement = this._drawnElements[aKey];
            if (el) {
                Util.remove(el);
            }
            delete this._drawnElements[aKey];
        }
        forEach(fn: { (aKey: any, aItem: HTMLElement): boolean; }) {
            var key;
            for (key in this._drawnElements) {
                if (this._drawnElements.hasOwnProperty(key)) {
                    if (fn(key, this._drawnElements[key])) {
                        break;
                    }
                }
            }
        }
        setAnimation(aEnable: boolean) {
            if (aEnable) {
                this.forEach(function (aKey: any, aItem: HTMLElement) {
                    aItem.style[KCssPropTransition] = KCssTransitionParamPos;
                    //FIXME: dummy return
                    return true;
                });
            } else {
                this.forEach(function (aKey: any, aItem: HTMLElement) {
                    delete aItem.style[KCssPropTransition];
                    //FIXME: dummy return
                    return true;
                });
            }
        }
    }
    export class CDataProvider extends CSignalSource {
        constructor() {
            super();
            this.registerSignal(['ItemInserted', 'ItemUpdated', 'ItemMoved', 'ItemRemoved', 'ItemRefreshed']);
        }
        destroy() {
            super.destroy();
        }
        getItem(aKey: any): any {
            throw "not implemented";
            return null;
        }
        getLength(): number {
            throw "not implemented";
            return 0;
        }
        getCheckedItems(): any[] {
            return this._doGetCheckedItems();
        }
        deleteCheckedItem(): void {
            this._doDeleteCheckedItem();
        }
        setItemCheck(aIndex: number) {
            this._doItemCheck(aIndex);
        }
        setItemUnCheck(aIndex: number) {
            this._doItemUnCheck(aIndex);
        }
        setItemCheckClear() {
            this._doItemCheckClear();
        }
        isItemCheck(aIndex: number) {
            this._doIsItemCheck(aIndex);
        }
        _doDeleteCheckedItem: () => void;
        _doGetCheckedItems: () => any[];
        _doItemCheck: (aIndex: number) => void;
        _doItemUnCheck: (aIndex: number) => void;
        _doItemCheckClear: () => void;
        _doIsItemCheck: (aIndex) => boolean;
        insertItems(aKey: any, aItems: any[]) {
            this._doInsertItems(aKey, aItems);
            this.emit("ItemInserted", aKey, aItems);
        }
        _doInsertItems: (aKey: any, aItem: any[]) => void;
        removeItems(aKeys: any[]) {
            this._doRemoveItems(aKeys);
            this.emit("ItemRemoved", aKeys);
        }
        _doRemoveItems: (aKeys: any[]) => void;
        updateItems(aKeys: any[], aItems?: any[]) {
            this._doUpdateItems(aKeys, aItems);
            this.emit("ItemUpdated", aKeys, aItems);
        }
        _doUpdateItems: (aKey: any[], aItems?: any[]) => void;
        connectItemInserted(aHolder: any, aSlotName: string, aSlot: FItemInserted) {
            this.connect("ItemInserted", aHolder, aSlotName);
        }
        connectItemRemoved(aHolder: any, aSlotName: string, aSlot: FItemRemoved) {
            this.connect("ItemRemoved", aHolder, aSlotName);
        }
        connectItemUpdated(aHolder: any, aSlotName: string, aHandler: { (aKeys: any[], aItems?: any[]): void; }) {
            this.connect("ItemUpdated", aHolder, aSlotName);
        }
        sort(aSortEngine: (aData: any[]) => void) {
            if (aSortEngine) {
                if (typeof aSortEngine !== "function") {
                    throw "sort error argument type must function or undefined";
                }
            }
            this._sort(aSortEngine);
        }
        _sort(aSortEngine) {
        }
    }
    export class CListDataProvider extends CDataProvider {
        static KPropertyCheck: string = 'check_data_provider';
        static KMethodIsCheck: string = 'isCheck';
        private _itemsChecked: any[];
        _listData: any;
        _bDataRolling: boolean;
        constructor(aArray: any, aRolling?: boolean) {
            super();
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
        private _isItemCheck(aIndex: number) {
            var item = this.getItem(aIndex);
            var ret = false;
            if (item) {
                ret = !!item[CListDataProvider.KPropertyCheck];
            }
            return ret;
        }
        private _itemCheck(aIndex: number) {
            var item = this.getItem(aIndex);
            if (item) {
                item[CListDataProvider.KPropertyCheck] = true;
                this._itemsChecked.push(item);
            }
        }
        private _itemUnCheck(aIndex: number) {
            var item = this.getItem(aIndex);
            if (item) {
                item[CListDataProvider.KPropertyCheck] = false;
                this._itemsChecked.splice(this._itemsChecked.indexOf(item), 1);
            }
        }
        private _itemCheckClear() {
            var i, item, len = this.getLength();
            var fnClear = (aItem) => {
                if (aItem.hasOwnProperty(CListDataProvider.KPropertyCheck)) {
                    aItem[CListDataProvider.KPropertyCheck] = false;
                }
            };
            if (this._bDataRolling) {
                for (i = 0; i < len; i++) {
                    item = this._listData.at(i);
                    fnClear(item);
                }
            } else {
                for (i = 0; i < len; i++) {
                    item = this._listData[i];
                    fnClear(item);
                }
            }
            this._itemsChecked.length = 0;
        }
        private _getCheckedItems(): any {
            return this._itemsChecked;
        }
        private _deleteCheckedItem(): void {
            var i, list = this._listData, len = this.getLength();
            var aKeys: number[] = [];
            if (this._bDataRolling) {
                for (i = 0; i < len; i++) {
                    if (list.at(i)[CListDataProvider.KPropertyCheck]) {
                        aKeys.push(i);
                    }
                }
            } else {
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
        }
        private _getItem(aKey: number): any {
            return (this._bDataRolling) ? this._listData.at(aKey) : this._listData[aKey];
        }
        private _getLength(): number {
            return (this._bDataRolling) ? this._listData.length() : this._listData.length;
        }
        private _addCheckFn(aItem) {
            aItem[CListDataProvider.KMethodIsCheck] = function () {
                return !!this[CListDataProvider.KPropertyCheck];
            }
        }
        private _insertItems(aKey: number, aItems: any[]) {
            var list = this._listData;
            var i;
            for (i = 0; i < aItems.length; i++) {
                this._addCheckFn(aItems[i]);
            }
            if (this._bDataRolling) {
                list.appendArray(aItems);
            } else {
                if (aItems.length) {
                    var front, end;
                    front = list.slice(0, aKey);
                    end = list.slice(aKey);
                    this._listData = front.concat(aItems, end);
                } else {
                    list.splice(aKey, 0, aItems);
                }
            }
        }
        private _removeItems(aKey: number[]) {
            var i;
            aKey.sort(function(a: number, b: number) {
                return a < b ? -1 : a > b ? 1 : 0;
            });
            if (this._bDataRolling) {
                for (i = 0; i < aKey.length; i++) {
                    this._listData.del(aKey[i]);
                }
            } else {
                for (i = aKey.length - 1; i >= 0; i--) {
                    this._listData.splice(aKey[i], 1);
                }
            }
        }
        private _updateItems(aKey: number[], aItem?: any[]) {
            var i, len;

            if (aItem) {
                for (i = 0, len = aKey.length; i < len; i++) {
                    if (aItem[i]) {
                        this._listData[aKey[i]] = aItem[i];
                    }
                }
            }
        }
        getIndex(aKey: number): number {
            return (this._bDataRolling) ? this._listData.indexing(aKey) : aKey;
        }
        _sort(aSortEngine: (aData: any[]) => any[]) {
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
            } else {
                this._listData = sortedData;
            }
        }
    }
    export interface TDrawHint {
        index: number;
        span: {
            col: number;
            row: number;
        }
    }
    export class CDataControl extends CControl {
        _bDataRolling: boolean;
        _ownedDataProvider: CDataProvider;
        _drawer: FDataDrawer;
        _drawnElements: CDrawnElements;
        _prevDrawnElements: CDrawnElements;
        _drawerHint: TDrawHint[];
        _keyMapHint: any;
        private _redrawAfterOperation: boolean = false;

        constructor(aElement: HTMLElement) {
            //TODO: about jQuery
            super($(aElement));
            this._drawnElements = new CDrawnElements();
            this.registerSignal(["DataItemSelected", "FocusedDataItemChanged"]);
            this.registerSignal(["ItemInserted", "ItemRemoved"]);
            this.registerSignal(["DataItemOrderChanged"]);
        }
        destroy() {
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
        }
        draw(aRect?: TRect) {
            if (!this._drawer) {
                throw "data drawer must be provided to draw";
            }
            super.draw(aRect);
        }
        setDataDrawer(aDrawer: FDataDrawer) {
            this._drawer = aDrawer;
        }
        getFocusedItemInfo(): TFocusedInfo {
            var el: HTMLElement = this.getFocusedElement();
            var key, item;
            if (el) {
                key = el.attributes["data"];
                item = this._ownedDataProvider.getItem(key);
                return {
                    el: el,
                    key: key,
                    item: item
                }
            }
            return null;
        }
        setFocusedItem(aKey: string) {
            var el: HTMLElement = this._drawnElements.getElement(aKey);
            if (el) {
                this.setFocusedElement(el);
            }
        }
        getNextFocusIndex(aIncrementRow: number, aFocusRemain: boolean): number {
            throw "implement for scrolling!";
        }
        setDrawnHint(aHint: TDrawHint[]) {
            this._drawerHint = aHint;
        }
        getDrawerHint(aIndex: number): TDrawHint {
            var ret : TDrawHint = null;
            var i, len = this._drawerHint.length;
            for(i=0; i<len; i++) {
                if(this._drawerHint[i].index === aIndex) {
                    ret = this._drawerHint[i];
                    break;
                }
            }
            return ret;
        }
        doItemChagned(aKeys: any[]) {
        }
        doItemInserted(aKey: any, aItems: any[], aNeedFocus?: boolean) {
        }
        doItemRemoved(aKey: number, aUnsetFocus?: boolean) {
        }
        setRedrawAfterOperation (aRedraw: boolean) {
            this._redrawAfterOperation = aRedraw;
        }
        setOwnedDataProvider(dataProvider: CDataProvider) {
            this._keyMapHint = null;
            if (this._drawnElements) {
                this._drawnElements.removeAll();
            }
            if (this._ownedDataProvider) {
                this._ownedDataProvider.destroy();
            }
            this._ownedDataProvider = dataProvider;
        }
        getOwnedDataProvider(): CDataProvider {
            return this._ownedDataProvider;
        }
        clearDrawnElements() {
            this._clearDrawnElements();
        }
        _clearDrawnElements() {
            if (this._drawnElements) {
                this._drawnElements.removeAll();
            }
        }
        _setDrawnElement(aKey: any, el: HTMLElement) {
            if (this._drawnElements) {
                this._drawnElements.setElement(aKey, el);
            }
        }
        _getDrawnElement(aKey: any): HTMLElement {
            if (this._drawnElements) {
                return this._drawnElements.getElement(aKey);
            }
            return;
        }
        _getDrawnElements(): HTMLElement[]{
            if (this._drawnElements) {
                return this._drawnElements.getElements();
            }
            return;
        }
        _removeDrawnElement(aKey: any) {
            if (this._drawnElements) {
                this._drawnElements.remove(aKey);
            }
            if (this._prevDrawnElements) {
                this._prevDrawnElements.remove(aKey);
            }
        }
        _pickDrawnElements() {
            var ret = this._drawnElements;
            this._drawnElements = new CDrawnElements();
            return ret;
        }
        _setPrevDrawnElements(aDrawnElements: CDrawnElements) {
            if (this._prevDrawnElements) {
                this._prevDrawnElements.destroy();
            }
            aDrawnElements.forEach(function (aKey: any, aEl: HTMLElement) {
                aEl.classList.remove(TClassStr.KClassActiveFocusedLeaf);
                aEl.classList.remove(TClassStr.KClassFocused);
                //FIXME: dummy return
                return true;
            });
            this._prevDrawnElements = aDrawnElements;
        }

        _handleFocusChanged(aElOld: HTMLElement, aElNew: HTMLElement) {
            //FIXME
            super._handleFocusChanged(aElOld, aElNew);
            var key = aElNew.attributes["data"];
            var item = this._ownedDataProvider.getItem(key);
            this._emitFocusedDataItemChanged(key, item, aElNew);
        }
        /*
         Signals
         */
        _doKeyEnterLatent() {
            //FIXME
//            super._doKeyEnterLatent();
            var focusedInfo = this.getFocusedItemInfo();
            return this._emitDataItemSelected(focusedInfo.key, focusedInfo.item, focusedInfo.el);
        }
        connectDataItemSelected(aHolder: any, aSlotName: string, aSlot: FDataItemSelected) {
            this.connect("DataItemSelected", aHolder, aSlotName);
        }
        private _emitDataItemSelected(aKey: any, aItem: any, aEl: HTMLElement): boolean {
            return this.emit.call(this, "DataItemSelected", aKey, aItem, aEl);
        }
        connectFocusedDataItemChanged(aHolder: any, aSlotName: string, aSlot: FFocusedDataItemChanged) {
            this.connect("FocusedDataItemChanged", aHolder, aSlotName);
        }
        private _emitFocusedDataItemChanged(aKey: any, aItem: any, aEl: HTMLElement) {
            this.emit.call(this, "FocusedDataItemChanged", aKey, aItem, aEl);
        }
        connectDataItemOrderChanged(aHolder: any, aSlotName: string, aSlot: FDataItemOrderChanged) {
            this.connect("DataItemOrderChanged", aHolder, aSlotName);
        }
        protected _emitDataItemOrderChanged(aItemKey: number, aItemMovedPosition: number, aDataProvider: CDataProvider) {
            this.emit.call(this, "DataItemOrderChanged", aItemKey, aItemMovedPosition, aDataProvider);
        }
        // avail
        _contentAvail: TContentAvail;
        getContentAvail(): TContentAvail {
            return this._contentAvail;
        }
        setContentAvail(aContentAvail: TContentAvail) {
            this._contentAvail = aContentAvail;
        }
        getCustomScrollSize(aKeyStr:string, aDrawnRect: TRect): TContentAvail {
            // todo child control impliment
            return null;
        }
        getCustomScrollSizeByIndex(aIndex: number): {top: number; left: number} {
            // todo child control impliment
            return null;
        }
        setDataRolling(aRolling: boolean) {
            this._bDataRolling = aRolling;
        }
        getDataRolling() {
            return this._bDataRolling;
        }
        _doUpdate(aKeepFocus: boolean = false) {
            super._doUpdate(aKeepFocus);
            if (aKeepFocus) {
                this._pickDrawnElements().destroy();
            }
        }
        prependItems(aItems: any[]) {
            if (this._ownedDataProvider) {
                this._ownedDataProvider.insertItems(0, aItems);
            }
        }
        insertItems(position: number, aItems: any[]) {
            if (this._ownedDataProvider) {
                this._ownedDataProvider.insertItems(position, aItems);
            }
        }
        appendItems(aItems: any[]) {
            if (this._ownedDataProvider) {
                this._ownedDataProvider.insertItems(this._ownedDataProvider.getLength(), aItems);
            }
        }
        updateItems(aKeys: number[], aItems?: any[]) {
            this.setKeepFocus(true);
            if (this._ownedDataProvider) {
                this._ownedDataProvider.updateItems(aKeys, aItems);
            }
        }
        removeItems(index: number[]) {
            this.setKeepFocus(true);
            if (this._ownedDataProvider) {
                this._ownedDataProvider.removeItems(index);
            }
        }
        getItemLength() {
            if (this._ownedDataProvider) {
                return this._ownedDataProvider.getLength();
            }
            return;
        }
        doSelect(focusedChild) {
            var itemInfo = this.getFocusedItemInfo();
            if (itemInfo) {
                return this._emitDataItemSelected(itemInfo.key, itemInfo.item, itemInfo.el);
            } else {
                return false;
            }
        }
        getCheckedItems() {
            if (this._ownedDataProvider) {
                return this._ownedDataProvider.getCheckedItems();
            }
            return;
        }
        deleteCheckedItem() {
            this.setKeepFocus(true);
            if (this._ownedDataProvider) {
                this._ownedDataProvider.deleteCheckedItem();
            }
        }
        setItemCheck(aIndex: number) {
            this.setKeepFocus(true);
            if (this._ownedDataProvider) {
                this._ownedDataProvider.setItemCheck(aIndex);
            }
        }
        setItemUnCheck(aIndex: number) {
            this.setKeepFocus(true);
            if (this._ownedDataProvider) {
                this._ownedDataProvider.setItemUnCheck(aIndex);
            }
        }
        setItemCheckClear() {
            this.setKeepFocus(true);
            if (this._ownedDataProvider) {
                this._ownedDataProvider.setItemCheckClear();
            }
        }
        isItemCheck(aIndex: number) {
            if (this._ownedDataProvider) {
                this._ownedDataProvider.isItemCheck(aIndex);
            }
        }
    }
    export class CListDataControl extends CDataControl {
        private _fnEndTransition: Function;
        private _stackMove = null;
        constructor(aElement: HTMLElement, aRolling?: boolean) {
            super(aElement);
            this._bDataRolling = aRolling;
            this._element.classList.add("-list");
        }
        endTransition() {
            this._fnEndTransition();
        }
        getNextFocusIndex(aIncrementRow: number, aFocusRemain: boolean) {
            if(aFocusRemain) {
                return aIncrementRow > 0 ? this.getFocusedElementIndex() + aIncrementRow : this.getFocusedElementIndex() + aIncrementRow - 2;
            } else {
                return aIncrementRow > 0 ? this.getFocusedElementIndex() + aIncrementRow - 1 : this.getFocusedElementIndex() + aIncrementRow + 1;
            }
        }
        doItemInserted (aKey: any, aItems: any[], aNeedFocus?: boolean) {
            this.emit.call(this, "ItemInserted", this._drawnElements, aNeedFocus);
        }
        doItemRemoved(aKey: number, aUnsetFocus?: boolean) {
            this.emit.call(this, "ItemRemoved", this._drawnElements, aUnsetFocus);
        }
        getSize(aPageSize?: TSize): TSize {
            var itemHeight: number = this._getDrawParam(KParamStrItemHeight) || 0;
            var itemWidth: number = this._getDrawParam(KParamStrItemWidth) || 0;
            var horizontal: boolean = (this._getDrawParam(KParamStrOrientation) === TParamOrientation.EHorizontal);
            var count = this._ownedDataProvider ? this._ownedDataProvider.getLength() : 0;
            var w: number;
            var h: number;

            w = horizontal ? itemWidth * count : itemWidth;
            h = horizontal ? itemHeight : itemHeight * count;
            if (aPageSize && this._bDataRolling) {
                var countPage: number;
                var page: number = 0;
                if (!horizontal) {
                    var containerHeight = aPageSize.height;
                    countPage = containerHeight / itemHeight;
                    if (count <= countPage) {
                        page = 1;
                    } else {
                        page = Math.floor(count / (countPage - 1));
                        page += count % (countPage - 1) ? 1 : 0
                    }
                    if (page > 0) {
                        h = containerHeight;
                        page -= 1;
                        if (page > 0) {
                            h += (containerHeight - itemHeight) * page;
                        }
                    }
                } else {
                    //TODO: is horizontal page scroll?
                    var containerWidth = aPageSize.width;
                }
            }

            return {
                width: w,
                height: h
            }
        }
        _doDraw(aRect: TRect, aDrawParam: { [key: string]: any; }) {
            var focusableElements: HTMLElement[] = [];
            var horizontal: boolean = (aDrawParam[KParamStrOrientation] === TParamOrientation.EHorizontal);
            var dp: CListDataProvider = <CListDataProvider>this._ownedDataProvider;
            var fixedItemSize: number = horizontal ? aDrawParam[KParamStrItemWidth] : aDrawParam[KParamStrItemHeight];
            var drawPosStart: number = horizontal ? aRect.left : aRect.top;
            var drawPosEnd: number = horizontal ? aRect.right : aRect.bottom;
            var drawSize: number = horizontal ? aRect.getHeight() : aRect.getWidth();
            var posProp: string = horizontal ? "left" : "top";
            var posPropPair: string = horizontal ? "right" : "bottom";
            var startIndex: number = drawSize ? Math.floor(drawPosStart / fixedItemSize) : 0;
            var itemCount: number = this._bDataRolling ? startIndex + dp.getLength() : dp.getLength();
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
            this._keyMapBuilder = horizontal ? KBuilderLeftRight : KBuilderTopDown;
            var compensation = 0;
            if (!horizontal && aDrawParam[KParamStrScrollSchemeVertical] === CtrlBase.TParamScrollScheme.EByFixedFocusRemains) {
                if (this._bDataRolling) {
                    itemCount = dp.getLength();
                    if (aRect.bottom < 0) {
                        startIndex -= startIndex;
                    }
                } else if (aRect.top < 0) {
                    if (!this._bDataRolling) {
                        drawPosStart -= drawPosEnd;
                        drawPosEnd = 0;
                        if (startIndex < 0 && Math.abs(startIndex)) {
                            compensation  = dp.getLength() % (countItemOnPage - 1) - 1;
                            startIndex += compensation;
                            this.setKeepFocus(false);
                        }
                    } else {
                        startIndex -= startIndex;
                    }
                }
            }
            for (i = startIndex; i < itemCount; i++) {
                var elPosStart = i * fixedItemSize;
                var elPosEnd = elPosStart + fixedItemSize;
                var index = dp.getIndex(i);
                if (elPosStart < drawPosEnd) { // inbound
                    // draw element if necessary
                    var el: HTMLElement = prevDrawnItem.pickElement(index);
                    var item: any = dp.getItem(index);
                    if (!el) {
                        el = document.createElement("div");
                        el.classList.add("-list-item");
                        el.attributes["data"] = index;
                        el.style.position = "absolute";
                        el.style[posProp] = (i - compensation) * fixedItemSize + "px";
                        el.style[posPropPair] = parseInt(el.style[posProp]) + fixedItemSize + "px";

                        var focusInfo: TFocusInfo = this._drawer(index, item, el);
                        if (focusInfo === TFocusInfo.KFocusAble) {
                            el.classList.add(TClassStr.KClassFocusable);
                        }
                        if (focusInfo !== TFocusInfo.KFocusUnknown) {
                            this._element.appendChild(el);
                        }
                    }
                    this._setDrawnElement(index, el);
                    if (drawPosStart <= elPosStart && elPosEnd <= drawPosEnd && el.classList.contains(TClassStr.KClassFocusable)) {
                        focusableElements.push(el);
                    }

                    // check content available
//                    if (elPosStart < drawPosStart) {
//                        if (horizontal) {
//                            contentAvailable.left = drawPosStart - elPosStart;
//                        } else {
//                            contentAvailable.up = drawPosStart - elPosStart;
//                        }
//                    }
//                    if (drawPosEnd < elPosEnd) {
//                        if (horizontal) {
//                            contentAvailable.right = elPosEnd - drawPosEnd;
//                        } else {
//                            contentAvailable.down = elPosEnd - drawPosEnd;
//                        }
//                    }
                }
            }
            if (!this.getAnimation()) {
                prevDrawnItem.destroy();
            } else {
                prevDrawnItem.forEach((aKey: any, aItem: HTMLElement) => {
                    var ret = false;
                    if (aItem.classList.contains(CtrlBase.TClassStr.KClassActiveFocusedLeaf)) {
                        aItem.classList.remove(CtrlBase.TClassStr.KClassActiveFocusedLeaf);
                        ret = true;
                    }
                    return ret;
                });
                this._fnEndTransition = () => {
                    prevDrawnItem.destroy();
                }
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
            if(itemCount) {
                var size = this.getSize();
                var first = focusableElements[0];
                var last = focusableElements[focusableElements.length - 1];
                if (first) {
                    if(horizontal) {
                        var width = first.offsetWidth;
                        var left = first.offsetLeft;
                        var right = last.offsetLeft + width;
                        contentAvailable.left = left;
                        contentAvailable.right = size.width - right;
                        if (contentAvailable.right < 0) {
                            contentAvailable.right = 0;
                        }
                    } else {
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
        }
        setAnimation(aAnimation: boolean) {
            super.setAnimation(aAnimation);
        }
        setModeMove(aSet: boolean) {
            var keyMap = this._keyMap;
            var isDirty = keyMap.setMode(aSet ? eModeKeyMap.MOVE : eModeKeyMap.NORMAL)
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
                } else {
                    for (var i = 0; i < len; i++) {
                        if (stack[i] < updatedKey) {
                            stack[i]++;
                        }
                    }
                }
                stack[key] = posMoved;
            }
        }
    }
    export class CGridDataControl extends CDataControl {
        private _fnEndTransition: Function;
        private _nextFocusItem: HTMLElement;
        //private _arrayBlock: Array<Array>;
        _keyMapHint: any;
        constructor(aElement: HTMLElement) {
            super(aElement);
            this._element.classList.add("-grid");
        }
        _doGetKeyMapHint() {
            var rowCount = this.getMaxRowCount();
            if (rowCount > 0) {
                // horizontal
                return rowCount;
            }
            return this._keyMapHint;
        }
        getRowElement(aRow:number, bDuplicate:boolean) {
            var ret: HTMLElement[];
            var len = this._focusableElement.length;
            var startData = parseInt(this._focusableElement[0].getAttribute('data'));
            var endData = parseInt(this._focusableElement[len-1].getAttribute('data'));
            var i;
            if(this._drawerHint) {
                var startPosition = this.getPositionFromHintArray(startData);
                var endPosition = this.getPositionFromHintArray(endData);
                if(aRow >= startPosition.row && aRow <= endPosition.row) {
                    var hintArray = this._getHintArray();
                    var rowArray = hintArray[aRow];
                    var rowLen = rowArray.length;
                    var j;
                    for(i=0; i<rowLen; i++) {
                        for(j=0; j<len; j++) {
                            var data = parseInt(this._focusableElement[j].getAttribute('data'));
                            if(data === rowArray[i]) {
                                if(ret) {
                                    if(bDuplicate || ret.indexOf(this._focusableElement[j]) === -1) {
                                        ret.push(this._focusableElement[j]);
                                    }
                                } else {
                                    ret = [this._focusableElement[j]];
                                }
                                break;
                            }
                        }
                    }
                }
            } else {
                var maxCol:number = this._getDrawParam(KParamStrMaxColCount);
                var startRow = Math.floor(startData/maxCol);
                var endRow = Math.floor(endData/maxCol);
                if(aRow >= startRow&& aRow <= endRow) {
                    for(i=0; i<len; i++) {
                        var data = parseInt(this._focusableElement[i].getAttribute('data'));
                        var row = Math.floor(data/maxCol);
                        if(aRow === row) {
                            if(ret) {
                                if(ret.indexOf(this._focusableElement[i]) === -1) {
                                    ret.push(this._focusableElement[i]);
                                }
                            } else {
                                ret = [this._focusableElement[i]];
                            }
                        }
                    }
                }
            }
            return ret;
        }
        getColElement(aCol:number, bDuplicate:boolean) {
            var ret: HTMLElement[];
            var len = this._focusableElement.length;
            var startData = parseInt(this._focusableElement[0].getAttribute('data'));
            var endData = parseInt(this._focusableElement[len-1].getAttribute('data'));
            var maxCol:number = this._getDrawParam(KParamStrMaxColCount);
            var i;
            if(aCol <= maxCol) {
                if(this._drawerHint) {
                    var startPosition = this.getPositionFromHintArray(startData);
                    var endPosition = this.getPositionFromHintArray(endData);
                    var hintArray = this._getHintArray();
                    var rowLen = hintArray.length;
                    var j;
                    for(i=0; i<rowLen; i++) {
                        if(i >= startPosition.row && i <= endPosition.row) {
                            if(hintArray[i][aCol] !== undefined && hintArray[i][aCol] !== null) {
                                for(j=0; j<len; j++) {
                                    var data = parseInt(this._focusableElement[j].getAttribute('data'));
                                    if(hintArray[i][aCol] === data) {
                                        if(ret) {
                                            if(bDuplicate || ret.indexOf(this._focusableElement[j]) === -1) {
                                                ret.push(this._focusableElement[j]);
                                            }
                                        } else {
                                            ret = [this._focusableElement[j]];
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    var maxCol:number = this._getDrawParam(KParamStrMaxColCount);
                    var startRow = Math.floor(startData/maxCol);
                    var endRow = Math.floor(endData/maxCol);
                    for(i=0; i<len; i++) {
                        var data = parseInt(this._focusableElement[i].getAttribute('data'));
                        var col = data % maxCol;
                        if(col === aCol) {
                            if(ret) {
                                if(ret.indexOf(this._focusableElement[i]) === -1) {
                                    ret.push(this._focusableElement[i]);
                                }
                            } else {
                                ret = [this._focusableElement[i]];
                            }
                        }
                    }
                }
            }
            return ret;
        }
        getNextFocusIndex(aIncrementRow: number, aFocusRemain: boolean) {
            var focusableElementCount = this.getFocusableElementCount();
            var currentIndex = this.getFocusedElementIndex();
            var maxColCount: number = this._getDrawParam(KParamStrMaxColCount) || 1;
            var currentRow, currentCol, nextRow;
            var arrayBlock = this._getHintArray();
            var nextIndex: number;
            if(this._nextFocusItem) {
                nextIndex =  parseInt(this._nextFocusItem.getAttribute('data'));
                this._nextFocusItem = null;
            } else {
                if(this._drawerHint) {
                    var position = this.getPositionFromHintArray(currentIndex);
                    if(position) {
                        currentCol = position.col;
                        currentRow = position.row;
                        if(aFocusRemain) {
                            nextRow = aIncrementRow > 0 ? currentRow - aIncrementRow + 1 : currentRow - aIncrementRow - 1;
                        } else {
                            nextRow = aIncrementRow > 0 ? currentRow - aIncrementRow : currentRow - aIncrementRow;
                        }
                        if(nextRow < 0) {
                            nextRow = 0;
                        } else if(nextRow >= arrayBlock.length) {
                            nextRow = arrayBlock.length - 1;
                        }
                        nextIndex = arrayBlock[nextRow][currentCol];
                        if(nextIndex === undefined) {
                            nextIndex = this.getItemLength() - 1;
                        } else if(nextIndex === currentIndex) {
                            currentRow = nextRow;
                            if(aFocusRemain) {
                                nextRow = aIncrementRow > 0 ? currentRow - aIncrementRow + 1 : currentRow - aIncrementRow - 1;
                            } else {
                                nextRow = aIncrementRow > 0 ? currentRow - aIncrementRow : currentRow - aIncrementRow;
                            }
                            if(nextRow < 0) {
                                nextRow = 0;
                            } else if(nextRow >= arrayBlock.length) {
                                nextRow = arrayBlock.length - 1;
                            }
                            nextIndex = arrayBlock[nextRow][currentCol];
                        }
                    }
                } else {
                    currentRow = Math.ceil((currentIndex + 1) / maxColCount);
                    currentCol = currentIndex % maxColCount;
                    if(aFocusRemain) {
                        nextRow = currentRow + aIncrementRow;
                    } else {
                        nextRow = aIncrementRow > 0 ? currentRow + aIncrementRow - 1 : currentRow + aIncrementRow + 1;
                    }
                    var itemLength = this.getItemLength();
                    var maxRow = Math.ceil(itemLength / maxColCount);
                    if(nextRow < 0) {
                        nextRow = 0;
                    } else if(nextRow > maxRow) {
                        nextRow = maxRow;
                    }
                    nextIndex = (nextRow - 1) * maxColCount + currentCol;
                }
            }
            return nextIndex;
        }
        getCustomScrollSize(aKeyStr:string, aDrawnRect: TRect): TContentAvail {
            var focusedItem = this.getFocusedElement();
            var position = this.getPositionFromHintArray(parseInt(focusedItem.getAttribute('data')));
            var ret = false;
            var nextItemContentAvail: TContentAvail  = {
                up: 0,
                left: 0,
                right: 0,
                down: 0
            };
            if(position) {
                switch(aKeyStr) {
                    case KKeyStrUp:
                        if(position.row > 0) {
                            var nextItemIndex = this._keyMapHint[position.row-1][position.col];
                            var hint = this.getDrawerHint(nextItemIndex);
                            if(hint) {
                                this._nextFocusItem = this._getDrawnElement(nextItemIndex);
                                if(this._nextFocusItem) {
                                    nextItemContentAvail.up = aDrawnRect.top - this._nextFocusItem.offsetTop;
                                } else {
                                    nextItemContentAvail.up = hint.span.row * this.getItemHeight();
                                }
                                ret = true;
                            }
                        }
                        break;
                    case KKeyStrDown:
                        if(position.row < this._keyMapHint.length - 1) {
                            var nextItemIndex = this._keyMapHint[position.row+1][position.col];
                            var hint = this.getDrawerHint(nextItemIndex);
                            if(hint) {
                                this._nextFocusItem = this._getDrawnElement(nextItemIndex);
                                if(this._nextFocusItem) {
                                    nextItemContentAvail.down = this._nextFocusItem.offsetTop + this._nextFocusItem.offsetHeight - aDrawnRect.bottom;
                                } else {
                                    nextItemContentAvail.down = hint.span.row * this.getItemHeight();
                                }
                                ret = true;
                            }
                        }
                        break;
                    case KKeyStrLeft:
                        if(position.col > 0) {
                            this._nextFocusItem = this._getDrawnElement(this._keyMapHint[position.row][position.col-1]);
                            if(this._nextFocusItem) {
                                if(this._nextFocusItem.offsetTop < focusedItem.offsetTop) {
                                    nextItemContentAvail.up = focusedItem.offsetTop - this._nextFocusItem.offsetTop;
                                } else if(this._nextFocusItem.offsetTop + this._nextFocusItem.offsetHeight > focusedItem.offsetTop + focusedItem.offsetHeight) {
                                    nextItemContentAvail.down = this._nextFocusItem.offsetTop + this._nextFocusItem.offsetHeight - focusedItem.offsetTop - focusedItem.offsetHeight
                                }
                            }
                            ret = true;
                        }
                        break;
                    case KKeyStrRight:
                        if(position.col < this._keyMapHint[position.row].length - 1) {
                            this._nextFocusItem = this._getDrawnElement(this._keyMapHint[position.row][position.col+1]);
                            if(this._nextFocusItem) {
                                if(this._nextFocusItem.offsetTop < focusedItem.offsetTop) {
                                    nextItemContentAvail.up = focusedItem.offsetTop - this._nextFocusItem.offsetTop;
                                } else if(this._nextFocusItem.offsetTop + this._nextFocusItem.offsetHeight > focusedItem.offsetTop + focusedItem.offsetHeight) {
                                    nextItemContentAvail.down = this._nextFocusItem.offsetTop + this._nextFocusItem.offsetHeight - focusedItem.offsetTop - focusedItem.offsetHeight
                                }
                                ret = true;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            if(ret) {
                return nextItemContentAvail;
            } else {
                return null
            }
        }
        getCustomScrollSizeByIndex(aIndex: number) {
            var ret = {
                top: 0,
                left: 0
            };
            var focusedItem = this.getFocusedElement();
            var curIndex = parseInt(focusedItem.getAttribute('data'));
            if(this._keyMapHint) {
                var curPosition = this.getPositionFromHintArray(curIndex);
                var nextPosition = this.getPositionFromHintArray(aIndex);

                if(this._getDrawParam(KParamStrOrientation) === TParamOrientation.EVertical) {
                    var itemHeight: number = this.getItemHeight();
                    ret.top = (curPosition.row - nextPosition.row) * itemHeight;
                }
            } else {
                if(this._getDrawParam(KParamStrOrientation) === TParamOrientation.EVertical) {
                    var maxCol = this._getDrawParam(KParamStrMaxColCount);
                    var curRow = Math.floor(curIndex / maxCol);
                    var nextRow = Math.floor(aIndex / maxCol);
                    var itemHeight: number = this.getItemHeight();
                    ret.top = (curRow - nextRow) * itemHeight;
                }
            }
            return ret;
        }
        endTransition() {
            this._fnEndTransition();
        }
        getSize(aPageSize?: TSize): TSize {
            var horizontal: boolean = (this._getDrawParam(KParamStrOrientation) === TParamOrientation.EHorizontal);
            var itemHeight: number = this._getDrawParam(KParamStrItemHeight) || 0;
            var itemWidth: number = this._getDrawParam(KParamStrItemWidth) || 0;
            var maxRowCount: number = this._getDrawParam(KParamStrMaxRowCount) || 1;
            var maxColCount: number = this._getDrawParam(KParamStrMaxColCount) || 1;
            var count = this._ownedDataProvider.getLength();
            var w = itemWidth * maxColCount;
            var h;
            if(this._drawerHint) {
                var arrayBlock = this._getHintArray();
                h = itemHeight * arrayBlock.length;
            } else {
                if (horizontal) {
                    w = itemWidth * Math.ceil(count / maxRowCount);
                    h = itemHeight * maxRowCount;
                } else {
                    h = itemHeight * Math.ceil(count / maxColCount);
                }
            }
            return {
                width: w,
                height: h
            }
        }
        _getHintArray(): Array<any[]> {
            if(!this._keyMapHint && this._drawerHint) {
                var i, j;
                var arrayBlock: Array<any[]> = new Array(new Array(3));
                var arrayRowIndex = 0;
                var arrayColIndex = 0;
                var itemIndex = 0;
                var itemCount: number = this._ownedDataProvider.getLength();
                var maxColCount: number =  this._getDrawParam(KParamStrMaxColCount);
                for(i=0; itemIndex<itemCount; i++) {
                    if(arrayColIndex >= maxColCount) {
                        arrayColIndex = 0;
                        arrayRowIndex += 1;
                        if(!arrayBlock[arrayRowIndex]) {
                            arrayBlock.push(new Array(3));
                        }
                    }
                    var hint: TDrawHint = this.getDrawerHint(itemIndex);
                    if(hint) {
                        var rowSpan = hint.span.row;
                        var colSpan = hint.span.col;
                        var r,c;
                        var tempColIndex = arrayColIndex;
                        for(r=0; r<rowSpan; r++) {
                            for(c=0; c<colSpan; c++) {
                                arrayBlock[arrayRowIndex+r][arrayColIndex] = itemIndex;
                                arrayColIndex += 1;
                            }
                            arrayColIndex = tempColIndex;
                            if(r !== rowSpan-1) {
                                arrayBlock.push(new Array(3));
                            }
                        }
                        itemIndex += 1;
                        arrayColIndex = tempColIndex + colSpan;
                    } else {
                        if(arrayBlock[arrayRowIndex][arrayColIndex] === undefined) {
                            arrayBlock[arrayRowIndex][arrayColIndex] = itemIndex++;
                            arrayColIndex += 1;
                        } else {
                            arrayColIndex += 1;
                            continue;
                        }
                    }
                }
                this._keyMapHint = arrayBlock;
            }
            return this._keyMapHint;
        }
        _doDrawForSpan(aRect: TRect, aDrawParam: { [key: string]: any; }) {
            var focusableElements: HTMLElement[] = [];
            var dp: CDataProvider = this._ownedDataProvider;
            var itemCount: number = dp.getLength();
            var itemWidth: number = aDrawParam[KParamStrItemWidth];
            var itemHeight: number = aDrawParam[KParamStrItemHeight];
            var maxColCount: number = aDrawParam[KParamStrMaxColCount];
            var horizontal: boolean = (aDrawParam[KParamStrOrientation] === TParamOrientation.EHorizontal);
            var startIndex: number = aDrawParam[KParamStrStartIndex];
            var prevDrawnItem = this._pickDrawnElements();

            var contentAvailable = {
                up: 0,
                left: 0,
                right: 0,
                down: 0
            }

            this._keyMapBuilder = KBuilderGrid;
            var arrayBlock = this._getHintArray();
            var curRow = Math.floor(aRect.top/itemHeight);
            var curCol = 0;
            var lastRow = Math.ceil(aRect.bottom/itemHeight);
            var rowLen = arrayBlock.length;
            var itemIndex = arrayBlock[curRow][curCol];
            var i, j;
            for(i=0; i<rowLen; i++) {
                var rowBlock: any[] = arrayBlock[i];
                for(j=0; j<rowBlock.length; j++) {
                    if(i >= curRow && i <= lastRow) {
                        var index = arrayBlock[i][j];
                        if(index !== undefined && !this._getDrawnElement(index)) {
                            var hint: TDrawHint = this.getDrawerHint(index);
                            var el: HTMLElement = prevDrawnItem.pickElement(index);
                            var elTop = i * itemHeight;
                            var elLeft = j * itemWidth;
                            var elRight, elBottom;
                            var isNewEl = false;
                            if(!el) {
                                el = document.createElement('div');
                                el.setAttribute('data', index);
                                el.attributes['data'] = index;
                                el.classList.add('-grid-item');
                                el.style.position = 'absolute';
                                if(hint) {
                                    var position = this.getPositionFromHintArray(index);
                                    if(position.row !== i || position.col !== j) {
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
                                } else{
                                    el.style.top = elTop + 'px';
                                    el.style.left = elLeft + 'px';
                                    el.style.width = itemWidth + 'px';
                                    el.style.height = itemHeight + 'px';
                                    elRight = elLeft + itemWidth;
                                    elBottom = elTop + itemHeight;
                                }
                                isNewEl = true;
                            } else {
                                elTop = parseInt(el.style.top);
                                elLeft = parseInt(el.style.left);
                                elRight = elLeft + parseInt(el.style.width);
                                elBottom = elTop + parseInt(el.style.height);
                            }
                            // TO Draw
                            if(elTop < aRect.bottom && elBottom > aRect.top && elLeft < aRect.right && elRight > aRect.left) {
                                if(isNewEl) {
                                    var item: any = dp.getItem(index);
                                    var focusInfo: TFocusInfo = this._drawer(index, item, el);
                                    this._element.appendChild(el);
                                } else {
                                    if(el.classList.contains(KClassActiveFocusedLeaf)) {
                                        el.classList.remove(KClassActiveFocusedLeaf);
                                    }
//                                    if(el.classList.contains(KClassFocused)) {
//                                        el.classList.remove(KClassFocused);
//                                    }
                                }
                                this._setDrawnElement(index, el);

                                // TO Focus
                                if(elTop >= aRect.top && elBottom <= aRect.bottom && elLeft >= aRect.left && elRight <= aRect.right) {
                                    if(startIndex === index) {
                                        el.classList.add(KClassFocused);
                                    }
                                    el.classList.add(KClassFocusable);
                                    focusableElements.push(el);
                                } else if(el.classList.contains(KClassFocusable)) {
                                    el.classList.remove(KClassFocusable);
                                }
                            } else {
                                prevDrawnItem.setElement(index, el);
                            }
//                            itemIndex ++;
                        }
                    }
                }
            }
            if (!this.getAnimation()) {
                prevDrawnItem.destroy();
            } else {
                prevDrawnItem.forEach((aKey: any, aItem: HTMLElement) => {
                    var ret = false;
                    if (aItem.classList.contains(CtrlBase.TClassStr.KClassActiveFocusedLeaf)) {
                        aItem.classList.remove(CtrlBase.TClassStr.KClassActiveFocusedLeaf);
                        ret = true;
                    }
                    return ret;
                });
                this._fnEndTransition = () => {
                    prevDrawnItem.destroy();
                }
            }
            if(itemCount) {
                var size = this.getSize();
                var first = focusableElements[0];
                var last = focusableElements[focusableElements.length - 1];
                if(horizontal) {
                    var left = first.offsetTop - contentAvailable.left;
                    var right = last.offsetTop + last.offsetHeight + contentAvailable.right;
                    contentAvailable.left = left;
                    contentAvailable.right = size.width - right;
                } else {
                    if(size.height > aRect.getHeight()) {
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
        }
        _doDraw(aRect: TRect, aDrawParam: { [key: string]: any; }) {
            if(this._drawerHint) {
                return this._doDrawForSpan(aRect, aDrawParam);
            } else {
                var focusableElements: HTMLElement[] = [];
                var dp: CDataProvider = this._ownedDataProvider;
                var itemCount: number = dp.getLength();
                var horizontal: boolean = (aDrawParam[KParamStrOrientation] === TParamOrientation.EHorizontal);
                var itemWidth: number = aDrawParam[KParamStrItemWidth];
                var itemHeight: number = aDrawParam[KParamStrItemHeight];

                var maxColCount: number = aDrawParam[KParamStrMaxColCount];
                var maxRowCount: number = aDrawParam[KParamStrMaxRowCount];

                var curCol: number;
                var curRow: number;
                var startIndex: number;
                var startFocusIndex: number;
                curCol = aRect ? Math.floor(aRect.left / itemWidth) : 0;
                curRow = aRect ? Math.floor(aRect.top / itemHeight) : 0;
                if (horizontal) {
                    startIndex = aRect ? curCol * maxRowCount + curRow : 0;
                } else {
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
                    this._keyMapBuilder = KBuilderGridHorizontal;
                } else {
                    this._keyMapBuilder = KBuilderGrid;
                }
                for (i = startIndex; i < itemCount; i++) {
                    var colCount;
                    var rowCount;
                    if (horizontal) {
                        rowCount = i % maxRowCount;
                        colCount = Math.floor(i / maxRowCount);
                    } else {
                        colCount = i % maxColCount;
                        rowCount = Math.floor(i / maxColCount);
                    }
                    var elTop = rowCount * itemHeight;
                    var elLeft = colCount * itemWidth;
                    var elBottom = elTop + itemHeight;
                    var elRight = elLeft + itemWidth;
                    if (elTop < aRect.bottom && elLeft < aRect.right) {
                        var el: HTMLElement = prevDrawnItem.pickElement(i);
                        var item: any = dp.getItem(i);
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
                            var focusInfo: TFocusInfo = this._drawer(i, item, el);
                            if (focusInfo === TFocusInfo.KFocusAble) {
                                el.classList.add(TClassStr.KClassFocusable);
                            }
                            if (focusInfo !== TFocusInfo.KFocusUnknown) {
                                this._element.appendChild(el);
                            }
                            this._element.appendChild(el);
                        }
                        if(el.classList.contains(KClassActiveFocusedLeaf)) {
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
                        } else if(el.classList.contains(KClassFocusable)) {
                            el.classList.remove(KClassFocusable);
                            el.classList.add(KClassFocusDisable);
                        }
                        if(horizontal) {
                            if (elRight > aRect.right) {
                                contentAvailable.right = aRect.right - elLeft;
                            } else if (elLeft < aRect.left) {
                                contentAvailable.left = aRect.left - elRight;
                            }
                        } else {
                            if (elBottom > aRect.bottom) {
                                contentAvailable.down = aRect.bottom - elTop;
                            } else if (elTop < aRect.top) {
                                contentAvailable.up = aRect.top - elBottom;
                            }
                        }
                    }
                }
                if (!this.getAnimation()) {
                    prevDrawnItem.destroy();
                } else {
                    prevDrawnItem.forEach((aKey: any, aItem: HTMLElement) => {
                        var ret = false;
                        if (aItem.classList.contains(CtrlBase.TClassStr.KClassActiveFocusedLeaf)) {
                            aItem.classList.remove(CtrlBase.TClassStr.KClassActiveFocusedLeaf);
                            ret = true;
                        }
                        return ret;
                    });
                    this._fnEndTransition = () => {
                        prevDrawnItem.destroy();
                    }
                }
                if (itemCount) {
                    var size = this.getSize();
                    var first = focusableElements[0];
                    var last = focusableElements[focusableElements.length - 1];
                    if(horizontal) {
                        var left = first ? first.offsetLeft - contentAvailable.left : 0;
                        var right = last ? last.offsetLeft + last.offsetWidth + contentAvailable.right : 0;
                        contentAvailable.left = left;
                        contentAvailable.right = size.width - right;
                    } else {
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
        }
        setMaxRowCount(aMaxRowCount: number) {
            this._setDrawParam(KParamStrMaxRowCount, aMaxRowCount, false);
            this._setDrawParam(KParamStrOrientation, TParamOrientation.EVertical, false);
            this.setMaxColCount = function(x) {
                throw "can not setMaxColCount after setMaxRowCount";
            }
        }
        setMaxColCount(aMaxColCount: number) {
            this._setDrawParam(KParamStrMaxColCount, aMaxColCount, false);
            this._setDrawParam(KParamStrOrientation, TParamOrientation.EVertical, false);
            this.setMaxRowCount = function(x) {
                throw "can not setMaxRowCount after setMaxColCount ";
            }
        }
        getMaxRowCount() {
            return this._getDrawParam(KParamStrMaxRowCount)
        }
        doItemInserted (aKey: any, aItems: any[], aNeedFocus?: boolean) {
            this.emit.call(this, "ItemInserted", this._drawnElements, aNeedFocus);
        }
        doItemRemoved(aKey: number, aUnsetFocus?: boolean) {
            this.emit.call(this, "ItemRemoved", this._drawnElements, aUnsetFocus);
        }
        getPositionFromHintArray(aItem: number, aMax?: boolean): {col:number; row:number; maxCol?:number; maxRow?:number} {
            var ret = null;
            if(this._keyMapHint) {
                var i, j, rowLen = this._keyMapHint.length;
                for(i=0; i<rowLen; i++) {
                    var temp = this._keyMapHint[i];
                    var colLen = temp.length;
                    for(j=0; j<colLen; j++) {
                        if(temp[j] === aItem) {
                            if(aMax) {
                                if(ret) {
                                    ret.maxCol = j;
                                    ret.maxRow = i;
                                } else {
                                    ret = {
                                        col : j,
                                        row : i,
                                        maxCol : j,
                                        maxRow : i
                                    };
                                }
                            } else {
                                ret = {
                                    col : j,
                                    row : i
                                };
                                break;
                            }
                        }
                    }
                    if(!aMax && ret !== null) {
                        break;
                    }
                }
            }
            return ret;
        }
    }
    export class CGroupControl extends CControl {

        _children: CControl[];
        constructor(aElement: HTMLElement) {
            //FIXME: jQuery
            super($(aElement), "-group");
            this._group = true;
            this._children = [];
            this.registerSignal(["ChildViewMoved"]);
        }
        private _destroyChild() {
            var i, len, c: CControl;
            for (i = 0, len = this._children.length; i < len; i++) {
                c = this._children[i];
                c.destroy();
            }
        }
        /*
        destroy() {
            this._destroyChild();

        }
        */
        setKeyBuilder(aLayout: TParamOrientation) {
            if (aLayout == TParamOrientation.EVertical) {
                this._keyMapBuilder = KBuilderTopDown;
            } else if (aLayout == TParamOrientation.EHorizontal) {
                this._keyMapBuilder = KBuilderLeftRight;
            } else {
                this._keyMapBuilder = KBuilderLeftRight;
            }
        }
        setOwnedChildControls(aChildControls: CControl[], aFocusedChildIndex?:number) {
            this._destroyChild();
            var i, len;
            for (i = 0, len = aChildControls.length; i < len; i += 1) {
                aChildControls[i].setParent(this);
            }
            if(aFocusedChildIndex && aChildControls[aFocusedChildIndex]) {
                aChildControls[aFocusedChildIndex].setFocus(true);
            } else {
                aChildControls[0].setFocus(true);
            }
            this._children = aChildControls;
        }
        insertOwnedChild(aIndex: number, aControl: CControl) {
            this._children.unshift(aControl);
        }
        appendOwnedChild(aControl: CControl) {
            this._children.push(aControl);
        }
        removeOwnedControl(aControl: CControl) {
            var index = this._children.indexOf(aControl);
            if (index != -1) {
                this._children.splice(index, 1);
                aControl.destroy();
            }
        }

        draw(aRect?: TRect) {
            super.draw(aRect);
//            this.setActiveFocus();
        }
        _doDraw(aRect: TRect, aDrawParam: { [key: string]: any; }) {
            return this._doDrawCommon(this._element, aRect, aDrawParam);
        }
        _doDrawCommon(aParent: HTMLElement, aRect: TRect, aDrawParam: { [key: string]: any; }) {
            var ret: HTMLElement[] = [];
            this._element.classList.add(KClassFocused);
            var i, len, c: CControl, el: HTMLElement;
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
        }
        getFocusedChild(): CControl {
            var i, len, c: CControl;
            for (i = 0, len = this._children.length; i < len; i++) {
                c = this._children[i];
                if (c.isFocused()) {
                    return c;
                }
            }
            return null;
        }
        _setActiveFocus(aFocus: boolean) {
            var focusedChild: CControl;
            if (this.isFocusable()) {
                if (aFocus) {
                    this._element.classList.add(KClassFocused);
                } else {
                    this._element.classList.remove(KClassFocused);
                }
                focusedChild = this.getFocusedChild();
                focusedChild._setActiveFocus(aFocus);
            }
        }
        // CGroupControl events
        connectChildFocusChanged(aHolder: any, aSlotName: string, aSlot: FChildFocusChanged) {
            this.connect("ChildFocusChanged", aHolder, aSlotName);
        }
        _handleViewMoved(aControl: CControl, aIncrement: { top: number; left: number; }) {
            this._emitChildViewMoved(aControl, aIncrement);
        }
        connectChildViewMoved(aHolder: any, aSlotName: string, aSlot: FChildViewMoved) {
            this.connect("ChildViewMoved", aHolder, aSlotName);
        }
        private _emitChildViewMoved(aControl: CControl, aIncrement: { top: number; left: number; }) {
            this.emit.call(aControl, "ChildViewMoved", this, aIncrement);
        }
    }
    export class CViewGroupControl extends CGroupControl {
        _container: HTMLElement;
        _containerPosForAni: { top: number; left: number; };
        _targetChild: CControl;
        _showScrollbar: boolean = false;
        _scrollBarBg: HTMLElement;
        _scrollBarTrack: HTMLElement;
        _scrollBarBgUserHeight: number;
        _scrollBarTrackUserHeight: number;

        private _drawnRect: TRect;

        constructor(aElement: HTMLElement) {
            super(document.createElement("div"));
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
            this._keyMapBuilder = KBuilderTopDown;
        }
        setClassToGroupViewElement(aClassName: string) {
            var classes = aClassName.split(" ");
            for (var i = 0; i < classes.length; i++) {
                this._element.classList.add(classes[i]);
            }
        }
        setOwnedChildControls(aChildControls: CControl[]) {
            if (aChildControls.length != 1) {
                throw "just single child supported";
            }
            this._targetChild = aChildControls[0];
            super.setOwnedChildControls(aChildControls);
        }
        setScrollScheme(aScheme: TParamScrollScheme, aFixedScrollUnit?: number) {
            super.setScrollScheme(aScheme, aFixedScrollUnit);
            //this._targetChild.setScrollScheme(aScheme, aFixedScrollUnit);
        }
        setVerticalScrollScheme(aScheme: TParamScrollScheme, aFixedScrollUnit?: number) {
            super.setVerticalScrollScheme(aScheme, aFixedScrollUnit);
            //this._targetChild.setVerticalScrollScheme(aScheme, aFixedScrollUnit);
        }
        setHorizontalScrollScheme(aScheme: TParamScrollScheme, aFixedScrollUnit?: number) {
            super.setHorizontalScrollScheme(aScheme, aFixedScrollUnit);
            //this._targetChild.setHorizontalScrollScheme(aScheme, aFixedScrollUnit);
        }
        setKeepFocus(aKeepFocus: boolean) {
            super.setKeepFocus(aKeepFocus);
            //this._targetChild.setKeepFocus(aKeepFocus);
        }
        setShowScrollbar(aShowScrollbar: boolean) {
            this._showScrollbar = aShowScrollbar;
        }
        setScrollbarBgUserHeight(aHeight: number) {
            this._scrollBarBgUserHeight = aHeight;
        }
        setScrollbarTrackUserHeight(aHeight: number) {
            this._scrollBarTrackUserHeight = aHeight;
        }
        setViewSize(aRect: TRect) {
            this._element.style.left = aRect.left + "px";
            this._element.style.top = aRect.top + "px";
            this._element.style.height = aRect.getHeight() + "px";
            this._element.style.width = aRect.getWidth() + "px";
        }
        moveFocusOutOfKeymap(aIndex: number) {
            var focusedChild: CDataControl = <CDataControl>this.getFocusedChild();
            var customScrollSize = focusedChild.getCustomScrollSizeByIndex(aIndex);
            if(customScrollSize && customScrollSize.top) {
                if(this._drawnRect.top < customScrollSize.top) {
                    customScrollSize.top = this._drawnRect.top;
                }
                var incrementRow = Math.floor(customScrollSize.top / focusedChild.getItemHeight());
                this._handleViewMoved(this, customScrollSize);
                this.moveDrawPosition(customScrollSize);
                if(!focusedChild._keyMapHint) {
                    var i, len = focusedChild._focusableElement? focusedChild._focusableElement.length : 0;
                    for(i=0; i<len; i++) {
                        var item = focusedChild._focusableElement[i];
                        if(aIndex === parseInt(item.getAttribute('data'))) {
                            aIndex = i;
                            break;
                        }
                    }
                }
                focusedChild.setFocusedElementByIndex(aIndex);
                focusedChild.setActiveFocus();
            }
        }
        _doDraw(aRect: TRect, aDrawParam: { [key: string]: any; }) {
            if (this._element.offsetWidth == 0 || this._element.offsetHeight == 0) {
                throw "drawing view group without size meaning-less";
            }
            var focusableElements: HTMLElement[] = [];
            var drawPosTop: number;
            var drawPosLeft: number;
            if (this.getAnimation() && this._getContainerPosForAni()) {
                drawPosTop = -this._getContainerPosForAni().top;
                drawPosLeft = -this._getContainerPosForAni().left;
                this._setContainerPosForAni(undefined);
            } else {
                drawPosTop = -this._container.offsetTop;
                drawPosLeft = -this._container.offsetLeft;
            }
            var drawRect: TRect = new TRect({
                top: drawPosTop,
                left: drawPosLeft,
                right: drawPosLeft + this._element.offsetWidth,
                bottom: drawPosTop + this._element.offsetHeight
            });
            var i, len, c: CControl, el: HTMLElement; // looping param
            var childRect: TRect = new TRect; // rectangle for child control
            var childSize: { width: number; height: number; }; // size of child control
            var drawRectForChild: TRect; // rectangle param for child control
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
                if (scrollSchemeVertical === CtrlBase.TParamScrollScheme.EByFixedFocusRemains || scrollSchemeHorizontal === CtrlBase.TParamScrollScheme.EByVariableFocusRemains) {
                    drawRectForChild = drawRect;
                } else {
                    drawRectForChild = drawRect.getIntersectedRect(childRect);
                }
                c.draw(drawRectForChild || new TRect());
                if (c.isFocusable()) {
                    focusableElements.push(el);
                }
                childRect.moveTop(childRect.getHeight());
                if (this._showScrollbar && childRect.getHeight() > drawRectForChild.getHeight()) {
                    var barBgHeight = this._scrollBarBgUserHeight? this._scrollBarBgUserHeight : this._element.offsetHeight;
                    if(!this._scrollBarTrack) {
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
                        this._scrollBarBg = scrollBarBg
                        this._scrollBarTrack = scrollBarTrack;
                    } else if(this._scrollBarTrack.style.display === 'none') {
                        this._scrollBarBg.style.display = 'block';
                        this._scrollBarTrack.style.display = 'block';
                    }
                    if(this._scrollBarTrackUserHeight) {
                        this._scrollBarTrack.style.height = this._scrollBarTrackUserHeight + 'px';
                        this._scrollBarTrack.style.top = Math.floor(((barBgHeight - this._scrollBarTrackUserHeight) * drawRectForChild.top) / (childRect.getHeight() - drawRectForChild.getHeight())) + 'px';
                    } else {
                        this._scrollBarTrack.style.height = Math.floor((drawRectForChild.getHeight() * barBgHeight) / childRect.getHeight()) + 'px';
                        this._scrollBarTrack.style.top = Math.floor((drawRectForChild.top * barBgHeight) / childRect.getHeight()) + 'px';
                    }
                } else if (this._scrollBarTrack) {
                    this._scrollBarBg.style.display = 'none';
                    this._scrollBarTrack.style.display = 'none';
                }
            }
            this._drawnRect = drawRect;
            return focusableElements;
        }
        private _getNextMove(aKeyStr: string, aControl: CDataControl): { top: number; left: number; } {
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
        }
        private _setContainerPosForAni(aPosition: { top: number; left: number; }) {
            this._containerPosForAni = aPosition;
        }
        private _getContainerPosForAni(): { top: number; left: number; } {
            return this._containerPosForAni;
        }
        private _getContainerPos(): { top: number; left: number; } {
            var containerTop = this._container.offsetTop;
            var containerLeft = this._container.offsetLeft;
            return {
                top: containerTop,
                left: containerLeft
            };
        }
        moveDrawPosition(aPosition: { top: number; left: number; }) {
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
        }
        initDrawPosition(aPosition?: { top: number; left: number; }) {
            var pos = (aPosition) ? aPosition : { top: 0, left: 0 };
            this._setContainerPosForAni(pos);
            this._container.style.top = pos.top + "px";
            this._container.style.left = pos.left + "px";
            this.draw();
        }
        setDrawPosition(aPosition: { top: number; left: number; }) {
            if (this.getAnimation()) {
                Util.afterTransition(this._container, () => {
                    this.setTransition(false);
                }, true);
                this._setContainerPosForAni(aPosition);
                this.draw();
                this.setTransition(true);
                this._container.style.top = aPosition.top + "px";
                this._container.style.left = aPosition.left + "px";
            } else {
                this._container.style.top = aPosition.top + "px";
                this._container.style.left = aPosition.left + "px";
                this.draw();
            }
        }
        setAnimation(aAnimation: boolean) {
            super.setAnimation(aAnimation);
            if (aAnimation) {
                this._container.style[KCssPropTransition] = KCssTransitionParamPos;
            } else {
                this._container.style[KCssPropTransition] = '';
            }
        }
        private _getScrollSizeVertical(aUp:boolean, aPage: boolean): { top: number; left: number; } {
            var ret = {
                top: 0,
                left: 0
            }

            var focusedChild: CDataControl = <CDataControl>this.getFocusedChild();
            var contentAvail: TContentAvail = focusedChild.getContentAvail();
            if(aUp && contentAvail.up === 0) {
                return ret;
            } else if(!aUp && contentAvail.down === 0) {
                return ret;
            }

            var scrollScheme = this.getVerticalScrollScheme();
            var itemHeight = focusedChild.getItemHeight();
            switch(scrollScheme) {
                case CtrlBase.TParamScrollScheme.EByItem:
                    if(aPage) {
                        ret.top = Math.floor(this._drawnRect.getHeight() / itemHeight) * itemHeight;
                    } else {
                        if(focusedChild._drawerHint) {
                            var customSrollSize: TContentAvail = focusedChild.getCustomScrollSize(aUp? KKeyStrUp:KKeyStrDown, this._drawnRect);
                            ret.top = customSrollSize? customSrollSize.up : itemHeight;
                        } else {
                            ret.top = itemHeight;
                        }
                    }
                    break;
                case CtrlBase.TParamScrollScheme.EByPage:
                    ret.top = this._drawnRect.getHeight();
                    break;
                case CtrlBase.TParamScrollScheme.EByFixed:
                    var unit = this._getDrawParam(KParamStrScrollSchemeFixedUnitVertical);
                    if(aPage) {
                        ret.top = Math.floor(this._drawnRect.getHeight() / unit) * unit;
                    } else {
                        ret.top = unit;
                    }
                    break;
                case CtrlBase.TParamScrollScheme.EByFixedFocusRemains:
                case CtrlBase.TParamScrollScheme.EByVariableFocusRemains:
                    var containerPos = this._getContainerPos();
                    if(aPage) {
                        if(aUp) {
                            var firstFocusableElement = focusedChild.getElementByIndex(0);
                            ret.top = this._drawnRect.getHeight() - (containerPos.top + firstFocusableElement.offsetTop + itemHeight);
                        } else {
                            var lastFocusableElement = focusedChild.getElementByIndex(focusedChild.getFocusableElementCount() - 1);
                            ret.top = containerPos.top + lastFocusableElement.offsetTop;
                        }
                    } else {
                        var focusedElement = focusedChild.getFocusedElement();
                        if(aUp) {
                            ret.top = this._drawnRect.getHeight() - (containerPos.top + focusedElement.offsetTop + itemHeight);
                        } else {
                            ret.top = containerPos.top + focusedElement.offsetTop;
                        }
                    }
                    break;
                default:
                    break;
            }

            if(aUp) {
                if(ret.top > contentAvail.up) {
                    ret.top = contentAvail.up;
                }
            } else {
                if(ret.top > contentAvail.down) {
                    if(scrollScheme === CtrlBase.TParamScrollScheme.EByVariableFocusRemains) {
                        ret.top = contentAvail.down;
                    } else if (aPage && scrollScheme === CtrlBase.TParamScrollScheme.EByItem) {
                        ret.top =  Math.ceil(contentAvail.down / itemHeight) * itemHeight;
                    } else if (this._drawnRect.getHeight() < ret.top) {
                        ret.top = this._drawnRect.getHeight();
                    }
                }
                ret.top = -ret.top;
            }
            console.log('ret.top='+ret.top);
            return ret;
        }
        private _getScrollSizeHorizontal(aLeft:boolean): { top: number; left: number; } {
            var ret = {
                top: 0,
                left: 0
            };

            var focusedChild: CDataControl = <CDataControl>this.getFocusedChild();
            var contentAvail: TContentAvail = focusedChild.getContentAvail();
            var oriental = focusedChild._getDrawParam(KParamStrOrientation);
            var scrollScheme = this.getHorizontalScrollScheme();
            var itemWidth = focusedChild.getItemWidth();
            if(oriental === TParamOrientation.EHorizontal) {
                var containerPos = this._getContainerPos();
                var focusedElement = focusedChild.getFocusedElement();
                if (aLeft && contentAvail.left === 0) {
                    return ret;
                } else if(!aLeft && contentAvail.right === 0) {
                    return ret;
                }
                switch(scrollScheme) {
                    case CtrlBase.TParamScrollScheme.EByItem:
                        ret.left = itemWidth;
                        break;
                    case CtrlBase.TParamScrollScheme.EByPage:
                        ret.left = this._drawnRect.getWidth();
                        break;
                    case CtrlBase.TParamScrollScheme.EByFixed:
                        if (aLeft) {
                            if (contentAvail.left > 0) {
                                ret.left = itemWidth;
                            }
                        } else if (contentAvail.right > itemWidth) {
                            ret.left = itemWidth;
                        }
                        break;
                    case CtrlBase.TParamScrollScheme.EByFixedFocusRemains:
                    case CtrlBase.TParamScrollScheme.EByVariableFocusRemains:
                        if(aLeft) {
                            ret.left = this._drawnRect.getWidth() - (containerPos.left + focusedElement.offsetLeft + itemWidth);
                        } else {
                            ret.left = containerPos.left + focusedElement.offsetLeft;
                        }
                        break;
                    default:
                        break;
                }

                if(aLeft) {
                    if(contentAvail.left !== 0 && ret.left > contentAvail.left) {
                        ret.left = contentAvail.left;
                    }
                } else {
                    if(ret.left > contentAvail.right) {
                        if(scrollScheme === CtrlBase.TParamScrollScheme.EByVariableFocusRemains) {
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
                console.log('ret.left='+ret.left);
            } else {
                var customSrollSize: TContentAvail = focusedChild.getCustomScrollSize(aLeft? KKeyStrLeft:KKeyStrRight, this._drawnRect);
                if(customSrollSize) {
                    ret.top = customSrollSize.up;
                }
            }
            return ret;
        }
        private _getNextFocusedIndex(aKeyStr: string, aIncrement: {top: number; left: number;}) {
            var focusedChild: CDataControl = <CDataControl>this.getFocusedChild();
            var ret = focusedChild.getFocusedElementIndex();

            var scrollScheme = this.getVerticalScrollScheme();
            var oriental = focusedChild._getDrawParam(KParamStrOrientation);
            switch(aKeyStr) {
                case KKeyStrUp:
                case KKeyStrDown:
                    var incrementRow = Math.floor(aIncrement.top / focusedChild.getItemHeight());
                    if(scrollScheme === CtrlBase.TParamScrollScheme.EByPage ||
                        scrollScheme === CtrlBase.TParamScrollScheme.EByFixedFocusRemains ||
                        scrollScheme === CtrlBase.TParamScrollScheme.EByVariableFocusRemains ) {
                        ret = focusedChild.getNextFocusIndex(incrementRow, true);
                    } else if(scrollScheme === CtrlBase.TParamScrollScheme.EByFixed) {
                        ret = focusedChild.getNextFocusIndex(incrementRow, false);
                    } else if(focusedChild._drawerHint) {
                        ret = focusedChild.getNextFocusIndex(incrementRow, false);
                    }
                    break;
                case KKeyStrChannelUp:
                case KKeyStrChannelDown:
                    var incrementRow = Math.floor(aIncrement.top / focusedChild.getItemHeight());
                    if(focusedChild._drawerHint) {
                        ret = focusedChild.getNextFocusIndex(incrementRow, false);
                    }
                    break;
                case KKeyStrLeft:
                case KKeyStrRight:
                    if(oriental === TParamOrientation.EVertical) {
                        if(focusedChild._drawerHint) {
                            ret = focusedChild.getNextFocusIndex(null, false);
                        }
                    }
                    break;
                default:
                    break;
            }
            return ret;
        }
        private _fnScrolling2(aKeyStr: string) {
            var handled = false;
            var focusedChild: CDataControl = <CDataControl>this.getFocusedChild();
            var increment: {top:number; left: number;};

            switch(aKeyStr) {
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

            if(increment.top !== 0 || increment.left !== 0) {
                var nextIndex = this._getNextFocusedIndex(aKeyStr, increment);
                this._handleViewMoved(this, increment);
                this.moveDrawPosition(increment);

                var focusableCount = focusedChild.getFocusableElementCount();
                focusedChild.setFocusedElementByIndex(nextIndex);
                focusedChild.setActiveFocus();
                if(focusedChild.getDataRolling()) {
                    if(nextIndex < 0) {
                        nextIndex = focusableCount - 1;
                    } else if(nextIndex > focusableCount - 1) {
                        nextIndex = 0
                    }
                } else {
                    if(nextIndex < 0) {
                        nextIndex = 0;
                    } else if(nextIndex > focusableCount - 1) {
                        nextIndex = focusableCount - 1;
                    }
                }
                focusedChild.setFocusedElementByIndex(nextIndex);
                focusedChild.setActiveFocus();
                handled = true;
            }
            return handled;
        }
        _doKeyUpLatent(aKeyCode) {
            //return this._fnScrolling(KKeyStrUp);
            return this._fnScrolling2(KKeyStrUp);
        }
        _doKeyDownLatent(aKeyCode) {
            //return this._fnScrolling(KKeyStrDown);
            return this._fnScrolling2(KKeyStrDown);
        }
        _doKeyChannelUpLatent(aKeyCode) {
            //return this._fnScrolling(KKeyStrUp, this.getChannelUpIndex());
            return this._fnScrolling2(KKeyStrChannelUp);
        }
        _doKeyChannelDownLatent(aKeyCode) {
            //return this._fnScrolling(KKeyStrDown, this.getChannelDownIndex());
            return this._fnScrolling2(KKeyStrChannelDown);
        }
        _doKeyLeftLatent(aKeyCode) {
            return this._fnScrolling2(KKeyStrLeft);
        }
        _doKeyRightLatent(aKeyCode) {
            return this._fnScrolling2(KKeyStrRight);
        }
    }
    export class CAbsCommonDataOperationControl extends CViewGroupControl {
        _dataControl: CDataControl;

        constructor(aElement: HTMLElement) {
            super(aElement);
            this.setupDataControl();
            this._element.appendChild(this._dataControl._element);
            this.setOwnedChildControls([this._dataControl]);
        }
        setupDataControl() {
            this.doSetupDataControl();
            this._dataControl.connectKeyMapUpdated(this, "_slKeyMapUpdated", this._slKeyMapUpdated);
        }
        doSetupDataControl() {
        }
        // setting
        setDataRolling(aRolling:boolean) {
            this._dataControl.setDataRolling(aRolling);
        }
        getDataRolling() {
            return this._dataControl.getDataRolling();
        }
        setItemHeight(aItemHeight:number) {
            this._dataControl.setItemHeight(aItemHeight);
        }
        setItemWidth(aItemWidth:number) {
            this._dataControl.setItemWidth(aItemWidth);
        }
        setOrientation(aLayout:TParamOrientation) {
            this._dataControl._setDrawParam(KParamStrOrientation, aLayout, false);
        }
        setOwnedDataProvider(aDataProvider: CDataProvider) {
            this._dataControl.setOwnedDataProvider(aDataProvider);
            aDataProvider.connectItemInserted(this, "_slItemInserted", this._slItemInserted);
            aDataProvider.connectItemRemoved(this, "_slItemRemoved", this._slItemRemoved);
            aDataProvider.connectItemUpdated(this, "_slItemUpdated", this._slItemUpdated);
        }
        setDataDrawer(aDrawer: FDataDrawer) {
            this._dataControl.setDataDrawer(aDrawer);
        }
        setListData(aData: any[], aDataRolling?: boolean) {
            this.setDataRolling(aDataRolling);
            this.setOwnedDataProvider(new CListDataProvider(aData, aDataRolling));
        }
        // event connection API
        connectFocusChanged(aHolder: any, aSlotName: string, aSlot: FFocusChanged) {
            this._dataControl.connectFocusChanged(aHolder, aSlotName, aSlot);
        }
        connectFocusGained(aHolder: any, aSlotName: string, aSlot: FFocusGained) {
            this._dataControl.connectFocusGained(aHolder, aSlotName, aSlot);
        }
        connectFocusLost(aHolder: any, aSlotName: string, aSlot: FFocusLost) {
            this._dataControl.connectFocusLost(aHolder, aSlotName, aSlot);
        }
        connectFocusedDataItemChanged(aHolder: any, aSlotName: string, aSlot: FFocusedDataItemChanged) {
            this._dataControl.connectFocusedDataItemChanged(aHolder, aSlotName, aSlot);
        }
        connectDataItemSelected(aHolder: any, aSlotName: string, aSlot: FDataItemSelected) {
            this._dataControl.connectDataItemSelected(aHolder, aSlotName, aSlot);
        }
        connectItemSelected(aHolder: any, aSlotName: string, aSlot: FItemSelected) {
            this._dataControl.connectItemSelected(aHolder, aSlotName, aSlot);
        }
        connectDataItemOrderChanged(aHolder: any, aSlotName: string, aSlot: FDataItemOrderChanged) {
            this._dataControl.connectDataItemOrderChanged(aHolder, aSlotName, aSlot);
        }
        // data control API
        prependItems(aItems: any[]) {
            this._dataControl.insertItems(0, aItems);
        }
        insertItems(aPosition: number, aItems: any[]) {
            this._dataControl.insertItems(aPosition, aItems);
        }
        appendItems(aItems: any[]) {
            this._dataControl.appendItems(aItems);
        }
        removeItems(aIndex: number[]) {
            this._dataControl.removeItems(aIndex);
        }
        updateItems(aKeys: number[], aItems?: any[]) {
            this._dataControl.updateItems(aKeys, aItems);
        }
        getItemLength() {
            return this._dataControl.getItemLength();
        }
        getItemHeight() {
            return this._dataControl.getItemHeight();
        }
        getCountOnPage() {
            //TODO: consider horizontal or vertical, currently support vertical scheme only.
            return this._element.offsetHeight / this.getItemHeight();
        }
        getCheckedItems() {
            return this._dataControl.getCheckedItems();
        }
        deleteCheckedItem() {
            return this._dataControl.deleteCheckedItem();
        }
        setItemCheck(aIndex: number) {
            this._dataControl.setItemCheck(aIndex);
            this.update();
        }
        setItemUnCheck(aIndex: number) {
            this._dataControl.setItemUnCheck(aIndex);
            //XXXX
            this.update();
        }
        setItemCheckClear() {
            this._dataControl.setItemCheckClear();
            this.update();
        }
        isItemCheck(aIndex: number) {
            this._dataControl.isItemCheck(aIndex);
        }
        // event handler
        private _slItemInserted(drawnElements: CDrawnElements, aNeedFocus?: boolean) {
            this.update();
        }
        private _slItemRemoved(drawnElements: CDrawnElements, aUnsetFocus?: boolean) {
            this.update();
        }
        private _slItemUpdated(aKeys: any[], aItems?: any[]) {
            this.update();
        }
        getFocusedItemInfo(): TFocusedInfo {
            return this._dataControl.getFocusedItemInfo();
        }
        getFocusedElementIndex(): number {
            return this._dataControl.getFocusedElementIndex();
        }
        getFocusedElement(): HTMLElement {
            return this._dataControl.getFocusedElement();
        }
        /* unsed
        setFocusedItem(aKey: any) {
            this._dataControl.setFocusedItem(aKey);
        }
        setRedrawAfterOperation(aRedraw: boolean) {
            this._dataControl.setRedrawAfterOperation(aRedraw);
        }
        */
        isMovable(aKeyStr: string) {
            //TODO: this function need additional implementation, consider scroll scheme & data rolling
            var focusedItem = this.getFocusedElement();
            var ret = false;
            if (focusedItem) {
                var onePage = this.getCountOnPage();
                var count = this.getItemLength();
                var pos = focusedItem.attributes['data'];
                var scheme = this.getVerticalScrollScheme();
                var container = this._container;

                if (scheme === TParamScrollScheme.EByFixedFocusRemains) {
                    if (aKeyStr === KKeyStrUp) {
                        ret = (pos !== 0);
                    } else if (aKeyStr === KKeyStrDown) {
                        ret = (pos !== count - 1);
                    } else if (aKeyStr === KKeyStrChannelUp) {
                        ret = (pos >= onePage - 1 && container.offsetTop !== 0);
                    } else if (aKeyStr === KKeyStrChannelDown) {
                        if (count < onePage) {
                            ret = false;
                        } else {
                            var heightScroll = this.getItemHeight() * (onePage - 1);
                            var heightList = this._dataControl.getSize().height;
                            var endOfList = Math.floor(heightList / heightScroll) * heightScroll;
                            if (endOfList === heightList) {
                                endOfList -= heightScroll;
                            }
                            ret = (Math.abs(container.offsetTop) !== endOfList);
                        }
                    }
                } else {
                    throw 'do implement for control other scroll scheme';
                }
            }
            return ret;
        }
        _slKeyMapUpdated(aKeyMap: CKeyMap) {
            throw "do implement case 'this._keyMap === null', occurred by remove operation";
        }
    }
    export class CListControl extends CAbsCommonDataOperationControl {
        _dataControl: CListDataControl;

        constructor(aElement: HTMLElement) {
            super(aElement);
            this.name = "CListControl";
            this.setListData([]);
        }
        setAnimation(aAni: boolean) {
            super.setAnimation(aAni);
            this._dataControl.setAnimation(aAni)
            this.connectTransitioningChanged(this, '_slTransitioning', this._slTransitioning);
        }
        _slTransitioning(aTransitioning: boolean) {
            if (!aTransitioning) {
                this._dataControl.endTransition();
            }
        }
        doSetupDataControl() {
            this._dataControl = new CListDataControl(document.createElement('div'));
        }
        _slKeyMapUpdated(aKeyMap: CKeyMap) {
            if (aKeyMap === null) {
                var scheme = this.getVerticalScrollScheme();
                var topContainer = this._container.style.top;
                if (scheme === TParamScrollScheme.EByFixedFocusRemains) {
                    if (topContainer && parseInt(topContainer) !== 0) {
                        setTimeout(() => {
                            if (!this.isTransitioning()) {
                                this.moveDrawPosition({
                                    top: (this.getCountOnPage() - 1) * this.getItemHeight(),
                                    left: 0
                                });
                                var focusedChild = this.getFocusedChild();
                                var keyMap = focusedChild._keyMap;
                                if (keyMap) {
                                    var index = keyMap.getMapCount() - 1 || 0;
                                    focusedChild.setActiveFocus();
                                    focusedChild.setFocusedElementByIndex(index);
                                }
                                this.setActiveFocus();
                            }
                        }, 300);
                    }
                }
            }
        }
    }
    export class CGridControl extends CAbsCommonDataOperationControl {
        _dataControl: CGridDataControl;

        constructor(aElement: HTMLElement) {
            super(aElement);
            this.name = "CGridControl";
            this.setListData([]);
        }
        doSetupDataControl() {
            this._dataControl = new CGridDataControl(document.createElement('div'));
        }
        setStartIndex(aIndex: number) {
            this._dataControl.setStartIndex(aIndex);
        }
        setMaxRowCount(aMax: number) {
            this._dataControl.setMaxRowCount(aMax);
        }
        setMaxColCount(aMax: number) {
            this._dataControl.setMaxColCount(aMax);
        }
        setAnimation(aAni: boolean) {
            super.setAnimation(aAni);
            this._dataControl.setAnimation(aAni);
            this.connectTransitioningChanged(this, '_slTransitioning', this._slTransitioning);
        }
        setDrawHint(aHint: TDrawHint[]) {
            this._dataControl.setDrawnHint(aHint);
        }
        getRowElemnt(aRow:number, bDuplicate:boolean) {
            return this._dataControl.getRowElement(aRow, bDuplicate);
        }
        getColElemnt(aCol:number, bDuplicate:boolean) {
            return this._dataControl.getColElement(aCol, bDuplicate);
        }
        _slTransitioning(aTransitioning: boolean) {
            if (!aTransitioning) {
                this._dataControl.endTransition();
            }
        }
        _slKeyMapUpdated(aKeyMap: CKeyMap) {
            //TODO
        }
        _doKeyDownLatent(aKeyCode) {
            var handled = super._doKeyDownLatent(aKeyCode);
            if(!handled) {
                var maxCol = this._dataControl._getDrawParam(KParamStrMaxColCount);
                var focusedIndex = this.getFocusedElementIndex();
                if(this._dataControl._drawerHint) {
                    var itemCount = this._dataControl._ownedDataProvider.getLength();
                    var focusedItemInfo = this._dataControl.getPositionFromHintArray(focusedIndex);
                    var lastItemInfo = this._dataControl.getPositionFromHintArray(itemCount-1);
                    if(focusedItemInfo.row < lastItemInfo.row) {
                        handled = this._dataControl.setFocusedElementByIndex(itemCount-1);
                    }
                } else {
                    var focusableCount = this._dataControl.getFocusableElementCount();
                    var currentRow = Math.ceil((focusedIndex+1) / maxCol);
                    var lastRow = Math.ceil(focusableCount / maxCol);
                    if(currentRow < lastRow) {
                        handled = this._dataControl.setFocusedElementByIndex(focusableCount-1);
                    }
                }
            }
            return handled;
        }
        _doKeyRightLatent(aKeyCode) {
            var handled = super._doKeyRightLatent(aKeyCode);
            if(!handled) {
                var maxCol = this._dataControl._getDrawParam(KParamStrMaxColCount);
                var focusedIndex = this.getFocusedElementIndex();
                if(this._dataControl._drawerHint) {
                    var itemCount = this._dataControl._ownedDataProvider.getLength();
                    var focusedItemInfo = this._dataControl.getPositionFromHintArray(focusedIndex);
                    var lastItemInfo = this._dataControl.getPositionFromHintArray(itemCount-1);
                    if(focusedItemInfo.row === lastItemInfo.row) {
                        var nextIndex = this._dataControl._keyMapHint[focusedItemInfo.row-1][focusedItemInfo.col+1];
                        handled = this._dataControl.setFocusedElementByIndex(nextIndex);
                    }
                } else {
                    var focusableCount = this._dataControl.getFocusableElementCount();
                    var currentRow = Math.ceil((focusedIndex+1) / maxCol);
                    var lastRow = Math.ceil(focusableCount / maxCol);
                    if((currentRow === lastRow) && ((focusedIndex+1)%maxCol !== 0)) {
                        handled = this._dataControl.setFocusedElementByIndex(focusedIndex-maxCol+1);
                    }
                }
            }
            return handled;
        }
    }

    export class CMediaListDataProvider extends CListDataProvider {
        private _arrayFile = [];
        private _fileMap = {};
        constructor(aArray: any, aRolling?: boolean) {
            super(aArray, aRolling);
            this._fnPushFileToArray(aArray);
        }
        _fnPushFileToArray(aArray: any[]) {
            for (var i = 0; i < aArray.length; i++) {
                if (aArray[i].fileType === 0) {
                    this._arrayFile.push(aArray[i]);
                    this._fileMap[i] = this._arrayFile.length - 1;
                }
            }
        }
        _fnRemoveFileToArray(aKeys: number[]) {
            var i, len, realKey, listDel = [], map = this._fileMap;
            for (i = 0, len = aKeys.length; i < len; i++) {
                realKey = map[aKeys[i]];
                if (typeof realKey === 'number') {
                    listDel.push(realKey);
                }
            }
            listDel.sort(function(a: number, b: number) {
                return a < b ? -1 : a > b ? 1 : 0;
            });
            for (i = listDel.length - 1; i >= 0; i--) {
                this._arrayFile.splice(listDel[i], 1);
            }
        }
        insertItems(aKey, aItems) {
            this._fnPushFileToArray(aItems);
            super.insertItems(aKey, aItems);
        }
        removeItems(aKeys: any[]) {
            this._fnRemoveFileToArray(aKeys);
            super.removeItems(aKeys);
        }
        getContents(aContentType?) {
            return this._arrayFile;
        }
        //TODO: removeItems implement
    }
    export class CMediaMediaShareDataProvider extends CListDataProvider{
        static KTypeObject = ['video', 'photo', 'music'];
        static KDlnaPrifix = 'hdlna://';
        static KURIPrifix = 'http://';
        private _contents = {
            'video': [],
            'photo': [],
            'music': []
        };

        _fnPushFileToArray(aArray: any[]) {
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
        }
        insertItems(aKey, aItems) {
            this._fnPushFileToArray(aItems);
            super.insertItems(aKey, aItems);
        }
        getContents(aContentType: string) {
            if (CMediaMediaShareDataProvider.KTypeObject.indexOf(aContentType) === -1) {
                return null;
            }
            return this._contents[aContentType];
        }
    }
    export class CMediaListDataControl extends CListDataControl {
        _ownedDataProvider: CMediaListDataProvider;
        constructor(aElement: HTMLElement, aRolling?: boolean) {
            super(aElement, aRolling);
        }
        getContents(aContentType?) {
            return this._ownedDataProvider.getContents(aContentType);
        }
    }
    export class CMediaListControl extends CListControl {
        _dataControl: CMediaListDataControl;
        constructor(aElement: HTMLElement) {
            super(aElement);
            this.setAnimation(true);
        }
        doSetupDataControl() {
            this._dataControl = new CMediaListDataControl(document.createElement('div'));
        }
        setListData(aData: any[], aDataRolling?: boolean) {
            this.setDataRolling(aDataRolling);
            this.setOwnedDataProvider(new CMediaListDataProvider(aData, aDataRolling));
        }
        getContents(aContentType?) {
            return this._dataControl.getContents(aContentType);
        }
    }
    export class CShowcaseTileCarousel extends CControl {
        _title: string;
        _data: any[];
        _index: number;
        _description: string;
        _elDescription: HTMLElement;
        _elContent: HTMLElement;
        constructor(aElement: HTMLElement, aData: any[]) {
            super($(aElement));
            this._title = 'discover showcase';
            this._description = 'Browse top content recommended by Freesat editors.';
            this._data = aData;
            this._index = 0;
            this._element.classList.add('-showcase-tile');
        }
        _doDraw() {
            var ret: HTMLElement[];
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
        }
        _drawContent(aContainer: HTMLElement) {
            aContainer.innerHTML = '';
            var item: any = this._data[this._index];

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

            if(item.ctaText)  {
                this._elDescription.innerHTML = item.ctaText;
            } else {
                this._elDescription.innerHTML = this._description;
            }

            aContainer.appendChild(eThumbnail);
            aContainer.appendChild(eBorder);
            aContainer.appendChild(eLogo);
//            aContainer.appendChild(eTitle);
            aContainer.appendChild(eSubtitle);

            this._index++;
        }
        _doKeyOk() {
            this.emit.call(this, "ItemSelected", this, this._index, null);
            return true;
        }
    }
    console.log("CtrlBase End");
}
export = CtrlBase;
