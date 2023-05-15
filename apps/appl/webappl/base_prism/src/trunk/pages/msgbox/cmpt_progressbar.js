///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cProgressBar.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var progressBar = (function (_super) {
        __extends(progressBar, _super);
        function progressBar() {
            _super.call(this);
        }
        progressBar.prototype.init = function () {
            // TODO:
        };
        progressBar.prototype.create = function (aParam) {
            var _this = this;
            hx.logadd('cProgressBar');
            hx.log('cProgressBar', 'create');
            $.extend(this, aParam, {
                _contents: $('<p>', {
                    'text': aParam.text,
                }),
                _progress: $('<dl>', {
                    'class': 'progress type4'
                }),
                _progress_bar_1: $('<dd>', {
                    'class': 'graph'
                }),
                _progress_bar_2: $('<span>', {
                    'width': '0%'
                }),
                '_msg_type': aParam._msg_type,
                '_btn_title': aParam.btn_title,
            });
            var _attr = {
                'class': 'pro line1',
                'style': 'top:310px;'
            };
            if (this._msg_type === 'progressBarBtn' || this._msg_type === 'progressBtn') {
                $.extend(this, aParam, {
                    '_callback_fn': aParam.callback_fn
                });
                _attr = {
                    'class': 'pro vaBot'
                };
            }
            this._create({
                'tag': 'div',
                'attr': _attr
            });
            if (this._msg_type === 'progressBtn') {
                this.$.append(this._contents);
            }
            else {
                this.$.append(this._contents).append(this._progress.append(this._progress_bar_1.append(this._progress_bar_2)));
            }
            if (this._msg_type === 'progressBarBtn' || this._msg_type === 'progressBtn') {
                this.$.append($('<p>', {
                    'class': 'button',
                    //'text' : 'stop'
                    'text': this._btn_title
                }));
            }
            this.$.focus(function () {
                _this._doFocus(0);
            });
            return this;
        };
        progressBar.prototype.destroy = function () {
            this.$.remove();
        };
        // EVENT HANDLER
        progressBar.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var kc = aParam.alKey;
            switch (kc) {
                case hx.key.KEY_UP:
                case hx.key.KEY_DOWN:
                case hx.key.KEY_LEFT:
                case hx.key.KEY_RIGHT:
                    bConsumed = true;
                    break;
                case hx.key.KEY_OK:
                    if (this._msg_type == 'progressBarBtn' || this._msg_type == 'progressBtn') {
                        this.__send_event(aParam);
                    }
                    bConsumed = true;
                    break;
            }
            return bConsumed;
        };
        // PRIVATE FUNCTION FOR CONTENT MAKING
        progressBar.prototype._doFocus = function (aIndex) {
            var btn = this.$.children('.button');
            btn.eq(aIndex).focus();
        };
        progressBar.prototype.__send_event = function (aParam) {
            aParam = (aParam) || {};
            aParam.alEvent = 'alClicked';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData = (aParam.alData) || {};
            aParam.alData.me = this;
            if (this._msg_type == 'progressBarBtn' || this._msg_type == 'progressBtn') {
                aParam.alData.input_title = this._btn_title;
                aParam.alData.callback_fn = this._callback_fn;
            }
            this.sendEventToParent(aParam);
        };
        return progressBar;
    })(__COMPONENT_KERNEL__);
    exports.progressBar = progressBar;
});
