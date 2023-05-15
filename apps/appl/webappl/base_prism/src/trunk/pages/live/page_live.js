var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    ///<reference path="../../def/sugar.d.ts" />
    /** CPgLive */
    var cCPgLive = (function (_super) {
        __extends(cCPgLive, _super);
        function cCPgLive(aServiceName) {
            _super.call(this, aServiceName);
        }
        cCPgLive.prototype.create = function () {
            $.extend(this, {
                $wrap: $('#wrap'),
                $liveScreen: $('<div>', {
                    'id': 'dLive'
                }),
                $frame: $('<section>', {
                    'id': 'ltv'
                })
            });
            if (hx.emulMode) {
                this.$wrap.append(this.$liveScreen);
            }
            this.$wrap.append(this.$frame);
            hx.al.compose(this);
        };
        cCPgLive.prototype.destroy = function () {
        };
        return cCPgLive;
    })(__PAGE_KERNEL__);
    return cCPgLive;
});
