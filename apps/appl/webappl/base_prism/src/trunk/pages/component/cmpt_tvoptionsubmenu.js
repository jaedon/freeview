var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    ///<reference path="../../def/sugar.d.ts" />
    var tvOptionSubmenu = (function (_super) {
        __extends(tvOptionSubmenu, _super);
        function tvOptionSubmenu() {
            _super.call(this);
        }
        tvOptionSubmenu.prototype.create = function () {
            var _this = this;
            $.extend(this, {
                $: $('<ul>'),
                _$items: [],
                _keyTable: [],
                _main_menu: [],
                parent_data: null
            });
            this._keyTable[hx.key.KEY_BACK] = this._doPrev;
            this._keyTable[hx.key.KEY_LEFT] = this._doPrev;
            this._keyTable[hx.key.KEY_OK] = this._doOk;
            this._keyTable[hx.key.KEY_DOWN] = this._doDown;
            this._keyTable[hx.key.KEY_UP] = this._doUp;
            this.$.focus(function () {
                _this.setItemFocus(0);
            });
            return this;
        };
        tvOptionSubmenu.prototype.setUlClass = function (aCls) {
            this.$.attr('class', aCls);
        };
        tvOptionSubmenu.prototype.destroy = function () {
            this.$.remove();
        };
        tvOptionSubmenu.prototype.getIndex = function (aObj) {
            var itm = this._$items;
            var n;
            for (n = 0; n < itm.length; n += 1) {
                if (itm[n].is(aObj)) {
                    return n;
                }
            }
            return 0;
        };
        tvOptionSubmenu.prototype.getItem = function (aIndex) {
            if (this._$items.length < aIndex) {
                return undefined;
            }
            if (0 > aIndex) {
                return undefined;
            }
            return this._$items[aIndex];
        };
        tvOptionSubmenu.prototype.setItemFocus = function (aIndex) {
            if (this._$items.length < aIndex) {
                return;
            }
            if (0 > aIndex) {
                return;
            }
            //this._$items[index].focus();
            this._$items[aIndex].addClass('on');
        };
        tvOptionSubmenu.prototype.setItems = function (aData) {
            var i;
            this._$items = [];
            for (i = 0; i < aData.length; i += 1) {
                var _o = aData[i];
                var _li = $('<li>', {
                    'class': _o.cls,
                    'html': _o.value
                });
                this._$items.push(_li);
            }
            this._main_menu = aData;
        };
        tvOptionSubmenu.prototype.setParentData = function (aDdata) {
            this.parent_data = aDdata;
        };
        tvOptionSubmenu.prototype.setListScroll = function (_$sub_menu, aIndex) {
            var i, _o;
            aIndex = aIndex - 0;
            this.$.removeAttr('style');
            this.$.find('li').remove();
            for (i = 0; i < this._$items.length; i += 1) {
                //that._$items[i].attr('class', '');
                this._$items[i].removeClass('dim30');
                this._$items[i].removeClass('on');
            }
            this.$.css('margin-top', '-25px');
            if (this._main_menu.length <= 5) {
                _$sub_menu.attr('class', 'dep2 rol');
                for (i = 0; i < this._$items.length; i += 1) {
                    _o = this._$items[i];
                    this.$.append(_o);
                }
                return;
            }
            if (aIndex < 5) {
                _$sub_menu.attr('class', 'dep2 rol');
                for (i = 0; i <= 5; i += 1) {
                    _o = this._$items[i];
                    this.$.append(_o);
                }
                //that.$.children('li').eq(5).attr('class', 'dim30');
                this.$.children('li').eq(5).addClass('dim30');
            }
            else if (aIndex < (this._$items.length - 1)) {
                _$sub_menu.attr('class', 'dep2 rol scrol');
                for (i = (aIndex - 5); i <= (aIndex + 1); i += 1) {
                    _o = this._$items[i];
                    this.$.append(_o);
                }
                //that.$.children('li').eq(0).attr('class', 'dim30');
                //that.$.children('li').eq(6).attr('class', 'dim30');
                this.$.children('li').eq(0).addClass('dim30');
                this.$.children('li').eq(6).addClass('dim30');
            }
            else {
                _$sub_menu.attr('class', 'dep2 rol scrol');
                for (i = (aIndex - 5); i <= aIndex; i += 1) {
                    _o = this._$items[i];
                    this.$.append(_o);
                }
                //that.$.children('li').eq(0).attr('class', 'dim30');
                this.$.children('li').eq(0).addClass('dim30');
            }
        };
        tvOptionSubmenu.prototype.setListScrollUp = function (_$sub_menu, aIndex) {
            var i, _o;
            this.$.removeAttr('style');
            this.$.find('li').remove();
            for (i = 0; i < this._$items.length; i += 1) {
                //that._$items[i].attr('class', '');
                this._$items[i].removeClass('dim30');
                this._$items[i].removeClass('on');
            }
            var needScroll = (this._$items.length > 5) ? true : false;
            this.$.css('margin-top', '-25px');
            if (needScroll === true && aIndex >= (this._$items.length - 5)) {
                _$sub_menu.attr('class', 'dep2 rol scrol');
                for (i = (this._$items.length - 6); i <= (this._$items.length - 1); i += 1) {
                    _o = this._$items[i];
                    this.$.append(_o);
                }
                //that.$.children('li').eq(0).attr('class', 'dim30');
                this.$.children('li').eq(0).addClass('dim30');
            }
            else if (needScroll === true && aIndex > 0) {
                _$sub_menu.attr('class', 'dep2 rol scrol');
                for (i = (aIndex - 1); i <= (aIndex + 5); i += 1) {
                    _o = this._$items[i];
                    this.$.append(_o);
                }
                //that.$.children('li').eq(0).attr('class', 'dim30');
                //that.$.children('li').eq(6).attr('class', 'dim30');
                this.$.children('li').eq(0).addClass('dim30');
                this.$.children('li').eq(6).addClass('dim30');
            }
            else {
                _$sub_menu.attr('class', 'dep2 rol');
                for (i = 0; i <= (aIndex + 5); i += 1) {
                    _o = this._$items[i];
                    this.$.append(_o);
                }
                //that.$.children('li').eq(5).attr('class', 'dim30');
                this.$.children('li').eq(5).addClass('dim30');
            }
        };
        tvOptionSubmenu.prototype._on_alKey = function (aParam) {
            var that = this;
            var bConsumed = false;
            var fx = this._keyTable[aParam.alKey];
            if (fx) {
                fx.call(this, aParam);
                // send key update
                that.sendEventToParent({
                    alEvent: 'alUpdate'
                });
                bConsumed = true;
            }
            return bConsumed;
        };
        tvOptionSubmenu.prototype._doDown = function (aParam) {
            var i;
            for (i = 0; i < this._$items.length; i += 1) {
                if (this._$items[i].is('.on')) {
                    this._$items[i].removeClass('on');
                    if (i === (this._$items.length - 1)) {
                        this.setListScroll(aParam.alPage._$sub_menu, 0);
                        this.setItemFocus(0);
                    }
                    else {
                        if (this._$items[i + 1].attr('class') !== '') {
                            this.setListScroll(aParam.alPage._$sub_menu, i + 1);
                            this.setItemFocus(i + 1);
                        }
                        else {
                            this.setItemFocus(i + 1);
                        }
                    }
                    break;
                }
            }
        };
        tvOptionSubmenu.prototype._doUp = function (aParam) {
            var i;
            for (i = 0; i < this._$items.length; i += 1) {
                if (this._$items[i].is('.on')) {
                    this._$items[i].removeClass('on');
                    if (i === 0) {
                        this.setListScrollUp(aParam.alPage._$sub_menu, (this._$items.length - 1));
                        this.setItemFocus((this._$items.length - 1));
                    }
                    else {
                        if (this._$items[i - 1].attr('class') !== '') {
                            this.setListScrollUp(aParam.alPage._$sub_menu, i - 1);
                            this.setItemFocus(i - 1);
                        }
                        else {
                            this.setItemFocus(i - 1);
                        }
                    }
                    break;
                }
            }
        };
        tvOptionSubmenu.prototype._doOk = function (aParam) {
            var i;
            for (i = 0; i < this._$items.length; i += 1) {
                if (this._$items[i].is('.on')) {
                    aParam.alEvent = 'alClicked';
                    aParam.alData = {
                        index: i,
                        data: this._main_menu[i]
                    };
                    aParam.alData.me = this;
                    this.sendEventToParent(aParam);
                    break;
                }
            }
        };
        tvOptionSubmenu.prototype._doPrev = function (aParam) {
            aParam.alEvent = 'alClicked';
            aParam.alData = {};
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        return tvOptionSubmenu;
    })(__COMPONENT_KERNEL__);
    exports.tvOptionSubmenu = tvOptionSubmenu;
});
