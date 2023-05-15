///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cList.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
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
    var list = (function (_super) {
        __extends(list, _super);
        function list() {
            var _this = this;
            _super.call(this);
            this.createKeyMap();
            this.doFocus = function () {
                var flist = _this.$.children('.on');
                if (!flist.length) {
                    _this._setFocus(0);
                }
            };
        }
        list.prototype.create = function (aParam) {
            $.extend(this, aParam);
            $.extend(this, {
                _$src: $('<li>', this.itemAttr),
                _$items: [],
                $: $('<ul>', this.attr)
            });
            this._setData(0);
            hx.logadd('eclee');
            return this;
        };
        list.prototype.createKeyMap = function () {
            var k = hx.key;
            this._keyTable = {};
            this._keyRTable = {};
            var a = this._keyTable;
            a[k.KEY_UP] = this._doUp;
            a[k.KEY_DOWN] = this._doDown;
            a[k.KEY_OK] = this._doOk;
        };
        list.prototype.destroy = function () {
            this.$.remove();
        };
        list.prototype.getIndex = function (aObj) {
            var itm = this._$items;
            var n, idx = -1;
            for (n = 0; n < itm.length; n += 1) {
                if (itm[n].is(aObj)) {
                    idx = n;
                    break;
                }
            }
            return idx;
        };
        list.prototype.getItem = function (aIndex) {
            if ((0 > aIndex) || (this._$items.length < aIndex)) {
                return undefined;
            }
            return this._$items[aIndex];
        };
        list.prototype.getFocusItem = function () {
            var i = this._get_focusIndex(this._get_subElemList());
            if (i !== -1) {
                return this._$items[i];
            }
            return undefined;
        };
        list.prototype.getFocusIndex = function () {
            var i = this._get_focusIndex(this._get_subElemList());
            return i;
        };
        list.prototype.setItemFocus = function (aIndex) {
            if ((0 > aIndex) || (this._$items.length < aIndex)) {
                return;
            }
            this._setFocus(aIndex);
        };
        list.prototype.setItemUnfocus = function () {
            this._setUnfocus();
        };
        list.prototype.setData = function (aData, aFx, aCount) {
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
        };
        list.prototype.appendData = function (aData) {
            var prevEndPos;
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
        };
        list.prototype.insertData = function (aIndex, aData) {
            this.data.splice(aIndex, 0, aData);
            this.count += 1;
            var o = this._$src.clone();
            this._$items.splice(aIndex, 0, o);
            this.fx(aIndex, o);
            if (aIndex === 0) {
                this.$.prepend(o);
            }
            else {
                this._$items[aIndex - 1].after(o);
            }
        };
        list.prototype.shiftToHead = function () {
            if (this.count <= 1) {
                return;
            }
            var firstItem = this._$items.shift();
            this._$items.push(firstItem);
            this.$.children().eq(0).appendTo(this.$);
        };
        list.prototype.shiftToTail = function () {
            if (this.count <= 1) {
                return;
            }
            var lastItem = this._$items.pop();
            this._$items.unshift(lastItem);
            this.$.children().last().prependTo(this.$);
        };
        list.prototype._on_alKey = function (aParam) {
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
        };
        list.prototype._doUp = function (aParam) {
            var elemList = this._get_subElemList() //
            , focusableStart = this._get_focusableStart(elemList) //
            , i = this._get_focusIndex(elemList);
            hx.log('eclee', 'up focusableStart ' + focusableStart + ' i ' + i);
            if (i === focusableStart) {
                hx.log('eclee', 'scroll up');
                aParam.alEvent = 'alScrollup';
                return;
            }
            this._setFocus(i - 1);
            return false;
        };
        list.prototype._doDown = function (aParam) {
            var elemList = this._get_subElemList() //
            , fableElList = this._get_focusableList(elemList) //
            , focusableEnd = elemList.index(fableElList[fableElList.length - 1]) //
            , i = this._get_focusIndex(elemList);
            hx.log('eclee', 'down focusableEnd ' + focusableEnd + ' i ' + i);
            if (i >= focusableEnd) {
                aParam.alEvent = 'alScrolldown';
                hx.log('eclee', 'scroll down');
                return;
            }
            this._setFocus(i + 1);
            return false;
        };
        list.prototype._doOk = function (aPparam) {
            var elemList = this._get_subElemList() //
            , i = this._get_focusIndex(elemList);
            aPparam.alEvent = 'alClicked';
            aPparam.alData = {
                index: i
            };
            hx.log('eclee', 'doOk index ' + i);
            return false;
        };
        list.prototype._get_subElemList = function () {
            return this.$.children('li');
        };
        list.prototype._get_focusableList = function (aElemList) {
            return aElemList.filter('.focusable');
        };
        list.prototype._get_focusableStart = function (aElemList) {
            var fableElList = this._get_focusableList(aElemList) //
            , focusableStart = -1;
            if (fableElList.length !== 0) {
                focusableStart = aElemList.index(fableElList[0]);
            }
            return focusableStart;
        };
        list.prototype._get_focusIndex = function (aElemList) {
            var focusedElList = aElemList.filter('.on') //
            , focusIndex = -1;
            if (focusedElList.length !== 0) {
                focusIndex = aElemList.index(focusedElList[0]);
            }
            return focusIndex;
        };
        list.prototype._setFocus = function (aIndex) {
            var o = this._$items[aIndex];
            if (o) {
                this._setUnfocus();
                o.addClass('on');
            }
        };
        list.prototype._setUnfocus = function () {
            var flist = this.$.children('.on');
            if (flist.length) {
                flist.removeClass('on');
            }
        };
        list.prototype._setData = function (aPos) {
            var i, ti;
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
        };
        return list;
    })(__COMPONENT_KERNEL__);
    exports.list = list;
    var tab = (function (_super) {
        __extends(tab, _super);
        function tab() {
            _super.call(this);
        }
        tab.prototype.create = function (aParam) {
            var _this = this;
            $.extend(this, aParam);
            $.extend(this, {
                _$src: $('<li>', this.itemAttr),
                _$items: [],
                $: $('<ul>', this.attr)
            });
            var i;
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
            this.$.focus(function () {
                _this._doFocus();
            });
            this.$.children().focus(function () {
                var index = _this.getIndex(_this);
                _this.sendEventToParent({
                    alEvent: 'alChangeItem',
                    alData: {
                        me: _this,
                        index: index
                    }
                });
                _this._index = index;
            });
            return this;
        };
        tab.prototype.destroy = function () {
            this.$.remove();
        };
        tab.prototype.getIndex = function (aObj) {
            var itm = this._$items;
            var n;
            for (n = 0; n < itm.length; n += 1) {
                if (itm[n].is(aObj)) {
                    return n;
                }
            }
            return 0;
        };
        tab.prototype.getItem = function (aIndex) {
            if (this._$items.length < aIndex) {
                return undefined;
            }
            if (0 > aIndex) {
                return undefined;
            }
            return this._$items[aIndex];
        };
        tab.prototype.setItemFocus = function (aIndex) {
            if (this._$items.length < aIndex) {
                return;
            }
            if (0 > aIndex) {
                return;
            }
            this._$items[aIndex].focus();
        };
        tab.prototype.setData = function (aData, aFx) {
            if (!aData || !aFx) {
                hx.log("error", "data or callback function not defined");
                return;
            }
            this.data = aData;
            var i;
            for (i = 0; i < this.count; i += 1) {
                aFx.call(this, i, this._$items[i]);
            }
        };
        tab.prototype._doFocus = function () {
            if (this.$.children('.on').length < 1) {
                if (this._$items.length < 1) {
                    hx.log("error", "Must to have one or more items");
                    return;
                }
                this._$items[0].focus();
            }
        };
        tab.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var fx = this._keyTable[aParam.alKey];
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
        };
        tab.prototype._doPrev = function (aParam) {
            var o = this.$.children(".on");
            var i = this._index;
            if (i === 0) {
                i = this.count;
            }
            this.$.children().eq(i - 1).focus();
        };
        tab.prototype._doNext = function (aParam) {
            var o = this.$.children(".on");
            var i = this._index;
            var cnt = this.$.children('.focusable').length - 1;
            // hx.log("warning", "i:" + i + " & cnt:" + cnt);
            if (i >= cnt) {
                i = -1;
            }
            this.$.children().eq(i + 1).focus();
        };
        tab.prototype._doOk = function (aParam) {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length;
            aParam.alEvent = 'alClicked';
            aParam.alData = {
                index: i
            };
        };
        return tab;
    })(__COMPONENT_KERNEL__);
    exports.tab = tab;
});
