///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cInputTime.js
/* jslint sloppy:true, nomen:true, vars:true */

/*
 * inputtime component
 * @param
 */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class inputonetime extends __COMPONENT_KERNEL__ {
    attr: any;
    tag: any;
    pinCnt: number;
    _contents: any;
    _enable: boolean;
    _insertedKey: any;
    _$pinList: any;
    _$pinWrap: any;
    _$numWrap: any;
    _$numWrap2: any;
    _currentIdx: number;
    _callback_fn: any;

    constructor() {
        super();
    }

    init() {
        // TODO:
    }

    create(aParam) {
        var src: any, o: any, i: number;
        $.extend(this, aParam, {
            _contents: $('<p>', {
                'text': aParam.text
            }),
            _$pinList: [],
            _$pinWrap: $('<p>', {
                'class': 'wrap_pw'
            }),
            _$numWrap: $('<em>', {
                'class': 'wrap_num colon'
            }),
            _$numWrap2: $('<em>', {
                'class': 'wrap_num'
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
        this.$.append(this._contents).append(this._$pinWrap);
        src = $('<span>', {
            'class': 'button def'
        });
        for (i = 0; i < this.pinCnt; i += 1) {
            o = src.clone();
            this._$pinList.push(o);
            if (i === 0 || i === 1) {
                this._$numWrap.append(o);
            } else if (i === 2 || i === 3) {
                this._$numWrap2.append(o);
            }
        }
        this._$pinWrap.append(this._$numWrap).append(this._$numWrap2);
        if (this._enable) {
            this._doFocus();
        } else {
            o = this._$pinList;
            for (i = 0; i < o.length; i += 1) {
                var obj = o[i];
                obj.addClass('def');
            }
            //that.setIncorrectPin();
        }
        return this;
    }

    destroy() {
        this.$.blur();
        this.$.remove();
    }

    getIndex(aObj) {
        var itm: any = this._$pinList;
        var n: number;
        for (n = 0; n < itm.length; n += 1) {
            if (itm[n].is(aObj)) {
                return n;
            }
        }
        return 0;
    }

    setIncorrectPin(aDestroy: boolean) {
        this._$pinWrap.addClass('incrt');
        /*if (bDestroy) {
         setTimeout(function() {
         that.destroy();
         }, 500);
         }*/
    }

    // EVENT HANDLER
    _on_alKey(aPparam): boolean {
        var bConsumed: boolean = false;
        var k: any = hx.key, kc: number = aPparam.alKey;
        var o: any;
        var firstnumber: number = 0, secondnumber: number = 0;
        if ((kc >= k.KEY_0) && (kc <= k.KEY_9)) {
            if (!this._enable) {
                return true;
            }
            if (this._insertedKey.length === 0 && (kc - k.KEY_0) > 2) {
                return true;
            }
            if (this._insertedKey.length === 1) {
                firstnumber = parseInt(this._insertedKey.substring(0, 1), 10);
                if (firstnumber === 2 && (kc - k.KEY_0) > 3) {
                    return true;
                }
            }
            if (this._insertedKey.length === 2 && (kc - k.KEY_0) > 5) {
                return true;
            }
            this._insertedKey += (kc - k.KEY_0);
            o = this._$pinList[this._insertedKey.length - 1];
            if (this.pinCnt > this._insertedKey.length) {
                o.html(kc - k.KEY_0);
                o.removeClass('def');
                o.removeClass('on');
                o.next().addClass('on');
                this._$pinList[this._insertedKey.length].addClass('on');
            } else {
                if (this.pinCnt === this._insertedKey.length) {
                    o.html(kc - k.KEY_0);
                    o.removeClass('def');
                    o.blur();
                    this.__send_event(this._insertedKey, aPparam);
                }
            }
            bConsumed = true;
        } else {
            switch (kc) {
                case k.KEY_LEFT:
                    if (!this._enable) {
                        return true;
                    }
                    this._insertedKey = this._insertedKey.substr(0, this._insertedKey.length - 1);
                    o = this._$pinList[this._insertedKey.length];
                    o.html('');
                    o.addClass('def');
                    this._$pinList[this._insertedKey.length + 1].removeClass('on');
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
        this.__send_timer_refresh(aPparam);
        return bConsumed;
    }

    // PRIVATE FUNCTION FOR CONTENT MAKING
    _doFocus() {
        var els: any = this.$.children();
        if (els.is('.on') === false) {
            if (this._$pinList.length < 1) {
                hx.log("error", "Must to have one or more items");
                return;
            }
            this.$.children('.on').removeClass('on');
            this._$pinList[this._insertedKey.length].addClass('on');
            var pinCmpt = this;
            //els.focus(function() {
            var index: number = pinCmpt.getIndex(this);
            pinCmpt.sendEventToParent({
                alEvent: 'alChangeItem',
                alData: {
                    me: pinCmpt,
                    index: index
                }
            });
            pinCmpt._currentIdx = index;
            //});
        }
    }

    __send_timer_refresh(aParam: any) {
        var index: number;
        aParam = (aParam) || {};
        aParam.alEvent = 'alMessageToPage';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData.me = this;
        this.sendEventToParent(aParam);
    }

    __send_event(aInput, aParam: any) {
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
