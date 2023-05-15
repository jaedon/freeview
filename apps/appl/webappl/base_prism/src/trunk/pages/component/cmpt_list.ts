///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cList.js
/*jslint sloppy:true, nomen:true, vars:true */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

/**
 * list component
 * @param count : count of shown item
 *        data : array user data
 *        fx : function setting each items function that argument is $ object of item
 *      attr: attribute for list
 *      itemAttr : attribute for item
 *      $item : $object for content
 *      scroll : true or false
 */
export class list extends __COMPONENT_KERNEL__ {
    $: JQuery;
    count: number;
    fx: any;
    _$items: any;
    data: any;
    _keyTable: any;
    _keyRTable: any;
    itemAttr: any;
    attr: any;
    _$src: any;

    constructor() {
        super();
        this.createKeyMap();
        this.doFocus = () => {
            var flist = this.$.children('.on');
            if (!flist.length) {
                this._setFocus(0);
            }
        }
    }


    create(aParam: any): any {
        $.extend(this, aParam);
        $.extend(this, {
            _$src: $('<li>', this.itemAttr),
            _$items: [],
            $: $('<ul>', this.attr)
        });
        this._setData(0);
        hx.logadd('eclee');

        return this;
    }

    createKeyMap(): void {
        var k: any = hx.key;
        this._keyTable = {};
        this._keyRTable = {};
        var a: any = this._keyTable;
        a[k.KEY_UP] = this._doUp;
        a[k.KEY_DOWN] = this._doDown;
        a[k.KEY_OK] = this._doOk;
    }

    destroy(): void {
        this.$.remove();
    }

    getIndex(aObj: any): number {
        var itm: any = this._$items;
        var n: number, idx: number = -1;
        for (n = 0; n < itm.length; n += 1) {
            if (itm[n].is(aObj)) {
                idx = n;
                break;
            }
        }
        return idx;
    }

    getItem(aIndex: number): any {
        if ((0 > aIndex) || (this._$items.length < aIndex)) {
            return undefined;
        }
        return this._$items[aIndex];
    }

    getFocusItem(): any {
        var i: number = this._get_focusIndex(this._get_subElemList());
        if (i !== -1) {
            return this._$items[i];
        }
        return undefined;
    }

    getFocusIndex(): number {
        var i = this._get_focusIndex(this._get_subElemList());
        return i;
    }

    setItemFocus(aIndex: number) {
        if ((0 > aIndex) || (this._$items.length < aIndex)) {
            return;
        }
        this._setFocus(aIndex);
    }

    setItemUnfocus() {
        this._setUnfocus();
    }

    setData(aData, aFx, aCount) {
        this.$.empty();
        if (aData !== undefined) {
            this.data = aData;
        }
        if (aFx !== undefined) {
            this.fx = aFx;
        }
        if (aCount !== undefined) {
            this.count = aCount;
        }
        this._setData(0);
    }

    appendData(aData: any): void {
        var prevEndPos: number;
        if (!aData) {
            hx.log("error", "data not defined");
            return;
        }
        if (!$.isArray(aData)) {
            aData = [aData];
        }
        this.count += aData.length;
        prevEndPos = this.data.length;
        this.data = this.data.concat(aData);
        this._setData(prevEndPos);
    }

    insertData(aIndex: number, aData: any): void {
        this.data.splice(aIndex, 0, aData);
        this.count += 1;
        var o: any = this._$src.clone();
        this._$items.splice(aIndex, 0, o);
        this.fx(aIndex, o);
        if (aIndex === 0) {
            this.$.prepend(o);
        } else {
            this._$items[aIndex - 1].after(o);
        }
    }

    shiftToHead(): void {
        if (this.count <= 1) {
            return;
        }
        var firstItem: any = this._$items.shift();
        this._$items.push(firstItem);
        this.$.children().eq(0).appendTo(this.$);
    }

    shiftToTail(): void {
        if (this.count <= 1) {
            return;
        }
        var lastItem: any = this._$items.pop();
        this._$items.unshift(lastItem);
        this.$.children().last().prependTo(this.$);
    }

    _on_alKey(aParam) {
        var bConsumed = false;
        var fx = this._keyTable[aParam.alKey];
        if (fx) {
            bConsumed = fx.call(this, aParam);
        }
        if (!bConsumed) {
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
        }
        return bConsumed;
    }

    _doUp(aParam: {alEvent: string}): any {
        var elemList: any = this._get_subElemList()//
            , focusableStart: number = this._get_focusableStart(elemList)//
            , i: number = this._get_focusIndex(elemList);
        hx.log('eclee', 'up focusableStart ' + focusableStart + ' i ' + i);
        if (i === focusableStart) {
            hx.log('eclee', 'scroll up');
            aParam.alEvent = 'alScrollup';
            return;
        }
        this._setFocus(i - 1);
        return false;
    }

    _doDown(aParam: {alEvent: string}): any {
        var elemList: any = this._get_subElemList()//
            , fableElList: any = this._get_focusableList(elemList)//
            , focusableEnd: number = elemList.index(fableElList[fableElList.length - 1])//
            , i: number = this._get_focusIndex(elemList);
        hx.log('eclee', 'down focusableEnd ' + focusableEnd + ' i ' + i);
        if (i >= focusableEnd) {
            aParam.alEvent = 'alScrolldown';
            hx.log('eclee', 'scroll down');
            return;
        }
        this._setFocus(i + 1);
        return false;
    }

