var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var cCPgWide = (function (_super) {
        __extends(cCPgWide, _super);
        function cCPgWide() {
            _super.call(this);
            this._curIndex = 0;
            this._bActivate = false;
            this._timerId = -1;
            this._aspectRatio = '16:9';
            this._format = 'auto';
        }
        cCPgWide.prototype.create = function () {
            var that = this;
            hx.al.CPgWide.items = {
                r169: [['wide r16 auto', hx.l('LOC_AUTO_ID')], ['wide r16 pill', hx.l('LOC_PILLARBOX_ID')], ['wide r16 zoom', hx.l('LOC_ZOOM_ID')]],
                r43: [['wide r04 auto', hx.l('LOC_AUTO_ID')], ['wide r04 lbox', hx.l('LOC_LETTERBOX_ID')], ['wide r04 cent', hx.l('LOC_CENTER_ID')]]
            };
            $.extend(that, {
                _$section: $('#ltv')
            });
            hx.al.compose(that);
        };
        cCPgWide.prototype.destroy = function () {
            this._hide();
        };
        cCPgWide.prototype.isActive = function () {
            return this._bActivate;
        };
        //visible () : boolean {
        //    return this._bActivate;
        //}
        cCPgWide.prototype._on_alKey = function (aParam) {
            var ulkeyCode = aParam.alKey;
            var bConsumed = true;
            switch (ulkeyCode) {
                case hx.key.KEY_POWER:
                    this._hide();
                    bConsumed = false;
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_ESCAPE:
                    this._hide();
                    break;
                case hx.key.KEY_SCREEN_MODE_NEXT:
                    this._curIndex += 1;
                    if (this._curIndex >= hx.al.CPgWide.itemMax) {
                        this._curIndex = 0;
                    }
                    this._apply();
                    this._update();
                    break;
                default:
                    break;
            }
            return bConsumed;
        };
        cCPgWide.prototype._on_alMessageToPage = function (param) {
            var i;
            if (param.query === 'show') {
                if (this._bActivate === false) {
                    var fl = hx.al.CPgWide.formatList;
                    var res = hx.svc.SETTING_UTIL.getPictureInfo();
                    this._aspectRatio = res.tvAspectRatio;
                    this._format = res.videoDisplayFormat;
                    this._curIndex = 0;
                    for (i = 0; i < hx.al.CPgWide.itemMax; i += 1) {
                        if (this._format === fl[i]) {
                            this._curIndex = i;
                            break;
                        }
                    }
                    this._show();
                }
            }
        };
        cCPgWide.prototype._apply = function () {
            var fl = hx.al.CPgWide.formatList;
            var format;
            format = fl[this._curIndex];
            // set format
            var param = {
                'tvAspectRatio': this._aspectRatio,
                'videoDisplayFormat': format
            };
            hx.svc.SETTING_UTIL.setPictureInfo(param);
        };
        cCPgWide.prototype._show = function () {
            var _this = this;
            var item;
            if (this._aspectRatio === '16:9') {
                item = hx.al.CPgWide.items.r169[this._curIndex];
            }
            else {
                item = hx.al.CPgWide.items.r43[this._curIndex];
            }
            // timer reset
            if (this._timerId !== -1) {
                clearTimeout(this._timerId);
                this._timerId = -1;
            }
            // set timer
            this._timerId = setTimeout(function () {
                _this._hide();
            }, 3000);
            // extend chChangeWrap
            $.extend(this, {
                $_image: $('<article>', {
                    'class': item[0]
                }),
                $_text: $('<p>', {
                    'text': item[1]
                })
            });
            // append elements
            this._$section.append(this.$_image.append(this.$_text));
            this._bActivate = true;
            this.sendEventToEntity({
                alEvent: 'alCheckPageVisible'
            });
        };
        cCPgWide.prototype._hide = function () {
            // kill timer
            if (this._timerId !== -1) {
                clearTimeout(this._timerId);
                this._timerId = -1;
            }
            // remove subWrap
            if (this._bActivate === true) {
                if (this.$_image !== undefined) {
                    this.$_image.remove();
                }
            }
            this._bActivate = false;
            this.sendEventToEntity({
                alEvent: 'alCheckPageVisible'
            });
        };
        cCPgWide.prototype._update = function () {
            this._hide();
            this._show();
        };
        return cCPgWide;
    })(__PAGE_KERNEL__);
    hx.al.CPgWide = cCPgWide;
    hx.al.CPgWide.itemMax = 3;
    hx.al.CPgWide.formatList = ['Auto', 'Sqeeze', 'CenterCutOut'];
    hx.al.CPgWide.items = {
        r169: [['wide r16 auto', hx.l('LOC_AUTO_ID')], ['wide r16 pill', hx.l('LOC_PILLARBOX_ID')], ['wide r16 zoom', hx.l('LOC_ZOOM_ID')]],
        r43: [['wide r04 auto', hx.l('LOC_AUTO_ID')], ['wide r04 lbox', hx.l('LOC_LETTERBOX_ID')], ['wide r04 cent', hx.l('LOC_CENTER_ID')]]
    };
    return cCPgWide;
});
