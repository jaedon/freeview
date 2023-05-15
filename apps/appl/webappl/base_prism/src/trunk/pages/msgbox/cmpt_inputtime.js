///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cInputTime.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var inputtime = (function (_super) {
        __extends(inputtime, _super);
        function inputtime() {
            _super.call(this);
        }
        inputtime.prototype.init = function () {
            // TODO:
        };
        inputtime.prototype.create = function (aParam) {
            var src, o, i;
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
                    'class': 'wrap_num dash'
                }),
                _$numWrap3: $('<em>', {
                    'class': 'wrap_num colon'
                }),
                _$numWrap4: $('<em>', {
                    'class': 'wrap_num'
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
            this.$.append(this._contents).append(this._$pinWrap);
            src = $('<span>', {
                'class': 'button def'
            });
            for (i = 0; i < this.pinCnt; i += 1) {
                o = src.clone();
                this._$pinList.push(o);
                if (i === 0 || i === 1) {
                    this._$numWrap.append(o);
                }
                else if (i === 2 || i === 3) {
                    this._$numWrap2.append(o);
                }
                else if (i === 4 || i === 5) {
                    this._$numWrap3.append(o);
                }
                else if (i === 6 || i === 7) {
                    this._$numWrap4.append(o);
                }
            }
            this._$pinWrap.append(this._$numWrap).append(this._$numWrap2).append(this._$numWrap3).append(this._$numWrap4);
            if (this._enable) {
                //that.$.focus(function() {
                this._doFocus();
            }
            else {
                o = this._$pinList;
                for (i = 0; i < o.length; i += 1) {
                    var obj = o[i];
                    obj.addClass('def');
                }
            }
            return this;
        };
        inputtime.prototype.destroy = function () {
            this.$.blur();
            this.$.remove();
        };
        inputtime.prototype.getIndex = function (aObj) {
            var itm = this._$pinList;
            var n;
            for (n = 0; n < itm.length; n += 1) {
                if (itm[n].is(aObj)) {
                    return n;
                }
            }
            return 0;
        };
        inputtime.prototype.setIncorrectPin = function (aDestroy) {
            this._$pinWrap.addClass('incrt');
            /*if (bDestroy) {
             setTimeout(function() {
             that.destroy();
             }, 500);
             }*/
        };
        // EVENT HANDLER
        inputtime.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var k = hx.key, kc = aParam.alKey;
            var o;
            var firstnumber = 0, secondnumber = 0;
            if ((kc >= k.KEY_0) && (kc <= k.KEY_9)) {
                if (!this._enable) {
                    return true;
                }
                if ((this._insertedKey.length === 0 || this._insertedKey.length === 4) && (kc - k.KEY_0) > 2) {
                    return true;
                }
                if (this._insertedKey.length === 1 || this._insertedKey.length === 5) {
                    if (this._insertedKey.length === 1) {
                        firstnumber = parseInt(this._insertedKey.substring(0, 1), 10);
                    }
                    else if (this._insertedKey.length === 5) {
                        firstnumber = parseInt(this._insertedKey.substring(4, 5), 10);
                    }
                    if (firstnumber === 2 && (kc - k.KEY_0) > 3) {
                        return true;
                    }
                }
                if ((this._insertedKey.length === 2 || this._insertedKey.length === 6) && (kc - k.KEY_0) > 5) {
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
                }
                else {
                    if (this.pinCnt === this._insertedKey.length) {
                        o.html(kc - k.KEY_0);
                        o.removeClass('def');
                        o.blur();
                        this.__send_event(this._insertedKey, aParam);
                    }
                }
                bConsumed = true;
            }
            else {
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
                    case k.KEY_RIGHT:
                    case k.KEY_UP:
                    case k.KEY_DOWN:
                        bConsumed = true;
                        break;
                    case k.KEY_OK:
                        this.__send_event(this._insertedKey, aParam);
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
        };
        // PRIVATE FUNCTION FOR CONTENT MAKING
        inputtime.prototype._doFocus = function () {
            var that = this;
            var els = that.$.children();
            if (els.is('.on') === false) {
                if (that._$pinList.length < 1) {
                    hx.log("error", "Must to have one or more items");
                    return;
                }
                that.$.children('.on').removeClass('on');
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
        inputtime.prototype.__send_timer_refresh = function (aParam) {
            aParam = (aParam) || {};
            aParam.alEvent = 'alMessageToPage';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        inputtime.prototype.__send_event = function (aInput, aParam) {
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
        };
        return inputtime;
    })(__COMPONENT_KERNEL__);
    exports.inputtime = inputtime;
    var inputonetime = (function (_super) {
        __extends(inputonetime, _super);
        function inputonetime() {
            _super.call(this);
        }
        inputonetime.prototype.init = function () {
            // TODO:
        };
        inputonetime.prototype.create = function (aParam) {
            var src, o, i;
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
                }
                else if (i === 2 || i === 3) {
                    this._$numWrap2.append(o);
                }
            }
            this._$pinWrap.append(this._$numWrap).append(this._$numWrap2);
            if (this._enable) {
                this._doFocus();
            }
            else {
                o = this._$pinList;
                for (i = 0; i < o.length; i += 1) {
                    var obj = o[i];
                    obj.addClass('def');
                }
            }
            return this;
        };
        inputonetime.prototype.destroy = function () {
            this.$.blur();
            this.$.remove();
        };
        inputonetime.prototype.getIndex = function (aObj) {
            var itm = this._$pinList;
            var n;
            for (n = 0; n < itm.length; n += 1) {
                if (itm[n].is(aObj)) {
                    return n;
                }
            }
            return 0;
        };
        inputonetime.prototype.setIncorrectPin = function (aDestroy) {
            this._$pinWrap.addClass('incrt');
            /*if (bDestroy) {
             setTimeout(function() {
             that.destroy();
             }, 500);
             }*/
        };
        // EVENT HANDLER
        inputonetime.prototype._on_alKey = function (aPparam) {
            var bConsumed = false;
            var k = hx.key, kc = aPparam.alKey;
            var o;
            var firstnumber = 0, secondnumber = 0;
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
                }
                else {
                    if (this.pinCnt === this._insertedKey.length) {
                        o.html(kc - k.KEY_0);
                        o.removeClass('def');
                        o.blur();
                        this.__send_event(this._insertedKey, aPparam);
                    }
                }
                bConsumed = true;
            }
            else {
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
                    case k.KEY_RIGHT:
                    case k.KEY_UP:
                    case k.KEY_DOWN:
                    case k.KEY_OK:
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
        };
        // PRIVATE FUNCTION FOR CONTENT MAKING
        inputonetime.prototype._doFocus = function () {
            var els = this.$.children();
            if (els.is('.on') === false) {
                if (this._$pinList.length < 1) {
                    hx.log("error", "Must to have one or more items");
                    return;
                }
                this.$.children('.on').removeClass('on');
                this._$pinList[this._insertedKey.length].addClass('on');
                var pinCmpt = this;
                //els.focus(function() {
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
        };
        inputonetime.prototype.__send_timer_refresh = function (aParam) {
            var index;
            aParam = (aParam) || {};
            aParam.alEvent = 'alMessageToPage';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        inputonetime.prototype.__send_event = function (aInput, aParam) {
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
        };
        return inputonetime;
    })(__COMPONENT_KERNEL__);
    exports.inputonetime = inputonetime;
});
