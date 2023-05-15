///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cResponse.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var response = (function (_super) {
        __extends(response, _super);
        function response() {
            _super.call(this);
        }
        response.prototype.init = function () {
            // TODO:
        };
        response.prototype.create = function (aParam) {
            hx.logadd('cResponse');
            hx.log('cResponse', 'create');
            this._block_key = false;
            $.extend(this, aParam, {
                _contents: $('<p>', {
                    'html': aParam.text
                })
            });
            var _attr = {
                'class': 'res vaTop'
            };
            if (this._msg_type === 'progress') {
                this._block_key = !aParam.enable_key;
                _attr = {
                    'class': 'pro vaTop line1'
                };
            }
            else if (this._msg_type === 'recordStatus') {
                if (this.recIcon) {
                    _attr = {
                        'class': 'rps ' + this.recIcon + ' vaTop'
                    };
                }
                else {
                    _attr = {
                        'class': 'rps vaTop'
                    };
                }
            }
            this._create({
                'tag': 'div',
                'attr': _attr
            });
            this.$.append(this._contents);
            if (this._msg_type === 'recordStatus' && this.text_detail) {
                this.$.append($('<p>', {
                    'class': 'prd',
                    'html': this.text_detail
                }));
            }
            //if((param.auto_close == undefined ? param.auto_close = true : param.auto_close)){
            //	this._setClosedTimer(param);
            //}
            return this;
        };
        response.prototype.destroy = function () {
            this.$.remove();
        };
        // EVENT HANDLER
        response.prototype._on_alKey = function (aParam) {
            var _key = aParam.alKey;
            var bConsumed = false;
            if (this._block_key) {
                if (_key === hx.key.KEY_BACK && this._use_backkey) {
                    return false;
                }
                else {
                    return true;
                }
            }
            /*
             switch (_key) {
             case hx.key.KEY_UP:
             case hx.key.KEY_DOWN:
             case hx.key.KEY_LEFT:
             case hx.key.KEY_RIGHT:
             case hx.key.KEY_OK:
             bConsumed = true;
             break;
             }*/
            return bConsumed;
        };
        return response;
    })(__COMPONENT_KERNEL__);
    exports.response = response;
});
