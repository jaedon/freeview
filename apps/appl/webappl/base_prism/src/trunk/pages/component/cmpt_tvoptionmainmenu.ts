///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cTvOptionMainmenu.js
/*jslint sloppy:true, nomen:true, vars:true */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class tvOptionMainmenu extends __COMPONENT_KERNEL__ {
    _$items: any;
    _keyTable: any;
    _main_menu: any;

    constructor() {
        super();
    }

    create() {
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
    }

    destroy() {
        this.$.remove();
    }

    getIndex(aObj: any): number {
        var itm = this._$items;
        var n: number;
        for (n = 0; n < itm.length; n += 1) {
            if (itm[n].is(aObj)) {
                return n;
            }
        }
        return 0;
    }

    getItem(aIndex: number) {
        if (this._$items.length < aIndex) {
            return undefined;
        }
        if (0 > aIndex) {
            return undefined;
        }
        return this._$items[aIndex];
    }

    setItemFocus(aIndex: number): boolean {
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
    }

    disableItem(aIndex: number): boolean {
        if (this._$items.length < aIndex) {
            return;
        }
        if (0 > aIndex) {
            return;
        }
        this._$items[aIndex].addClass('dis');
    }

    enableItem(aIndex: number) {
        if (this._$items.length < aIndex) {
            return;
        }
        if (0 > aIndex) {
            return;
        }
        this._$items[aIndex].removeClass('dis');
    }

    isDisabledItem(aIndex: number): boolean {
        if (this._$items.length < aIndex) {
            return false;
        }
        if (0 > aIndex) {
            return false;
        }
        return this._$items[aIndex].hasClass('dis');
    }

    setItemChange(aIndex: number, aData) {
        this._$items[aIndex].find('em').text(aData.value);
    }

    setData(aData) {
        var i: number;
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
    }

    _on_alKey(aParam): boolean {
        var bConsumed: boolean = false;
        var fx: any = this._keyTable[aParam.alKey];
        if (fx) {
            fx.call(this, aParam);
            bConsumed = true;
            // send key update
            this.sendEventToParent({
                alEvent: 'alUpdate'
            });
        }
        return bConsumed;
    }

    _doDown(aParam) {
        var _len: number = this.$.children(".on").prevAll('.focusable').length;
        this.$.children(".on").removeClass('on');
        var _all_len: number = this._$items.length;
        var i: number;
        _len = _len + 1;
        // skip disabled item
        if (_len < _all_len) {
            for (i = _len; i < _all_len; i += 1) {
                if (this.isDisabledItem(_len) === true) {
                    _len = _len + 1;
                } else {
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
        } else {
            this.setItemFocus(_len);
        }
    }

    _doUp(aParam) {
        var i: number;
        var _len: number = this.$.children(".on").prevAll('.focusable').length;
        this.$.children(".on").removeClass('on');
        var _all_len: number = this._$items.length;
        _len = _len - 1;
        // skip disabled item
        if (_len >= 0) {
            for (i = _len; i >= 0; i += 1) {
                if (this.isDisabledItem(_len) === true) {
                    _len = _len - 1;
                } else {
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
        } else {
            this.setItemFocus(_len);
        }
    }

    _doOk(aParam: {alData: any; alEvent: string}) {
        var o: any = this.$.children(".on");
        var i: number = o.prevAll('.focusable').length;
        var data: any = this._main_menu[i];
        aParam.alEvent = 'alClicked';
        aParam.alData = {
            index: i,
            data: data
        };
        aParam.alData.me = this;
        this.sendEventToParent(aParam);
    }
}