    _doOk(aPparam: {alData: any; alEvent: string}): boolean {
        var elemList = this._get_subElemList()//
            , i = this._get_focusIndex(elemList);
        aPparam.alEvent = 'alClicked';
        aPparam.alData = {
            index: i
        };
        hx.log('eclee', 'doOk index ' + i);
        return false;
    }

    _get_subElemList() {
        return this.$.children('li');
    }

    _get_focusableList(aElemList: any) {
        return aElemList.filter('.focusable');
    }

    _get_focusableStart(aElemList: any): number {
        var fableElList: any = this._get_focusableList(aElemList)//
            , focusableStart: number = -1;
        if (fableElList.length !== 0) {
            focusableStart = aElemList.index(fableElList[0]);
        }
        return focusableStart;
    }

    _get_focusIndex(aElemList: any) {
        var focusedElList: any = aElemList.filter('.on')//
            , focusIndex: number = -1;
        if (focusedElList.length !== 0) {
            focusIndex = aElemList.index(focusedElList[0]);
        }
        return focusIndex;
    }

    _setFocus(aIndex: number): void {
        var o: any = this._$items[aIndex];
        if (o) {
            this._setUnfocus();
            o.addClass('on');
        }
    }

    _setUnfocus() {
        var flist: any = this.$.children('.on');
        if (flist.length) {
            flist.removeClass('on');
        }
    }

    _setData(aPos) {
        var i: number, ti: number;
        for (i = aPos, ti = this.count; i < this.count; i += 1) {
            var o = this._$items[i];
            if (!o) {
                o = this._$src.clone();
                this._$items.push(o);
            }
            this.fx(i, o);
            this.$.append(o);
        }
        /*that.$.focus(function() {
         that._doFocus();
         });*/
    }
}
export class tab extends __COMPONENT_KERNEL__ {
    $;
    count: number;
    data: any;
    _$items: any;
    _keyTable: any;
    fx: any;
    attr: any;
    _index: number;
    itemAttr: any;
    _$src: any;

    constructor() {
        super();
    }

    create(aParam) {
        $.extend(this, aParam);
        $.extend(this, {
            _$src: $('<li>', this.itemAttr),
            _$items: [],
            $: $('<ul>', this.attr)
        });
        var i: number;
        ;
        for (i = 0; i < this.count; i += 1) {
            var o = this._$src.clone();
            this._$items.push(o);
            this.$.append(o);
            this.fx(i, o);
        }
        this._keyTable = [];
        this._keyTable[hx.key.KEY_LEFT] = this._doPrev;
        this._keyTable[hx.key.KEY_RIGHT] = this._doNext;
        this._keyTable[hx.key.KEY_OK] = this._doOk;
        this.$.focus(()=> {
            this._doFocus();
        });
        this.$.children().focus(()=> {
            var index = this.getIndex(this);
            this.sendEventToParent({
                alEvent: 'alChangeItem',
                alData: {
                    me: this,
                    index: index
                }
            });
            this._index = index;
        });
        return this;
    }

    destroy(): void {
        this.$.remove();
    }

    getIndex(aObj: any): number {
        var itm: any = this._$items;
        var n: number;
        for (n = 0; n < itm.length; n += 1) {
            if (itm[n].is(aObj)) {
                return n;
            }
        }
        return 0;
    }

    getItem(aIndex: number): any {
        if (this._$items.length < aIndex) {
            return undefined;
        }
        if (0 > aIndex) {
            return undefined;
        }
        return this._$items[aIndex];
    }

    setItemFocus(aIndex: number): any {
        if (this._$items.length < aIndex) {
            return;
        }
        if (0 > aIndex) {
            return;
        }
        this._$items[aIndex].focus();
    }

    setData(aData, aFx) {
        if (!aData || !aFx) {
            hx.log("error", "data or callback function not defined");
            return;
        }
        this.data = aData;
        var i: number;
        for (i = 0; i < this.count; i += 1) {
            aFx.call(this, i, this._$items[i]);
        }
    }

    _doFocus(): void {
        if (this.$.children('.on').length < 1) {
            if (this._$items.length < 1) {
                hx.log("error", "Must to have one or more items");
                return;
            }
            this._$items[0].focus();
        }
    }

    _on_alKey(aParam: {alKey: number; alData: any}) {
        var bConsumed: boolean = false;
        var fx: any = this._keyTable[aParam.alKey];
        if (fx) {
            fx.call(this, aParam);
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
            bConsumed = true;
        }
        return bConsumed;
    }

    _doPrev(aParam: any): void {
        var o: any = this.$.children(".on");
        var i: number = this._index;
        if (i === 0) {
            i = this.count;
        }
        this.$.children().eq(i - 1).focus();
    }

    _doNext(aParam): void {
        var o: any = this.$.children(".on");
        var i: number = this._index;
        var cnt: number = this.$.children('.focusable').length - 1;
        // hx.log("warning", "i:" + i + " & cnt:" + cnt);
        if (i >= cnt) {
            i = -1;
        }
        this.$.children().eq(i + 1).focus();
    }

    _doOk(aParam: {alData: any; alEvent: string}): void {
        var o: any = this.$.children(".on");
        var i: number = o.prevAll('.focusable').length;
        aParam.alEvent = 'alClicked';
        aParam.alData = {
            index: i
        };
    }
}
