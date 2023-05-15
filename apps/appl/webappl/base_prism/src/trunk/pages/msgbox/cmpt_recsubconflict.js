///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cRecSubConflict.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var recordingSubConflict = (function (_super) {
        __extends(recordingSubConflict, _super);
        function recordingSubConflict() {
            _super.call(this);
        }
        recordingSubConflict.prototype.create = function (aParam) {
            var _this = this;
            hx.logadd('recordingSubConflict');
            hx.log('recordingSubConflict', 'create');
            var i;
            $.extend(this, aParam, {
                _title: $('<h1>', {
                    'text': aParam.title
                }),
                _text: $('<p>', {
                    'text': aParam.text
                }),
                _text2: $('<span>', {
                    'text': aParam.text2
                }),
                _listWrapper: $('<article>', {
                    'class': 'box icoRec'
                }),
                _list: $('<ul>', {
                    'class': 'conli'
                }),
                _list_arr: aParam.list_arr,
                _btn_title_arr: aParam.btn_title_arr,
                _callback_fn: aParam.callback_fn,
                _last_focus: 0
            });
            this._create({
                'tag': 'div',
                'attr': {
                    'class': 'con vaBot'
                }
            });
            if (typeof (this._list_arr) === 'string') {
                this._list_arr = [this._list_arr];
            }
            for (i = 0; i < this._list_arr.length; i += 1) {
                var _o = this._list_arr[i];
                this._list.append(this.setRow(_o._one, _o._two));
            }
            if (aParam.text2) {
                this._text.append(this._text2);
            }
            this.$.append(this._title).append(this._text).append(this._listWrapper.append(this._list));
            for (i = 0; i < this._btn_title_arr.length; i += 1) {
                this.$.append(this._set_button(this._btn_title_arr[i]));
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
        recordingSubConflict.prototype.destroy = function () {
            this.$.remove();
        };
        recordingSubConflict.prototype.setRow = function (aText1, aStatus) {
            var tmp = $('<li>', {
                'text': aText1
            });
            if (aText1) {
                if (aStatus) {
                    tmp.append($('<span>', {
                        'class': aStatus
                    }));
                }
            }
            return tmp;
        };
        recordingSubConflict.prototype._set_button = function (aTxt) {
            return $('<p>', {
                'class': 'button',
                'text': aTxt
            });
        };
        recordingSubConflict.prototype.setFocus = function (aIdx) {
            aIdx = (aIdx === undefined ? this._last_focus : aIdx);
            if (this._last_focus !== aIdx) {
                this.$.children('.button').eq(this._last_focus).removeClass('on');
            }
            this.$.children('.button').eq(aIdx).addClass('on');
        };
        // EVENT HANDLER
        recordingSubConflict.prototype._on_alKey = function (aParam) {
            var tmp, index;
            var bConsumed = false;
            var kc = aParam.alKey;
            var _list_arr = this._list_arr;
            switch (kc) {
                case hx.key.KEY_UP:
                case hx.key.KEY_DOWN:
                case hx.key.KEY_CHANNEL_UP:
                case hx.key.KEY_CHANNEL_DOWN:
                    bConsumed = true;
                    break;
                case hx.key.KEY_OK:
                    this.__send_event(aParam);
                    bConsumed = true;
                    break;
                case hx.key.KEY_LEFT:
                    for (index = 0; index < this._btn_title_arr.length; index += 1) {
                        if (this.$.children('.button').eq(index).is('.on')) {
                            tmp = index - 1;
                            if (tmp < 0) {
                                tmp = (this._btn_title_arr.length - 1);
                            }
                            this.setFocus(tmp);
                            break;
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_RIGHT:
                    for (index = 0; index < this._btn_title_arr.length; index += 1) {
                        if (this.$.children('.button').eq(index).is('.on')) {
                            tmp = index + 1;
                            if (tmp >= this._btn_title_arr.length) {
                                tmp = 0;
                            }
                            this.setFocus(tmp);
                            break;
                        }
                    }
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
        recordingSubConflict.prototype.__send_timer_refresh = function (aParam) {
            this._last_focus = (this.$.children('.on').prevAll('.button').length);
            aParam = (aParam) || {};
            aParam.alEvent = 'alMessageToPage';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        recordingSubConflict.prototype.__send_event = function (aParam) {
            var index;
            var child = this.$.children('.button');
            aParam = (aParam) || {};
            aParam.alEvent = 'alClicked';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            for (index = 0; index < child.length; index += 1) {
                if (child.eq(index).is('.on')) {
                    aParam.alData.input = index;
                    aParam.alData.input_title = this._btn_title_arr[index];
                    break;
                }
            }
            aParam.alData = (aParam.alData) || {};
            aParam.alData.me = this;
            aParam.alData.callback_fn = this._callback_fn;
            this.sendEventToParent(aParam);
        };
        return recordingSubConflict;
    })(__COMPONENT_KERNEL__);
    exports.recordingSubConflict = recordingSubConflict;
});
