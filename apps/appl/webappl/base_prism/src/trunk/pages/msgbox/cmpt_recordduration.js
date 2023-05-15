///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cRecordDuration.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var recordDuration = (function (_super) {
        __extends(recordDuration, _super);
        function recordDuration() {
            _super.call(this);
        }
        recordDuration.prototype.create = function (aParam) {
            var _this = this;
            hx.logadd('cRecordDuration');
            hx.log('cRecordDuration', 'create');
            $.extend(this, aParam, {
                _title: $('<h1>', {
                    'text': aParam.title
                }),
                _text: $('<p>', {
                    'text': aParam.text
                }),
                _p_text: $('<p>', {
                    'class': 'culine icoRec'
                }).append($('<span>', {
                    'class': 'rec'
                })).append(aParam.p_text),
                _list: $('<article>', {
                    'class': 'stlist'
                }),
                _f: $('<p>', {}),
                _f_t: $('<em>', {
                    'text': aParam.f_text
                }),
                _f_l: $('<em>', {
                    'class': 'dul'
                }),
                _f_r: $('<em>', {
                    'class': 'dur'
                }),
                _f_arrow: aParam.f_arrow,
                _s_show: aParam.s_show,
                _f_step: aParam.f_step,
                _callback_fn: aParam.callback_fn,
                _last_focus: 0
            });
            if (this._s_show == undefined) {
                this._s_show = true;
            }
            if (this._s_show) {
                $.extend(this, {
                    _s: $('<p>', {}),
                    //_s_t : $('<em>', {
                    //    'text' : param.s_text
                    //}),
                    _s_t: $('<em>').html('<b style="max-width:460px;">' + aParam.s_text + '</b>' + aParam.s_time),
                    _s_arrow: aParam.s_arrow,
                });
            }
            this._create({
                'tag': 'div',
                'attr': {
                    'class': 'con vaBot'
                }
            });
            if (!this.f_step) {
                this.f_step = '30';
            }
            this._f_l.text('-' + this.f_step + 'mins');
            this._f_r.text('+' + this.f_step + 'mins');
            this.$.append(this._title).append(this._text).append(this._p_text);
            this.$.append(this._list.append(this._setDurationArrow(this._f_arrow, this._f, this._f_t, this._f_l, this._f_r)));
            if (this._s_show) {
                this.$.append(this._list.append(this._setDurationArrow(this._s_arrow, this._s, this._s_t)));
            }
            this.$.focus(function () {
                _this.setFocus();
            });
            hx.il.ani.action({
                type: 'in',
                targets: this.$
            });
            return this;
        };
        recordDuration.prototype.destroy = function () {
            this.$.remove();
        };
        recordDuration.prototype.setFocus = function (aIds) {
            aIds = (aIds === undefined ? this._last_focus : aIds);
            if (aIds === 0 || !this._s_show) {
                if (aIds !== this._last_focus) {
                    this._s.removeClass('on');
                }
                this._f.addClass('on');
            }
            else {
                if (aIds !== this._last_focus) {
                    this._f.removeClass('on');
                }
                this._s.addClass('on');
            }
        };
        recordDuration.prototype.setListTextArrow = function (aPosition, aText, aArrow, aTime) {
            if (aPosition === 'first' || !this._s_show) {
                this._f_t.text(aText);
                this._setDurationArrow(aArrow, this._f, this._f_t, this._f_l, this._f_r);
                this.setFocus(0);
            }
            else {
                //second
                //that._s_t.text(_text);
                this._s_t.html('<b style="max-width:460px;">' + aText + '</b>' + (aTime === undefined ? '' : aTime));
                this._setDurationArrow(aArrow, this._s, this._s_t);
                this.setFocus(1);
            }
        };
        recordDuration.prototype._setDurationArrow = function (aStatus, _$p, _$text, _$left, _$right) {
            if (_$p.children().length === 0) {
                _$p.attr('class', 'button arrL arrR');
                _$p.append(_$left).append(_$text).append(_$right);
            }
            if (_$left) {
                _$left.css('visibility', 'visible');
            }
            if (_$right) {
                _$right.css('visibility', 'visible');
            }
            if (aStatus === 'left') {
                _$p.attr('class', 'button arrL');
                if (_$right) {
                    _$right.css('visibility', 'hidden');
                }
            }
            else if (aStatus === 'right') {
                _$p.attr('class', 'button arrR');
                if (_$left) {
                    _$left.css('visibility', 'hidden');
                }
            }
            else {
                _$p.attr('class', 'button arrL arrR');
            }
            return _$p;
        };
        recordDuration.prototype._on_alKey = function (aParam) {
            var tmp, index;
            var bConsumed = false;
            var kc = aParam.alKey;
            tmp = this._list.children('.on').prevAll('.button').length;
            switch (kc) {
                case hx.key.KEY_UP:
                case hx.key.KEY_DOWN:
                    if (!this._s_show) {
                        this.setFocus(0);
                    }
                    else {
                        if (tmp === 0) {
                            this.setFocus(1);
                        }
                        else {
                            this.setFocus(0);
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_LEFT:
                    if (tmp === 0) {
                        if (this._f.hasClass('arrL')) {
                            aParam.alData = (aParam.alData) || {};
                            aParam.alData.input = this._f_t.text();
                            aParam.alData.input_title = 'first_left';
                            this.__send_event(aParam);
                        }
                    }
                    else {
                        if (this._s.hasClass('arrL')) {
                            aParam.alData = (aParam.alData) || {};
                            aParam.alData.input = this._s_t.text();
                            aParam.alData.input_title = 'second_left';
                            this.__send_event(aParam);
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_RIGHT:
                    if (tmp === 0) {
                        if (this._f.hasClass('arrR')) {
                            aParam.alData = (aParam.alData) || {};
                            aParam.alData.input = this._f_t.text();
                            aParam.alData.input_title = 'first_right';
                            aParam.alData.close = false;
                            this.__send_event(aParam);
                        }
                    }
                    else {
                        if (this._s.hasClass('arrR')) {
                            aParam.alData = (aParam.alData) || {};
                            aParam.alData.input = this._s_t.text();
                            aParam.alData.input_title = 'second_right';
                            aParam.alData.close = false;
                            this.__send_event(aParam);
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_OK:
                    aParam.alData = (aParam.alData) || {};
                    if (this._s_show) {
                        aParam.alData.input = {
                            'first': this._f_t.text(),
                            'second': this._s_t.text(),
                            'index': tmp
                        };
                    }
                    else {
                        aParam.alData.input = {
                            'first': this._f_t.text(),
                            'second': '',
                            'index': tmp
                        };
                    }
                    aParam.alData.input_title = 'ok';
                    aParam.alData.close = true;
                    this.__send_event(aParam);
                    bConsumed = true;
                    break;
                case hx.key.KEY_CHANNEL_UP:
                case hx.key.KEY_CHANNEL_DOWN:
                case hx.key.KEY_RECORD:
                    bConsumed = true;
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_POWER:
                case hx.key.KEY_ESCAPE:
                    return false;
                    break;
            }
            this.__send_timer_refresh(aParam);
            return bConsumed;
        };
        recordDuration.prototype.__send_timer_refresh = function (aPparam) {
            this._last_focus = this._list.children('.on').prevAll('.button').length;
            aPparam = (aPparam) || {};
            aPparam.alEvent = 'alMessageToPage';
            if (aPparam.alData === undefined) {
                aPparam.alData = {};
            }
            aPparam.alData.me = this;
            this.sendEventToParent(aPparam);
        };
        recordDuration.prototype.__send_event = function (aParam) {
            aParam = (aParam) || {};
            aParam.alEvent = 'alClicked';
            aParam.alData.me = this;
            aParam.alData.callback_fn = this._callback_fn;
            this.sendEventToParent(aParam);
        };
        return recordDuration;
    })(__COMPONENT_KERNEL__);
    exports.recordDuration = recordDuration;
});
