var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var cCPgkey = (function (_super) {
        __extends(cCPgkey, _super);
        function cCPgkey() {
            _super.call(this);
        }
        cCPgkey.prototype.create = function () {
            hx.al.compose(this);
        };
        cCPgkey.prototype.destroy = function () {
        };
        cCPgkey.prototype.isActive = function () {
            return true;
        };
        cCPgkey.prototype._on_alKey = function (aParam) {
            var lkeyCode = aParam.alKey;
            var bkeyRepeat = aParam.alRepeat;
            var bConsumed = false;
            var i = 0;
            var item = [hx.key.KEY_LEFT, hx.key.KEY_RIGHT, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_CHANNEL_UP, hx.key.KEY_CHANNEL_DOWN];
            //        var item : any = hx.al.CPgkeyR.item;
            // allows some repeat keys
            if (bkeyRepeat === true) {
                bConsumed = true;
                for (i = 0; i < item.length; i += 1) {
                    if (item[i] === lkeyCode) {
                        bConsumed = false;
                        break;
                    }
                }
            }
            return bConsumed;
        };
        return cCPgkey;
    })(__PAGE_KERNEL__);
    hx.al.CPgkey = cCPgkey;
    return cCPgkey;
});
//hx.al.CPgkeyR.item = [hx.key.KEY_LEFT, hx.key.KEY_RIGHT, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_CHANNEL_UP, hx.key.KEY_CHANNEL_DOWN];
