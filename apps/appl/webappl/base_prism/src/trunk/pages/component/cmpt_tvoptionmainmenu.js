///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cTvOptionMainmenu.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var tvOptionMainmenu = (function (_super) {
        __extends(tvOptionMainmenu, _super);
        function tvOptionMainmenu() {
            _super.call(this);
        }
        tvOptionMainmenu.prototype.create = function () {
            $.extend(this, {
                $: $('<ul>', {
                    'class': 'dep1'
                }),
                _$items: [],
                _keyTable: [],
                _main_menu: null
            });
            //this._keyTable[hx.key.KEY_LEFT] = this._doPrev;
            this._keyTable[hx.key.KEY_RIGHT] = this._doOk;
            this._keyTable[hx.key.KEY_OK] = this._doOk;
            this._keyTable[hx.key.KEY_DOWN] = this._doDown;
            this._keyTable[hx.key.KEY_UP] = this._doUp;
            return this;
        };
        tvOptionMainmenu.prototype.destroy = function () {
            this.$.remove();
        };
        tvOptionMainmenu.prototype.getIndex = function (aObj) {
            var itm = this._$items;
            var n;
            for (n = 0; n < itm.length; n += 1) {
                if (itm[n].is(aObj)) {
                    return n;
                }
            }
            return 0;
        };
        tvOptionMainmenu.prototype.getItem = function (aIndex) {
            if (this._$items.length < aIndex) {
                return undefined;
            }
            if (0 > aIndex) {
                return undefined;
            }
            return this._$items[aIndex];
        };
        tvOptionMainmenu.prototype.setItemFocus = function (aIndex) {
            if (this._$items.length < aIndex) {
                return false;
            }
            if (0 > aIndex) {
                return false;
            }
            if (this._$items[aIndex].hasClass('dis') === true) {
                return false;
            }
            this._$items[aIndex].addClass('on');
            return true;
        };
        tvOptionMainmenu.prototype.disableItem = function (aIndex) {
            if (this._$items.length < aIndex) {
                return;
            }
            if (0 > aIndex) {
                return;
            }
            this._$items[aIndex].addClass('dis');
        };
        tvOptionMainmenu.prototype.enableItem = function (aIndex) {
            if (this._$items.length < aIndex) {
                return;
            }
            if (0 > aIndex) {
                return;
            }
            this._$items[aIndex].removeClass('dis');
        };
        tvOptionMainmenu.prototype.isDisabledItem = function (aIndex) {
            if (this._$items.length < aIndex) {
                return false;
            }
            if (0 > aIndex) {
                return false;
            }
            return this._$items[aIndex].hasClass('dis');
        };
        tvOptionMainmenu.prototype.setItemChange = function (aIndex, aData) {
            this._$items[aIndex].find('em').text(aData.value);
        };
        tvOptionMainmenu.prototype.setData = function (aData) {
            var i;
            this._$items = [];
            this.$.find('li').remove();
            for (i = 0; i < aData.length; i += 1) {
                var _o = aData[i];
                var _li = $('<li>', {
                    'class': 'focusable'
                }).append($('<b>', {
                    'class': 'item',
                    'text': _o.name
                })).append($('<em>', {
                    'class': 'value',
                    'text': _o.value
                }));
                this._$items.push(_li);
                this.$.append(_li);
            }
            this._main_menu = aData;
        };
        tvOptionMainmenu.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var fx = this._keyTable[aParam.alKey];
            if (fx) {
                fx.call(this, aParam);
                bConsumed = true;
                // send key update
                this.sendEventToParent({
                    alEvent: 'alUpdate'
                });
            }
            return bConsumed;
        };
        tvOptionMainmenu.prototype._doDown = function (aParam) {
            var _len = this.$.children(".on").prevAll('.focusable').length;
            this.$.children(".on").removeClass('on');
            var _all_len = this._$items.length;
            var i;
            _len = _len + 1;
            // skip disabled item
            if (_len < _all_len) {
                for (i = _len; i < _all_len; i += 1) {
                    if (this.isDisabledItem(_len) === true) {
                        _len = _len + 1;
                    }
                    else {
                        break;
                    }
                }
            }
            if (_len === _all_len) {
                this.sendEventToParent({
                    alEvent: 'alChangeFocus',
                    alData: {
                        me: this
                    }
                });
            }
            else {
                this.setItemFocus(_len);
            }
        };
        tvOptionMainmenu.prototype._doUp = function (aParam) {
            var i;
            var _len = this.$.children(".on").prevAll('.focusable').length;
            this.$.children(".on").removeClass('on');
            var _all_len = this._$items.length;
            _len = _len - 1;
            // skip disabled item
            if (_len >= 0) {
                for (i = _len; i >= 0; i += 1) {
                    if (this.isDisabledItem(_len) === true) {
                        _len = _len - 1;
                    }
                    else {
                        break;
                    }
                }
            }
            if (_len < 0) {
                this.sendEventToParent({
                    alEvent: 'alChangeFocus',
                    alData: {
                        me: this
                    }
                });
            }
            else {
                this.setItemFocus(_len);
            }
        };
        tvOptionMainmenu.prototype._doOk = function (aParam) {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length;
            var data = this._main_menu[i];
            aParam.alEvent = 'alClicked';
            aParam.alData = {
                index: i,
                data: data
            };
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        return tvOptionMainmenu;
    })(__COMPONENT_KERNEL__);
    exports.tvOptionMainmenu = tvOptionMainmenu;
});
