///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cLipSync.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var lipsync = (function (_super) {
        __extends(lipsync, _super);
        function lipsync() {
            _super.call(this);
        }
        lipsync.prototype.init = function () {
            // TODO:
        };
        lipsync.prototype.create = function (aParam) {
            var src, o, i;
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
        };
        lipsync.prototype.destroy = function () {
            this.$.blur();
            this.$.remove();
        };
        // EVENT HANDLER
        lipsync.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var k = hx.key, kc = aParam.alKey;
            var o;
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
                case k.KEY_RIGHT:
                    if (this._lipsyncDelay <= 380) {
                        this._lipsyncDelay += 20;
                        this._$span.css('width', (this._lipsyncDelay / 4) + '%');
                        var showlipsync = (this._lipsyncDelay === 0) ? (this._lipsyncDelay + ' ms') : ('+ ' + this._lipsyncDelay + ' ms');
                        this._$lipDelay.html(showlipsync);
                    }
                    this.__send_event('right', aParam);
                    bConsumed = true;
                    break;
                case k.KEY_UP:
                case k.KEY_DOWN:
                    bConsumed = true;
                    break;
                case k.KEY_OK:
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
        };
        // PRIVATE FUNCTION FOR CONTENT MAKING
        lipsync.prototype._doFocus = function () {
            var that = this;
            var els = that.$.children();
            if (els.is('.on') === false) {
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
        };
        lipsync.prototype.__send_timer_refresh = function (aParam) {
            aParam = (aParam) || {};
            aParam.alEvent = 'alMessageToPage';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        lipsync.prototype.__send_event = function (aInput, aParam) {
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
        };
        return lipsync;
    })(__COMPONENT_KERNEL__);
    exports.lipsync = lipsync;
    var subtitleSync = (function (_super) {
        __extends(subtitleSync, _super);
        function subtitleSync() {
            _super.call(this);
            $.hxUpgrade(this, hx.cmpt.frame.prototype);
        }
        subtitleSync.prototype.init = function () {
            // TODO:
        };
        subtitleSync.prototype.create = function (aParam) {
            var src, o, i;
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
            });
            this._create({
                'tag': this.tag,
                'attr': this.attr
            });
            this.$.append(this._$subtitleTitle).append(this._$subtitleDelay).append(this._$graphPlus.append(this._$spanPlus)).append(this._$graphMinus.append(this._$spanMinus));
            this._syncDelay = aParam.syncDelay;
            var showSync;
            if (this._syncDelay > 0) {
                this._$spanPlus.css('width', (this._syncDelay * 10 / 3) + '%');
                showSync = '+ ' + this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
            }
            else if (this._syncDelay < 0) {
                this._$spanMinus.css('width', (((-1) * this._syncDelay) * 10 / 3) + '%');
                showSync = '- ' + ((-1) * this._syncDelay) + ' ' + hx.l('LOC_SEC_ID');
            }
            else {
                this._$spanPlus.css('width', '0%');
                this._$spanMinus.css('width', '0%');
                showSync = this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
            }
            this._$subtitleDelay.html(showSync);
            return this;
        };
        subtitleSync.prototype.destroy = function () {
            this.$.blur();
            this.$.remove();
        };
        // EVENT HANDLER
        subtitleSync.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var k = hx.key, kc = aParam.alKey;
            var o;
            var showSync;
            switch (kc) {
                case k.KEY_LEFT:
                    if (this._syncDelay <= -30) {
                        return;
                    }
                    this._syncDelay -= 0.5;
                    if (this._syncDelay > 0) {
                        this._$spanPlus.css('width', (this._syncDelay * 10 / 3) + '%');
                        showSync = '+ ' + this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
                    }
                    else if (this._syncDelay < 0) {
                        this._$spanMinus.css('width', (((-1) * this._syncDelay) * 10 / 3) + '%');
                        showSync = '- ' + ((-1) * this._syncDelay) + ' ' + hx.l('LOC_SEC_ID');
                    }
                    else {
                        this._$spanPlus.css('width', '0%');
                        this._$spanMinus.css('width', '0%');
                        showSync = this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
                    }
                    this._$subtitleDelay.html(showSync);
                    this.__send_event('left', aParam);
                    break;
                case k.KEY_RIGHT:
                    if (this._syncDelay >= 30) {
                        return;
                    }
                    this._syncDelay += 0.5;
                    if (this._syncDelay > 0) {
                        this._$spanPlus.css('width', (this._syncDelay * 10 / 3) + '%');
                        showSync = '+ ' + this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
                    }
                    else if (this._syncDelay < 0) {
                        this._$spanMinus.css('width', (((-1) * this._syncDelay) * 10 / 3) + '%');
                        showSync = '- ' + ((-1) * this._syncDelay) + ' ' + hx.l('LOC_SEC_ID');
                    }
                    else {
                        this._$spanPlus.css('width', '0%');
                        this._$spanMinus.css('width', '0%');
                        showSync = this._syncDelay + ' ' + hx.l('LOC_SEC_ID');
                    }
                    this._$subtitleDelay.html(showSync);
                    this.__send_event('left', aParam);
                    break;
                case k.KEY_UP:
                case k.KEY_DOWN:
                    bConsumed = true;
                    break;
                case k.KEY_OK:
                    this.__send_event('ok', aParam);
                    break;
                case k.KEY_BACK:
                case k.KEY_POWER:
                case k.KEY_ESCAPE:
                    return false;
            }
            this.__send_timer_refresh(aParam);
            return bConsumed;
        };
        // PRIVATE FUNCTION FOR CONTENT MAKING
        subtitleSync.prototype._doFocus = function () {
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
        };
        subtitleSync.prototype.__send_timer_refresh = function (aParam) {
            aParam = (aParam) || {};
            aParam.alEvent = 'alMessageToPage';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        subtitleSync.prototype.__send_event = function (aInput, aParam) {
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
        };
        return subtitleSync;
    })(__COMPONENT_KERNEL__);
    exports.subtitleSync = subtitleSync;
});
