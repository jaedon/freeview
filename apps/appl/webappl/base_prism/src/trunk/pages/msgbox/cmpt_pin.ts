///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cPin.js
/*jslint sloppy:true, nomen:true, vars:true */

/*
 * pin component
 * @param
 */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class pin extends __COMPONENT_KERNEL__ {
    pinCnt: number;
    tag: any;
    attr: any;
    _insertedKey: any;
    _$pinList: any;
    _$pinWrap: any;
    _enable: boolean;
    _currentIdx: number;
    _callback_fn: any;

    constructor() {
        super();
    }

    init() {
        // TODO:
    }

    create(aParam) {
        var src: any, o: any, i: number, text: string;
        $.extend(this, aParam, {
            _$pinList: [],
            _$pinWrap: $('<p>', {
                'class': 'wrap_pw blind'
            }),
            _enable: (aParam.enable === undefined ? aParam.enable = true : aParam.enable),
            _callback_fn: aParam.callback_fn,
            _insertedKey: '',
            pinCnt: (this.pinCnt) || 4,
            _last_focus: 0
        });
        this._create({
            'tag': this.tag,
            'attr': this.attr
        });
        if (aParam.text && (aParam.text.length > 0)) {
            text = '<p>' + aParam.text + '</p>';
        } else {
            if (aParam.first_line && (aParam.first_line.length > 0)) {
                text = '<p>' + aParam.first_line + '</p>';
            }
            if (aParam.second_line && (aParam.second_line.length > 0)) {
                text += '<p>' + aParam.second_line + '</p>';
            }
            if (aParam.html) {
                text = '<p>' + aParam.html + '</p>';
            }
        }
        this.$.append(text);
        src = $('<em>', {
            'class': 'button'
        });
        for (i = 0; i < this.pinCnt; i += 1) {
            o = src.clone();
            this._$pinList.push(o);
            this._$pinWrap.append(o);
        }
        this.$.append(this._$pinWrap);
        if (this._enable) {
            this._doFocus();
        } else {
            o = this._$pinList;
            for (i = 0; i < o.length; i += 1) {
                var obj = o[i];
                obj.addClass('pw');
            }
            this.setIncorrectPin();
        }
        return this;
    }

    destroy() {
        this.$.blur();
        this.$.remove();
    }

    getIndex(aObj): number {
        var itm: any = this._$pinList;
        var n: number;
        for (n = 0; n < itm.length; n += 1) {
            if (itm[n].is(aObj)) {
                return n;
            }
        }
        return 0;
    }

    setIncorrectPin(aDestroy?: boolean) {
        this._$pinWrap.addClass('incrt');
    }

    // EVENT HANDLER
    _on_alKey(aParam): boolean {
        var bConsumed: boolean = false;
        var k = hx.key, kc: number = aParam.alKey;
        var o: any, prevo: any;
        if ((kc >= k.KEY_0) && (kc <= k.KEY_9)) {
            if (!this._enable) {
                return true;
            }
            this._insertedKey += (kc - k.KEY_0);
            o = this._$pinList[this._insertedKey.length - 1];
            if (this.pinCnt > this._insertedKey.length) {
                o.addClass('pw');
                o.removeClass('on');
                o.next().addClass('on');
            } else {
                if (this.pinCnt === this._insertedKey.length) {
                    o.addClass('pw');
                    o.blur();
                    this.__send_event(this._insertedKey, aParam);
                }
            }
            bConsumed = true;
        } else {
            switch (kc) {
                case k.KEY_LEFT:
                    if (!this._enable) {
                        return true;
                    }
                    prevo = this._$pinList[this._insertedKey.length];
                    prevo.removeClass('on');
                    this._insertedKey = this._insertedKey.substr(0, this._insertedKey.length - 1);
                    o = this._$pinList[this._insertedKey.length];
                    o.removeClass('pw');
                    o.addClass('on');
                    bConsumed = true;
                    break;
                case k.KEY_RIGHT :
                case k.KEY_UP :
                case k.KEY_DOWN :
                case k.KEY_OK :
                    bConsumed = true;
                    break;
                case k.KEY_BACK:
                case k.KEY_POWER:
                case k.KEY_ESCAPE:
                    return false;
            }
        }
        this.__send_timer_refresh(aParam);
        return bConsumed;
    }

    // PRIVATE FUNCTION FOR CONTENT MAKING
    _doFocus() {
        var that = this;
        var els: any = that.$.children();
        if (els.hasClass('.on') === false) {
            if (that._$pinList.length < 1) {
                hx.log("error", "Must to have one or more items");
                return;
            }
            that._$pinList[that._insertedKey.length].addClass('on');
            var pinCmpt = that;
            var index = pinCmpt.getIndex(this);
            pinCmpt.sendEventToParent({
                alEvent: 'alChangeItem',
                alData: {
                    me: pinCmpt,
                    index: index
                }
            });
            pinCmpt._currentIdx = index;
        }
    }

    __send_timer_refresh(aParam) {
        aParam = (aParam) || {};
        aParam.alEvent = 'alMessageToPage';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData.me = this;
        this.sendEventToParent(aParam);
    }

    __send_event(aInput, aParam) {
        aParam = (aParam) || {};
        aParam.alEvent = 'alClicked';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData = (aParam.alData) || {};
        aParam.alData.me = this;
        aParam.alData.input = aInput;
        aParam.alData.input_title = 'stop';
        aParam.alData.callback_fn = this._callback_fn;
        this.sendEventToParent(aParam);
    }
}
