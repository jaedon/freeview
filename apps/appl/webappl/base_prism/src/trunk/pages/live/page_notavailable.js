var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var cCPgNotAvailable = (function (_super) {
        __extends(cCPgNotAvailable, _super);
        function cCPgNotAvailable() {
            _super.call(this);
            this._multiCustomMsgHandle = null;
            this._notAvailableIconHandle = null;
            this._timer = -1;
        }
        cCPgNotAvailable.prototype.create = function () {
            $.extend(this, {
                $na: $('<div class="notify" style="position:absolute;top:40px;right:60px;z-index:1000">' + '<p id="notAvable" class="ico"></p>' + '</div>')
            });
            hx.al.compose(this);
            hx.logadd('CPgNotAvailable');
        };
        cCPgNotAvailable.prototype.destroy = function () {
            this._notAvailableIconHide();
        };
        cCPgNotAvailable.prototype._on_alKey = function (aParam) {
            var k = hx.key;
            switch (aParam.alKey) {
                case k.KEY_UP:
                case k.KEY_DOWN:
                case k.KEY_LEFT:
                case k.KEY_RIGHT:
                case k.KEY_MENU:
                case k.KEY_SEARCH:
                case k.KEY_ESCAPE:
                case k.KEY_POWER:
                case k.KEY_NETFLIX:
                    break;
                default:
                    hx.log('CPgNotAvailable', 'not consummed key : ' + aParam.alKey);
                    this._notAvailableIconShow();
            }
            return false;
        };
        cCPgNotAvailable.prototype._notAvailableIconShow = function () {
            var _this = this;
            this._notAvailableIconHide();
            if (this._timer !== -1) {
                clearTimeout(this._timer);
                this._timer = -1;
            }
            $('#wrap').append(this.$na);
            this._timer = setTimeout(function () {
                _this._notAvailableIconHide();
            }, 1500);
        };
        cCPgNotAvailable.prototype._notAvailableIconHide = function () {
            this.$na.remove();
        };
        cCPgNotAvailable.prototype._on_alMessageToPage = function (param) {
            if (param.query === 'show') {
                this._notAvailableIconShow();
            }
            else if (param.query === 'hide') {
                this._notAvailableIconHide();
            }
        };
        return cCPgNotAvailable;
    })(__PAGE_KERNEL__);
    hx.al.CPgNotAvailable = cCPgNotAvailable;
    return cCPgNotAvailable;
});
