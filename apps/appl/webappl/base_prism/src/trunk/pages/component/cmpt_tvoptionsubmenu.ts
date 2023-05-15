///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cTvOptionSubmenu.js
/*jslint sloppy:true, nomen:true, vars:true */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class tvOptionSubmenu extends __COMPONENT_KERNEL__ {
    _$items: any;
    _keyTable: any;
    _main_menu: any;
    parent_data: any;

    constructor() {
        super();
    }

    create() {
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
        this.$.focus(()=> {
            this.setItemFocus(0);
        });
        return this;
    }

    setUlClass(aCls) {
        this.$.attr('class', aCls);
    }

    destroy() {
        this.$.remove();
    }

    getIndex(aObj): number {
        var itm: any = this._$items;
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

    setItemFocus(aIndex: number) {
        if (this._$items.length < aIndex) {
            return;
        }
        if (0 > aIndex) {
            return;
        }
        //this._$items[index].focus();
        this._$items[aIndex].addClass('on');
    }

    setItems(aData: any) {
        var i: number;
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
    }

    setParentData(aDdata) {
        this.parent_data = aDdata;
    }

    setListScroll(_$sub_menu: any, aIndex: number) {
        var i: number, _o: any;
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
        } else if (aIndex < (this._$items.length - 1)) {
            _$sub_menu.attr('class', 'dep2 rol scrol');
            for (i = (aIndex - 5); i <= (aIndex + 1); i += 1) {
                _o = this._$items[i];
                this.$.append(_o);
            }
            //that.$.children('li').eq(0).attr('class', 'dim30');
            //that.$.children('li').eq(6).attr('class', 'dim30');
            this.$.children('li').eq(0).addClass('dim30');
            this.$.children('li').eq(6).addClass('dim30');
        } else {
            _$sub_menu.attr('class', 'dep2 rol scrol');
            for (i = (aIndex - 5); i <= aIndex; i += 1) {
                _o = this._$items[i];
                this.$.append(_o);
            }
            //that.$.children('li').eq(0).attr('class', 'dim30');
            this.$.children('li').eq(0).addClass('dim30');
        }
    }

    setListScrollUp(_$sub_menu: any, aIndex: number) {
        var i, _o: any;
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
        } else if (needScroll === true && aIndex > 0) {
            _$sub_menu.attr('class', 'dep2 rol scrol');
            for (i = (aIndex - 1); i <= (aIndex + 5); i += 1) {
                _o = this._$items[i];
                this.$.append(_o);
            }
            //that.$.children('li').eq(0).attr('class', 'dim30');
            //that.$.children('li').eq(6).attr('class', 'dim30');
            this.$.children('li').eq(0).addClass('dim30');
            this.$.children('li').eq(6).addClass('dim30');
        } else {
            _$sub_menu.attr('class', 'dep2 rol');
            for (i = 0; i <= (aIndex + 5); i += 1) {
                _o = this._$items[i];
                this.$.append(_o);
            }
            //that.$.children('li').eq(5).attr('class', 'dim30');
            this.$.children('li').eq(5).addClass('dim30');
        }
    }

    _on_alKey(aParam: {alKey: number}): boolean {
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
    }

    _doDown(aParam) {
        var i: number;
        for (i = 0; i < this._$items.length; i += 1) {
            if (this._$items[i].is('.on')) {
                this._$items[i].removeClass('on');
                if (i === (this._$items.length - 1)) {
                    this.setListScroll(aParam.alPage._$sub_menu, 0);
                    this.setItemFocus(0);
                } else {
                    if (this._$items[i + 1].attr('class') !== '') {
                        this.setListScroll(aParam.alPage._$sub_menu, i + 1);
                        this.setItemFocus(i + 1);
                    } else {
                        this.setItemFocus(i + 1);
                    }
                }
                break;
            }
        }
    }

    _doUp(aParam) {
        var i: number;
        for (i = 0; i < this._$items.length; i += 1) {
            if (this._$items[i].is('.on')) {
                this._$items[i].removeClass('on');
                if (i === 0) {
                    this.setListScrollUp(aParam.alPage._$sub_menu, (this._$items.length - 1));
                    this.setItemFocus((this._$items.length - 1));
                } else {
                    if (this._$items[i - 1].attr('class') !== '') {
                        this.setListScrollUp(aParam.alPage._$sub_menu, i - 1);
                        this.setItemFocus(i - 1);
                    } else {
                        this.setItemFocus(i - 1);
                    }
                }
                break;
            }
        }
    }

    _doOk(aParam: {alData: any; alEvent: string}) {
        var i: number;
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
    }

    _doPrev(aParam: {alData: any; alEvent: string}) {
        aParam.alEvent = 'alClicked';
        aParam.alData = {};
        aParam.alData.me = this;
        this.sendEventToParent(aParam);
    }
}
