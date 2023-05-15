///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cConfirm.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var confirm = (function (_super) {
        __extends(confirm, _super);
        function confirm() {
            _super.call(this);
        }
        confirm.prototype.create = function (aParam) {
            var _this = this;
            //hx.logadd('cConfirm');
            hx.log('cConfirm', 'create');
            var index;
            $.extend(this, aParam, {
                _contents: $('<p>', {
                    'html': aParam.text
                }),
                _btn_title_arr: aParam.btn_title_arr,
                _callback_fn: aParam.callback_fn,
                _msg_type: aParam._msg_type,
                _last_focus: 0,
                _useOtherKey: aParam.useOtherKey,
                _checkText: aParam.checkText,
                _doNotUseBackExitKey: aParam.doNotUseBackExitKey
            });
            if (aParam.btn_last_focus) {
                this._last_focus = aParam.btn_last_focus;
            }
            if (aParam.html) {
                this._contents.html(aParam.html);
            }
            if (this._msg_type === 'codeConfirm') {
                if (aParam.second_line) {
                    var secondLine = '';
                    if (aParam.nolinejump) {
                        secondLine = aParam.second_line;
                    }
                    else {
                        secondLine = '<br>' + aParam.second_line;
                    }
                    this._contents.html(aParam.first_line).append($('<em>', {
                        'class': 'regiCode'
                    }).text(aParam.regcode)).append($('<p>').html(secondLine));
                }
                if (aParam.html) {
                    this._contents.html(aParam.html);
                }
                if (aParam.showRegi) {
                    this._contents.css('font-size', '36px');
                    this._contents.find('p').css('margin-top', '15px').css('font-size', '28px');
                }
                else if (aParam.thirdLineFont) {
                    this._contents.css('font-size', '34px').css('color', '#FFFF00');
                    this._contents.find('p').css('margin-top', '15px').css('font-size', '26px').css('color', 'white');
                }
            }
            if (this._msg_type === 'ssuDialog' || this._msg_type === 'remindDialog' || this._msg_type === 'checkDialog') {
                $.extend(this, aParam, {
                    _title: $('<h1>', {
                        'text': aParam.title
                    }),
                    _table: $('<dl>', {
                        'class': 'swInfo box'
                    })
                });
            }
            var _cls = 'con vaBot';
            if (this._msg_type === 'select') {
                _cls = 'con vaBot btn2';
            }
            else if (this._msg_type === 'remindDialog') {
                _cls = 'con vaBot tmr line1';
            }
            if (aParam.isCenter) {
                _cls = '';
            }
            this._create({
                'tag': 'div',
                'attr': {
                    'class': _cls
                }
            });
            if (aParam.isCenter) {
                this.$[0].style.top = "236px";
            }
            if (this._msg_type === 'ssuDialog' || this._msg_type === 'checkDialog') {
                if (aParam.title) {
                    this.$.append(this._title);
                }
            }
            this.$.append(this._contents);
            if (this._msg_type === 'ssuDialog' || this._msg_type === 'remindDialog') {
                var _first = aParam.first_line_arr;
                if (this._msg_type === 'remindDialog') {
                    _first = aParam.first_line;
                }
                else {
                    this._$text1 = $('<p>', {
                        'text': this.text1
                    });
                    if (this.text1) {
                        this.$.append(this._$text1);
                    }
                }
                var _second = aParam.second_line_arr;
                if (_first != null) {
                    this._set_ssuDialog_list(_first, this._table);
                }
                if (_second != null) {
                    this._set_ssuDialog_list(_second, this._table);
                }
                var _third = aParam.third_line_arr;
                if (_third != null) {
                    this._set_ssuDialog_list(_third, this._table);
                }
                if (_first != null || _second != null) {
                    this.$.append(this._table.append());
                }
            }
            if (this._msg_type === 'checkDialog') {
                this._$checkBox = $('<p>', {
                    'class': 'checkBox',
                });
                var html = '<img src = "images/112_DIA_Check_Off.png"/> ' + this._checkText;
                this._$checkBox.html(html);
                this.$.append($('<br>'));
                this.$.append(this._$checkBox);
            }
            for (index = 0; index < this._btn_title_arr.length; index += 1) {
                this.$.append(this._set_button(this._btn_title_arr[index]));
            }
            //if ((param.auto_close === undefined ? param.auto_close = true : param.auto_close)) {
            //    this._setClosedTimer(param);
            //}
            this.$.focus(function () {
                _this._doFocus();
            });
            return this;
        };
        confirm.prototype.setFocus = function (aIndex) {
            this._doFocus(aIndex);
        };
        confirm.prototype.resetTimeout = function () {
            this.__send_timer_refresh();
        };
        confirm.prototype._set_ssuDialog_list = function (_arr, _obj) {
            var i, tmp;
            if (typeof (_arr) === 'string') {
                _arr = [_arr];
            }
            for (i = 0; i < _arr.length; i += 1) {
                if (i % 2 === 0) {
                    tmp = '<dt>';
                }
                else {
                    tmp = '<dd>';
                }
                _obj.append($(tmp, {
                    'text': _arr[i]
                }));
            }
        };
        confirm.prototype.destroy = function () {
            this.$.remove();
        };
        confirm.prototype._set_button = function (txt) {
            return $('<p>', {
                'class': 'button',
                'text': txt
            });
        };
        // EVENT HANDLER
        confirm.prototype._on_alKey = function (aParam) {
            var tmp, index;
            var bConsumed = false;
            var kc = aParam.alKey;
            switch (kc) {
                case hx.key.KEY_UP:
                    if ((this._msg_type === 'confirm') || (this._btn_title_arr.length < 2)) {
                        this.__send_timer_refresh(aParam);
                        return true;
                    }
                    else if (this._msg_type === 'checkDialog') {
                        if (this._$checkBox.hasClass('on')) {
                            this._$checkBox.removeClass('on');
                            this._doFocus(this._prevIdx);
                        }
                        else {
                            for (index = 0; index < this._btn_title_arr.length; index += 1) {
                                var $btn = this.$.children('.button').eq(index);
                                if ($btn.is('.on')) {
                                    this._prevIdx = index;
                                    $btn.removeClass('on');
                                    break;
                                }
                            }
                            this._$checkBox.addClass('on');
                        }
                        this.__send_timer_refresh(aParam);
                        return true;
                    }
                    for (index = 0; index < this._btn_title_arr.length; index += 1) {
                        if (this.$.children('.button').eq(index).is('.on')) {
                            tmp = index - 1;
                            if (tmp < 0) {
                                tmp = (this._btn_title_arr.length - 1);
                            }
                            this._doFocus(tmp);
                            break;
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_DOWN:
                    if ((this._msg_type === 'confirm') || (this._btn_title_arr.length < 2)) {
                        this.__send_timer_refresh(aParam);
                        return true;
                    }
                    else if (this._msg_type === 'checkDialog') {
                        if (this._$checkBox.hasClass('on')) {
                            this._$checkBox.removeClass('on');
                            this._doFocus(this._prevIdx);
                        }
                        else {
                            for (index = 0; index < this._btn_title_arr.length; index += 1) {
                                var $btn = this.$.children('.button').eq(index);
                                if ($btn.is('.on')) {
                                    this._prevIdx = index;
                                    $btn.removeClass('on');
                                    break;
                                }
                            }
                            this._$checkBox.addClass('on');
                        }
                        this.__send_timer_refresh(aParam);
                        return true;
                    }
                    for (index = 0; index < this._btn_title_arr.length; index += 1) {
                        if (this.$.children('.button').eq(index).is('.on')) {
                            tmp = index + 1;
                            if (tmp >= this._btn_title_arr.length) {
                                tmp = 0;
                            }
                            this._doFocus(tmp);
                            break;
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_LEFT:
                    if ((this._msg_type === 'select') || (this._btn_title_arr.length < 2)) {
                        this.__send_timer_refresh(aParam);
                        return true;
                    }
                    for (index = 0; index < this._btn_title_arr.length; index += 1) {
                        if (this.$.children('.button').eq(index).is('.on')) {
                            tmp = index - 1;
                            if (tmp < 0) {
                                tmp = (this._btn_title_arr.length - 1);
                            }
                            this._doFocus(tmp);
                            break;
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_RIGHT:
                    if ((this._msg_type === 'select') || (this._btn_title_arr.length < 2)) {
                        this.__send_timer_refresh(aParam);
                        return true;
                    }
                    for (index = 0; index < this._btn_title_arr.length; index += 1) {
                        if (this.$.children('.button').eq(index).is('.on')) {
                            tmp = index + 1;
                            if (tmp >= this._btn_title_arr.length) {
                                tmp = 0;
                            }
                            this._doFocus(tmp);
                            break;
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_OK:
                    this.__send_event(aParam);
                    bConsumed = true;
                    break;
                case hx.key.KEY_POWER:
                    return false;
                case hx.key.KEY_BACK:
                case hx.key.KEY_ESCAPE:
                    if (!this._doNotUseBackExitKey) {
                        return false;
                    }
                    else {
                        return true;
                    }
                    break;
                case hx.key.KEY_MENU:
                case hx.key.KEY_CHANNEL_UP:
                case hx.key.KEY_CHANNEL_DOWN:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_PLAYER:
                case hx.key.KEY_ON_DEMAND:
                    if (this._useOtherKey) {
                        return false;
                    }
                    break;
            }
            this.__send_timer_refresh(aParam);
            return bConsumed;
        };
        // PRIVATE FUNCTION FOR CONTENT MAKING
        confirm.prototype._doFocus = function (aIndex) {
            var btn = this.$.children('.button');
            btn.removeClass('on');
            btn.eq(aIndex === undefined ? this._last_focus : aIndex).addClass('on');
        };
        confirm.prototype.__send_timer_refresh = function (aParam) {
            var index;
            this._last_focus = this.$.find('.on').prevAll('.focusable').length;
            aParam = (aParam) || {};
            aParam.alEvent = 'alMessageToPage';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        confirm.prototype.__send_event = function (aParam) {
            var index;
            aParam = (aParam) || {};
            aParam.alEvent = 'alClicked';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            if (this._msg_type === 'checkDialog') {
                if (this._$checkBox.hasClass('on')) {
                    var checkImg = this._$checkBox.find('img');
                    if (checkImg.hasClass('on')) {
                        checkImg.removeClass('on');
                        checkImg.attr('src', 'images/112_DIA_Check_Off.png');
                        aParam.alData.bChecked = false;
                    }
                    else {
                        checkImg.addClass('on');
                        checkImg.attr('src', 'images/112_DIA_Check_On.png');
                        aParam.alData.bChecked = true;
                    }
                    aParam.alData.input_title = 'checkBox';
                }
            }
            for (index = 0; index < this._btn_title_arr.length; index += 1) {
                if (this.$.children('.button').eq(index).is('.on')) {
                    aParam.alData.input = index;
                    aParam.alData.input_title = this._btn_title_arr[index];
                    break;
                }
            }
            aParam.alData.me = this;
            aParam.alData.callback_fn = this._callback_fn;
            this.sendEventToParent(aParam);
        };
        return confirm;
    })(__COMPONENT_KERNEL__);
    exports.confirm = confirm;
});
