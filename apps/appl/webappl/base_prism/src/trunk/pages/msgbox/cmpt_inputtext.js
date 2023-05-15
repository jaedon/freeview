///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cInputText.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var inputtext = (function (_super) {
        __extends(inputtext, _super);
        function inputtext() {
            _super.call(this);
        }
        inputtext.prototype.init = function () {
        };
        inputtext.prototype.create = function (aParam) {
            $.extend(this, aParam, {
                _input_html: $('<input class=' + 'wrap_input' + ' type=' + (aParam.type || 'text') + ' maxlength=32' + ' style=' + 'input-format:full;top:304px;>'),
                _cancel_btn: $('<p>', {
                    'class': 'button cancel'
                }).text(hx.l('LOC_CANCEL_ID')),
                _next_btn: $('<p>', {
                    'class': 'button next'
                }).text(hx.l('LOC_NEXT_ID')),
                _callback_fn: aParam.callback_fn,
                _last_focus: 0
            });
            this._focuslist = [];
            this._focuslist.length = 0;
            this._focuslist.push(this._input_html);
            this._focuslist.push(this._next_btn);
            this._focuslist.push(this._cancel_btn);
            this._create({
                'tag': 'osd',
                'attr': {
                    'class': 'pas',
                }
            });
            this.$.append($('<form method="post">').append(this._input_html).append(this._cancel_btn).append(this._next_btn));
            this.setFocus();
            return this;
        };
        inputtext.prototype.destroy = function () {
            this._next_btn.blur();
            this._cancel_btn.blur();
            this.$.remove();
        };
        inputtext.prototype.__make_btn = function (aTxt) {
            return $('<p class="button">').text(aTxt);
        };
        inputtext.prototype._on_alKey = function (aParam) {
            var result = false, _key = aParam.alKey, focusedList = this.$.children('form').children('.on'), i, index = 0;
            switch (_key) {
                case hx.key.KEY_UP:
                    for (i = 0; i < this._focuslist.length; i += 1) {
                        if (focusedList.get(0).outerHTML === this._focuslist[i].get(0).outerHTML) {
                            index = i;
                            break;
                        }
                    }
                    if (i === 0) {
                        this.setFocus(index += 1);
                    }
                    else if (i === 1 || i === 2) {
                        this.setFocus(0);
                    }
                    result = true;
                    break;
                case hx.key.KEY_DOWN:
                    for (i = 0; i < this._focuslist.length; i += 1) {
                        if (focusedList.get(0).outerHTML === this._focuslist[i].get(0).outerHTML) {
                            index = i;
                            break;
                        }
                    }
                    if (i === 0) {
                        this.setFocus(index += 1);
                    }
                    else if (i === 1 || i === 2) {
                        this.setFocus(0);
                    }
                    result = true;
                    break;
                case hx.key.KEY_LEFT:
                case hx.key.KEY_RIGHT:
                    for (i = 0; i < this._focuslist.length; i += 1) {
                        if (focusedList.get(0).outerHTML === this._focuslist[i].get(0).outerHTML) {
                            index = i;
                            break;
                        }
                    }
                    if (i === 1) {
                        this.setFocus(index += 1);
                    }
                    else if (i === 2) {
                        this.setFocus(index -= 1);
                    }
                    else {
                        return true;
                    }
                    result = true;
                    break;
                case hx.key.KEY_OK:
                    for (i = 0; i < this._focuslist.length; i += 1) {
                        if (focusedList.get(0).outerHTML === this._focuslist[i].get(0).outerHTML) {
                            index = i;
                            break;
                        }
                    }
                    if (index === 0) {
                        this._input_html.focus();
                    }
                    else {
                        this.__send_event(aParam, index);
                    }
                    result = true;
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_POWER:
                case hx.key.KEY_ESCAPE:
                    return false;
                    break;
            }
            //that.__send_timer_refresh(param);
            return result;
        };
        inputtext.prototype.setFocus = function (aIndex) {
            var i;
            var _length = this._focuslist.length;
            aIndex = (aIndex === undefined ? this._last_focus : aIndex);
            if (aIndex >= _length) {
                aIndex = 0;
            }
            else if (aIndex < 0) {
                aIndex = (_length - 1);
            }
            for (i = 0; i < this._focuslist.length; i += 1) {
                this._focuslist[i].removeClass('on');
            }
            if (aIndex === 0) {
                this._input_html.attr('class', 'wrap_input on');
            }
            else {
                this._input_html.attr('class', 'wrap_input blind');
            }
            this._focuslist[aIndex].addClass('on');
        };
        inputtext.prototype.__send_timer_refresh = function (aParam) {
            var focusedList = this.$.children('form').children('.on'), i, index = 0;
            for (i = 0; i < this._focuslist.length; i += 1) {
                if (focusedList.get(0).outerHTML === this._focuslist[i].get(0).outerHTML) {
                    index = i;
                    break;
                }
            }
            this._last_focus = index;
            aParam = (aParam) || {};
            aParam.alEvent = 'alMessageToPage';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        inputtext.prototype.__send_event = function (aParam, aIndex) {
            aParam = (aParam) || {};
            aParam.alEvent = 'alClicked';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            aParam.alData.callback_fn = this._callback_fn;
            if (aIndex === 1) {
                aParam.alData.input_title = this._next_btn.text();
                aParam.alData.input = this._input_html[0].value;
            }
            else {
                aParam.alData.input_title = 'key_back';
            }
            this.sendEventToParent(aParam);
        };
        return inputtext;
    })(__COMPONENT_KERNEL__);
    exports.inputtext = inputtext;
});
