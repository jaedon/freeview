///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cLipSync.js
/* jslint sloppy:true, nomen:true, vars:true */

/*
 * lipsync component
 * @param
 */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class lipsync extends __COMPONENT_KERNEL__ {
    tag: any;
    attr: any;
    getIndex: any;
    _lipsyncDelay: number;
    _$pinList: any;
    _$lipDelay: any;
    _$lipTitle: any;
    _$span: any;
    _$graph: any;
    _insertedKey: any;
    _currentIdx: number;
    _callback_fn: any;

    constructor() {
        super();
    }

    init() {
        // TODO:
    }

    create(aParam: any) {
        var src: any, o: any, i: number;
        $.extend(this, aParam, {
            _$lipTitle: $('<dt>', {
                'class': 'lipTitle line1',
                'html': hx.l('LOC_LIPSYNC_DELAY_ID')
            }),
            _$lipDelay: $('<dd>', {
                'class': 'lipDelay',
                'html': ''
            }),
            _$graph: $('<dd>', {
                'class': 'graph'
            }),
            _$span: $('<span>', {
                'style': 'width:0%'
            }),
            _callback_fn: aParam.callback_fn,
            // _insertedKey : '',
            // pinCnt : (that.pinCnt) || 4,
            // _last_focus : 0
        });
        this._create({
            'tag': this.tag,
            'attr': this.attr
        });
        this.$.append(this._$lipTitle).append(this._$lipDelay).append(this._$graph.append(this._$span));
        this._lipsyncDelay = aParam.lipsyncDelay;
        this._$span.css('width', (this._lipsyncDelay / 4) + '%');
        var showlipsync = (this._lipsyncDelay === 0) ? (this._lipsyncDelay + ' ms') : ('+ ' + this._lipsyncDelay + ' ms');
        this._$lipDelay.html(showlipsync);
        return this;
    }

    destroy() {
        this.$.blur();
        this.$.remove();
    }

    // EVENT HANDLER
    _on_alKey(aParam: any): boolean {
        var bConsumed: boolean = false;
        var k: any = hx.key, kc: number = aParam.alKey;
        var o: any;
        switch (kc) {
            case k.KEY_LEFT:
                if (this._lipsyncDelay >= 20) {
                    this._lipsyncDelay -= 20;
                    this._$span.css('width', (this._lipsyncDelay / 4) + '%');
                    var showlipsync = (this._lipsyncDelay === 0) ? (this._lipsyncDelay + ' ms') : ('+ ' + this._lipsyncDelay + ' ms');
                    this._$lipDelay.html(showlipsync);
                }
                this.__send_event('left', aParam);
                bConsumed = true;
                break;
            case k.KEY_RIGHT :
                if (this._lipsyncDelay <= 380) {
                    this._lipsyncDelay += 20;
                    this._$span.css('width', (this._lipsyncDelay / 4) + '%');
                    var showlipsync = (this._lipsyncDelay === 0) ? (this._lipsyncDelay + ' ms') : ('+ ' + this._lipsyncDelay + ' ms');
                    this._$lipDelay.html(showlipsync);
                }
                this.__send_event('right', aParam);
                bConsumed = true;
                break;
            case k.KEY_UP :
            case k.KEY_DOWN :
                bConsumed = true;
                break;
            case k.KEY_OK :
                this.__send_event('ok', aParam);
                bConsumed = true;
                break;
            case k.KEY_BACK:
            case k.KEY_POWER:
            case k.KEY_ESCAPE:
                return false;
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
            els.focus(function () {
                var index: number = pinCmpt.getIndex(this);
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

    __send_event(aInput, aParam) {
        aParam = (aParam) || {};
        aParam.alEvent = 'alClicked';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData = (aParam.alData) || {};
        aParam.alData.me = this;
        aParam.alData.input_title = aInput;
        aParam.alData.lipsyncDelay = this._lipsyncDelay;
        aParam.alData.callback_fn = this._callback_fn;
        this.sendEventToParent(aParam);
    }
}
export class subtitleSync extends __COMPONENT_KERNEL__ {
    tag: any;
    attr: any;
    _$spanPlus: any;
    _$spanMinus: any;
    _syncDelay: any;
    _$subtitleTitle: any;
    _$subtitleDelay: any;
    _$graphPlus: any;
    _$graphMinus: any;
    _callback_fn: any;

    constructor() {
        super();
        $.hxUpgrade(this, hx.cmpt.frame.prototype);
    }

    init() {
        // TODO:
    }

    create(aParam) {
        var src: any, o: any, i: number;
        $.extend(this, aParam, {
            _$subtitleTitle: $('<dt>', {
                'class': 'subtitleTitle line1',
                'html': hx.l('LOC_SYNC_SUBTITLE_ID')
            }),
            _$subtitleDelay: $('<dd>', {
                'class': 'subtitleDelay',
                'html': ''
            }),
            _$graphPlus: $('<dd>', {
                'class': 'graph plus'
            }),
            _$spanPlus: $('<span>', {
                'style': 'width:0%'
            }),
            _$graphMinus: $('<dd>', {
                'class': 'graph minus'
            }),
            _$spanMinus: $('<span>', {
                'style': 'width:0%'
            }),
            _callback_fn: aParam.callback_fn,
            // _insertedKey : '',
            // pinCnt : (that.pinCnt) || 4,
            // _last_focus : 0
        });
        this._create({
            'tag': this.tag,
            'attr': this.attr
        });
        this.$.append(this._$subtitleTitle).append(this._$subtitleDelay).append(this._$graphPlus.append(this._$spanPlus)).append(this._$graphMinus.append(this._$spanMinus));
        this._syncDelay = aParam.syncDelay;
        var showSync: string;
        if (this._syncDelay > 0) {
            this._$spanPlus.css('width', (this._syncDelay * 10 / 3) + '%');
            showSync = '+ ' + this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
        } else if (this._syncDelay < 0) {
            this._$spanMinus.css('width', (((-1) * this._syncDelay) * 10 / 3) + '%');
            showSync = '- ' + ((-1) * this._syncDelay) + ' ' + hx.l('LOC_SEC_ID');
        } else {
            this._$spanPlus.css('width', '0%');
            this._$spanMinus.css('width', '0%');
            showSync = this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
        }
        this._$subtitleDelay.html(showSync);
        return this;
    }

    destroy() {
        this.$.blur();
        this.$.remove();
    }

    // EVENT HANDLER
    _on_alKey(aParam) {
        var bConsumed: boolean = false;
        var k: any = hx.key, kc: number = aParam.alKey;
        var o: any;
        var showSync: string;
        switch (kc) {
            case k.KEY_LEFT:
                if (this._syncDelay <= -30) {
                    return;
                }
                this._syncDelay -= 0.5;
                if (this._syncDelay > 0) {
                    this._$spanPlus.css('width', (this._syncDelay * 10 / 3) + '%');
                    showSync = '+ ' + this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
                } else if (this._syncDelay < 0) {
                    this._$spanMinus.css('width', (((-1) * this._syncDelay) * 10 / 3) + '%');
                    showSync = '- ' + ((-1) * this._syncDelay) + ' ' + hx.l('LOC_SEC_ID');
                } else {
                    this._$spanPlus.css('width', '0%');
                    this._$spanMinus.css('width', '0%');
                    showSync = this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
                }
                this._$subtitleDelay.html(showSync);
                this.__send_event('left', aParam);
                break;
            case k.KEY_RIGHT :
                if (this._syncDelay >= 30) {
                    return;
                }
                this._syncDelay += 0.5;
                if (this._syncDelay > 0) {
                    this._$spanPlus.css('width', (this._syncDelay * 10 / 3) + '%');
                    showSync = '+ ' + this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
                } else if (this._syncDelay < 0) {
                    this._$spanMinus.css('width', (((-1) * this._syncDelay) * 10 / 3) + '%');
                    showSync = '- ' + ((-1) * this._syncDelay) + ' ' + hx.l('LOC_SEC_ID');
                } else {
                    this._$spanPlus.css('width', '0%');
                    this._$spanMinus.css('width', '0%');
                    showSync = this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
                }
                this._$subtitleDelay.html(showSync);
                this.__send_event('left', aParam);
                break;
            case k.KEY_UP :
            case k.KEY_DOWN :
                bConsumed = true;
                break;
            case k.KEY_OK :
                this.__send_event('ok', aParam);
                break;
            case k.KEY_BACK:
            case k.KEY_POWER:
            case k.KEY_ESCAPE:
                return false;
        }
        this.__send_timer_refresh(aParam);
        return bConsumed;
    }

    // PRIVATE FUNCTION FOR CONTENT MAKING
    _doFocus() {
        /*
         var that = this;
         var els = that.$.children();
         if (els.is('.on') === false) {
         if (that._$pinList.length < 1) {
         hx.log("error", "Must to have one or more items");
         return;
         }

         that._$pinList[that._insertedKey.length].addClass('on');

         var pinCmpt = that;
         els.focus(function() {
         var index = pinCmpt.getIndex(this);
         pinCmpt.sendEventToParent({
         alEvent : 'alChangeItem',
         alData : {
         me : pinCmpt,
         index : index
         }
         });
         pinCmpt._currentIdx = index;
         });
         }
         */
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
        aParam.alData.input_title = aInput;
        aParam.alData.syncDelay = this._syncDelay;
        aParam.alData.callback_fn = this._callback_fn;
        this.sendEventToParent(aParam);
    }
}
