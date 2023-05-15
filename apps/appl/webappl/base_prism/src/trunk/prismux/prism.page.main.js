var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", './prism.page'], function (require, exports, __PRISM_CPAGE__) {
    var mPrism;
    (function (mPrism) {
        var CMainPage = (function (_super) {
            __extends(CMainPage, _super);
            function CMainPage($el) {
                _super.call(this, $el);
            }
            CMainPage.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a.setId('main');
            };
            CMainPage.prototype.draw = function (param) {
                var _this = this;
                var a = this;
                if (!a.$layer()) {
                    a.createLayer({
                        fnAfterCreate: function () {
                            var childControl = [];
                            a.setChildControl(childControl);
                            _super.prototype.draw.call(a, {
                                setActiveFocus: true
                            });
                        }
                    });
                }
                else {
                    _super.prototype.draw.call(a, param);
                }
            };
            return CMainPage;
        })(__PRISM_CPAGE__.CPage);
        mPrism.CMainPage = CMainPage;
        if (!window.prism) {
            window.prism = {};
        }
        prism.createMainPage = function ($parent) {
            var page;
            var $page = $('<div>', {
                'class': 'prism-page'
            });
            $parent.append($page);
            return new CMainPage($page);
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
