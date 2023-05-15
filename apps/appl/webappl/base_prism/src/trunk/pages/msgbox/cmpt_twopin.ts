///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cTwoPin.js
/* jslint sloppy:true, nomen:true, vars:true */

/*
 * twopin component
 * @param
 */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class twopin extends __COMPONENT_KERNEL__ {
    pinCnt: number;
    tag: any;
    attr: any;
    _insertedKey: any;
    _$pinList: any;
    _enable: boolean;
    _$pinWrap: any;
    _$pinWrap2: any;
    _contents: any;
    _callback_fn: any;
    _currentIdx: number;

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
                'text': aParam.text,
            }),
            _$pinList: [],
            _$pinWrap: $('<p>', {
                'class': 'wrap_pw blind'
            }),
            _$pinWrap2: $('<p>', {
                'class': 'wrap_pw blind'
            }),
            _enable: (aParam.enable === undefined ? aParam.enable = true : aParam.enable),
            _callback_fn: aParam.callback_fn,
            _insertedKey: '',
            pinCnt: (this.pinCnt) || 8,
            _last_focus: 0
        });
        this._create({
            'tag': this.tag,
            'attr': this.attr
        });
        /*for (i = 0, it = that.txtList.length; i < it; i+=1) {
         that.$.append($('<p>', {
         'text' : that.txtList[i]
         }));
         }*/
        this.$.append(this._contents);
        /*if (that.noBlind) {
         that._$pinWrap.removeClass('blind');
         src = $('<em>', {
         'class' : 'button',
         'text' : 0
         });
         } else {
         src = $('<em>', {
         'class' : 'button'
         });
         }*/
        src = $('<em>', {
            'class': 'button'
        });
        this._$pinWrap.append($('<b>').html(hx.l('LOC_NEW_PASSWORD_ID')));
        this._$pinWrap2.append($('<b>').html(hx.l('LOC_VERIFY_PASSWORD_ID')));
        for (i = 0; i < this.pinCnt; i += 1) {
            o = src.clone();
            this._$pinList.push(o);
            if (i < 4) {
                this._$pinWrap.append(o);
            } else if (i >= 4) {
                this._$pinWrap2.append(o);
            }
        }
        this.$.append(this._$pinWrap).append(this._$pinWrap2);
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
        var that = this;
        that.$.blur();
        that.$.remove();
    }

    getIndex(obj) {
        var itm = this._$pinList;
        var n;
        for (n = 0; n < itm.length; n += 1) {
            if (itm[n].is(obj)) {
                return n;
            }
        }
        return 0;
    }

    setIncorrectPin(bDestroy?) {
        this._$pinWrap.addClass('incrt');
        /*if (bDestroy) {
         setTimeout(function() {
         that.destroy();
         }, 500);
         }*/
    }

    // EVENT HANDLER
    _on_alKey(aParam): boolean {
        var bConsumed: boolean = false;
        var k: any = hx.key, kc: number = aParam.alKey;
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
                if (this._insertedKey.length === 4) {
                    this._$pinList[3].removeClass('on');
                    this._$pinList[4].addClass('on');
                }
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
                    break;
            }
        }
        this.__send_timer_refresh(aParam);
        return bConsumed;
    }

    // PRIVATE FUNCTION FOR CONTENT MAKING
    _doFocus() {
        var that = this;
        var els: any = that.$.children();
        if (els.is('.on') === false) {
            if (that._$pinList.length < 1) {
                hx.log("error", "Must to have one or more items");
                return;
            }
            that._$pinList[that._insertedKey.length].addClass('on');
            var pinCmpt = that;
            var index: number = pinCmpt.getIndex(this);
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
