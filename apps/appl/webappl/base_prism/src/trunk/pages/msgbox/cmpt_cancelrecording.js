///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cCancelRecording.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var cancelRecording = (function (_super) {
        __extends(cancelRecording, _super);
        function cancelRecording() {
            _super.call(this);
        }
        cancelRecording.prototype.create = function (aParam) {
            var _this = this;
            hx.logadd('cCancelRecording');
            hx.log('cCancelRecording', 'create');
            var i;
            $.extend(this, aParam, {
                _title: $('<h1>', {
                    'text': aParam.title
                }),
                _text: $('<p>', {
                    'text': aParam.text
                }),
                _list: $('<article>', {
                    'class': 'stlist canlist icoRec'
                }),
                _list_arr: aParam.list_arr,
                _callback_fn: aParam.callback_fn,
                _msg_type: aParam._msg_type,
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
                if (_o._subListNum === undefined || _o._subListNum === null) {
                    _o._subListNum = 0;
                }
                this._list.append(this.setRow(_o._one, _o._two, _o._three, _o._subListNum));
            }
            if (aParam.text2) {
                this._text2 = $('<p>', {
                    'style': "margin : 10 0 -15"
                });
                this._text2[0].innerHTML = '<b>' + aParam.text2;
                this.$.append(this._title).append(this._text).append(this._text2).append(this._list);
            }
            else {
                this.$.append(this._title).append(this._text).append(this._list);
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
        cancelRecording.prototype.destroy = function () {
            this.$.remove();
        };
        cancelRecording.prototype.setRow = function (aText1, aText2, aStatus, aSubListNum) {
            var that = this, i, _o;
            var tmp = $('<p>', {
                'class': 'button'
            });
            if (aSubListNum > 1) {
                var text1 = ('<b style="max-width:200px;">' + aText1 + '</b>');
                var programText = (aSubListNum - 1 === 1) ? hx.l('LOC_PROGRAMME_ID') : hx.l('LOC_PROGRAMMES_ID');
                var subListText = ('<b class="point">' + hx.l('LOC_AND_ID') + ' ' + (aSubListNum - 1) + ' ' + programText + '</b>');
                var text2 = (' ' + '<b class="value" style="max-width:460px;">' + aText2 + ' ' + subListText + '</b>');
                _o = text1 + text2;
            }
            else {
                _o = ('<b style="max-width:660px;">' + aText1);
                if (aText2) {
                    _o += aText2;
                }
                _o += '</b>';
            }
            tmp.html(_o);
            if (aText2 && aSubListNum < 2) {
                if (aStatus) {
                    tmp.append($('<span>', {
                        'class': aStatus
                    }));
                }
            }
            return tmp;
        };
        cancelRecording.prototype.setFocus = function (aIdx) {
            aIdx = (aIdx === undefined ? this._last_focus : aIdx);
            if (this._last_focus !== aIdx) {
                this._list.children('p').eq(this._last_focus).removeClass('on');
            }
            this._list.children('p').eq(aIdx).addClass('on');
        };
        // EVENT HANDLER
        cancelRecording.prototype._on_alKey = function (aParam) {
            var tmp, index;
            var bConsumed = false;
            var kc = aParam.alKey;
            var _list_arr = this._list_arr;
            switch (kc) {
                case hx.key.KEY_UP:
                    tmp = (this._list.children('.on').prevAll('.button').length - 1);
                    if (tmp < 0) {
                        tmp = _list_arr.length - 1;
                    }
                    this.setFocus(tmp);
                    bConsumed = true;
                    break;
                case hx.key.KEY_DOWN:
                    tmp = (this._list.children('.on').prevAll('.button').length + 1);
                    if (tmp >= _list_arr.length) {
                        tmp = 0;
                    }
                    this.setFocus(tmp);
                    bConsumed = true;
                    break;
                case hx.key.KEY_OK:
                    this.__send_event(aParam);
                    bConsumed = true;
                    break;
                case hx.key.KEY_LEFT:
                case hx.key.KEY_RIGHT:
                case hx.key.KEY_STOP:
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
        cancelRecording.prototype.__send_timer_refresh = function (aParam) {
            var index;
            this._last_focus = (this._list.children('.on').prevAll('.button').length);
            aParam = (aParam) || {};
            aParam.alEvent = 'alMessageToPage';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        cancelRecording.prototype.__send_event = function (aParam) {
            var index;
            var child = this._list.children('p');
            aParam = (aParam) || {};
            aParam.alEvent = 'alClicked';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            for (index = 0; index < child.length; index += 1) {
                if (child.eq(index).is('.on')) {
                    aParam.alData.input = this._list_arr[index];
                    break;
                }
            }
            aParam.alData = (aParam.alData) || {};
            aParam.alData.me = this;
            aParam.alData.input_title = 'stop';
            aParam.alData.callback_fn = this._callback_fn;
            this.sendEventToParent(aParam);
        };
        return cancelRecording;
    })(__COMPONENT_KERNEL__);
    exports.cancelRecording = cancelRecording;
});
