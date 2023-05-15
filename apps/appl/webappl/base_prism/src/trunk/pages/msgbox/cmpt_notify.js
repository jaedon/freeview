var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel", "pages/util/iUiComponent"], function (require, exports, __COMPONENT_KERNEL__, __UI_COMPONENT__) {
    ///<reference path="../../def/sugar.d.ts" />
    var Notify = (function (_super) {
        __extends(Notify, _super);
        function Notify() {
            _super.call(this);
            this.position = null;
            this.delay = 5000;
            this.auto_close = true;
            $.extend(this, __UI_COMPONENT__.uiComponent.visibility);
        }
        Notify.prototype.create = function (param) {
            this.position = {
                top: 0,
                left: 0
            };
            $.extend(this, param);
            //hx.logadd('cConfirm');
            hx.log('cNotify', 'create');
            if (param.timeout !== undefined) {
                this.delay = param.timeout;
            }
            if (param.auto_close !== undefined) {
                this.auto_close = param.auto_close;
            }
            this.$ = $('<div class="notify" style="position:absolute;top:40px;right:60px;z-index:1000">' + this.html + '</div>');
        };
        Notify.prototype.destroy = function () {
            this.hide();
            this.$.remove();
            this.$ = null;
        };
        Notify.prototype._onShow = function () {
            var that = this;
            this.$.css(this.position);
            if (this.auto_close === true) {
                this.timer = setTimeout(function () {
                    that.hide();
                }, this.delay);
            }
        };
        Notify.prototype._onHide = function () {
            this.$[0].innerHTML = '';
        };
        return Notify;
    })(__COMPONENT_KERNEL__);
    exports.Notify = Notify;
});
