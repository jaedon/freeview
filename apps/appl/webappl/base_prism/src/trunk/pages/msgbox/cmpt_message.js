///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cMessage.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var cMessage = (function (_super) {
        __extends(cMessage, _super);
        function cMessage() {
            _super.call(this);
        }
        cMessage.prototype.init = function () {
            // TODO:
        };
        cMessage.prototype.create = function (aParam) {
            hx.logadd('cMessage');
            hx.log('cMessage', 'create');
            $.extend(this, aParam, {
                _contents: $('<p>', {
                    'html': aParam.text,
                })
            });
            var attr = {};
            attr['id'] = 'sta';
            attr['class'] = 'zSta';
            //TODO attr.class= 'sta'
            attr['style'] = 'top:310px';
            //TODO attr.style = 'top:310px';
            this._create({
                'tag': 'div',
                'attr': attr
            });
            this.$.append(this._contents);
            return this;
        };
        cMessage.prototype.destroy = function () {
            this.$.remove();
        };
        // EVENT HANDLER
        cMessage.prototype._on_alKey = function (aParam) {
            return false;
        };
        return cMessage;
    })(__COMPONENT_KERNEL__);
    exports.cMessage = cMessage;
});
