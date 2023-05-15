///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cTvOptionMainbtn.js
/*jslint sloppy:true, nomen:true, vars:true */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class tvOptionMainbtn extends __COMPONENT_KERNEL__ {
    _$btn: any;
    _$btn_one: any;
    _$btn_two: any;
    _keyTable: any;
    _btnType: string;

    constructor() {
        super();
    }

    create(aParam) {
        $.extend(this, aParam);
        this._btnType = aParam;
        if (this._btnType === 'subtitle') {
            $.extend(this, {
                $: $('<article>', {
                    'class': 'wrap_btn'
                }),
                _$btn: $('<p>', {
                    'class': 'button',
                    'text': hx.l('LOC_CLOSE_ID')
                }),
                _keyTable: []
            });
            this.$.append(//
                this._$btn);
            this._keyTable[hx.key.KEY_OK] = this._doOk;
            this._keyTable[hx.key.KEY_DOWN] = this._doDown;
            this._keyTable[hx.key.KEY_UP] = this._doUp;
        } else {
            $.extend(this, {
                $: $('<article>', {
                    'class': 'wrap_btn'
                }),
                _$btn_one: $('<p>', {
                    'class': 'button',
                    'text': 'OK'
                }),
                _$btn_two: $('<p>', {
                    'class': 'button',
                    'text': 'CANCEL'
                }),
                _keyTable: []
            });
            this.$.append(//
                this._$btn_one //
            ).append(//
                this._$btn_two //
            );
            this._keyTable[hx.key.KEY_LEFT] = this._doNext;
            this._keyTable[hx.key.KEY_RIGHT] = this._doNext;
            this._keyTable[hx.key.KEY_OK] = this._doOk;
            this._keyTable[hx.key.KEY_DOWN] = this._doDown;
            this._keyTable[hx.key.KEY_UP] = this._doUp;
        }
        //this._keyTable = [];
        return this;
    }

    destroy() {
        this.$.remove();
    }

    clearFocus() {
        this.$.children(".on").removeClass('on');
    }

    setItemFocus(aIndex: number) {
        if (0 > aIndex) {
            return;
        }
        if (this._btnType === 'subtitle') {
            this._$btn.addClass('on');
        } else {
            if (aIndex === 0) {
                this._$btn_two.removeClass('on');
                this._$btn_one.addClass('on');
            } else {
                this._$btn_one.removeClass('on');
                this._$btn_two.addClass('on');
            }
        }
    }

    _on_alKey(aParam: {alKey: number; alEvent: string}): boolean {
        var bConsumed: boolean = false;
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
    }

    _doDown(aParam) {
        this.$.children(".on").removeClass('on');
        this.sendEventToParent({
            alEvent: 'alChangeFocus',
            alData: {
                me: this,
                position: 'down'
            }
        });
    }

    _doUp(aParam) {
        this.$.children(".on").removeClass('on');
        this.sendEventToParent({
            alEvent: 'alChangeFocus',
            alData: {
                me: this,
                position: 'up'
            }
        });
    }

    _doNext(aParam) {
        if (this.$.children('.on').is(this._$btn_one)) {
            this.setItemFocus(1);
        } else {
            this.setItemFocus(0);
        }
    }

    _doOk(aParam: {alData: any; alEvent: string}) {
        aParam.alEvent = 'alChange';
        aParam.alData = {};
        aParam.alData.me = this;
        if (this.$.children('.on').is(this._$btn_one)) {
            aParam.alData.status = 'save';
        } else {
            aParam.alData.status = 'cancel';
        }
        this.sendEventToParent(aParam);
    }
}
