///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cPinPC.js
/* jslint sloppy:true, nomen:true, vars:true */

/*
 * pin component
 * @param
 */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class pinPC extends __COMPONENT_KERNEL__ {
    pinCnt: number;
    tag: any;
    attr: any;
    _insertedKey: any;
    _$pinList: any;
    _$pinWrap: any;
    _enable: boolean;
    _contents: any;
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
                'text': aParam.text,
            }),
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
        for (i = 0; i < this.pinCnt; i += 1) {
            o = src.clone();
            this._$pinList.push(o);
            this._$pinWrap.append(o);
        }
        this.$.append(this._$pinWrap);
        if (this._enable) {
            this.$.addClass('on');
            //that.$.focus(function() {
            this._doFocus();
            //});
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

    setIncorrectPin(bDestroy?) {
        this._$pinWrap.addClass('incrt');
        /*if (bDestroy) {
         setTimeout(function() {
         that.destroy();
         }, 500);
         }*/
    }

    // EVENT HANDLER
    _on_alKey(aParam: any): boolean {
        var that = this;
        var bConsumed: boolean = false;
        var k: any = hx.key, kc: number = aParam.alKey;
        var o: any, prevo: any;
        if ((kc >= k.KEY_0) && (kc <= k.KEY_9)) {
            if (!that._enable) {
                return true;
            }
            that._insertedKey += (kc - k.KEY_0);
            o = that._$pinList[that._insertedKey.length - 1];
            if (that.pinCnt > that._insertedKey.length) {
                o.addClass('pw');
                o.removeClass('on');
                o.next().addClass('on');
            } else {
                if (that.pinCnt === that._insertedKey.length) {
                    o.addClass('pw');
                    o.blur();
                    that.__send_event(that._insertedKey, aParam);
                }
            }
            bConsumed = true;
        } else {
            switch (kc) {
                case k.KEY_LEFT:
                    if (that._insertedKey.length === 0) {
                        if(hx.config.useNetflixPW && that.getParent().getCenterPageName() === 'CPgOnDemand'){
                            return true;
                        }
                        return false;
                    }
                    if (!that._enable) {
                        return true;
                    }
                    prevo = that._$pinList[that._insertedKey.length];
                    prevo.removeClass('on');
                    that._insertedKey = that._insertedKey.substr(0, that._insertedKey.length - 1);
                    o = that._$pinList[that._insertedKey.length];
                    o.removeClass('pw');
                    o.addClass('on');
                    bConsumed = true;
                    break;
                case k.KEY_RIGHT :
                    if (that._insertedKey.length === 0) {
                        if(hx.config.useNetflixPW && that.getParent().getCenterPageName() === 'CPgOnDemand'){
                            return true;
                        }
                        return false;
                    }
                    bConsumed = true;
                    break;
                case k.KEY_ESCAPE:
                    if(hx.config.useNetflixPW && hx.svc.NETFLIX.getPwStatus()){
                        return false;
                    }else{
                        bConsumed = true;
                    }
                    break;
                case k.KEY_OK :
                case k.KEY_UP :
                case k.KEY_DOWN :
                    if(hx.config.useNetflixPW && that.getParent().getCenterPageName() === 'CPgOnDemand'){
                        bConsumed = true;
                    }else{
                        return false;
                    }
                    break;
                case k.KEY_BACK:
                case k.KEY_POWER:
                case k.KEY_MENU:
                case k.KEY_CHANNEL_UP:
                case k.KEY_CHANNEL_DOWN:
                case k.KEY_RECORD:
                case k.KEY_STOP:
                case k.KEY_SUBTITLE:
                case k.KEY_SURROUND_MODE_NEXT:
                case k.KEY_SCREEN_MODE_NEXT:
                case k.KEY_TELETEXT:
                case k.KEY_OPTION:
                case k.KEY_GUIDE:
                case k.KEY_PLAYER:
                case k.KEY_ON_DEMAND:
                    return false;
                    break;
            }
            if (hx.config.netflix) {
                switch(kc) {
                    case k.KEY_NETFLIX:
                        return false;
                }
            }
        }
        that.__send_timer_refresh(aParam);
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
            els.focus(function () {
                var index = pinCmpt.getIndex(this);
                pinCmpt.sendEventToParent({
                    alEvent: 'alChangeItem',
                    alData: {
                        me: pinCmpt,
                        index: index
                    }
                });
                pinCmpt._currentIdx = index;
            });
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

    __send_event(input, aParam) {
        aParam = (aParam) || {};
        aParam.alEvent = 'alClicked';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData = (aParam.alData) || {};
        aParam.alData.me = this;
        aParam.alData.input = input;
        aParam.alData.input_title = 'stop';
        aParam.alData.callback_fn = this._callback_fn;
        this.sendEventToParent(aParam);
    }
}
