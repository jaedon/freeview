/**
 * Created by bglee@humaxdigital.com on 2014. 9. 1..
 */
///<reference path="../../def/sugar.d.ts"/>
///<reference path="../../def/prism.d.ts"/>
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "prismux/prism.page.tvguide"], function (require, exports, __PAGE_KERNEL__, __PAGE_TV_GUIDE__) {
    var FRONT_PANEL_TITLE = "-TV GUIDE-";
    var CPgGuide = (function (_super) {
        __extends(CPgGuide, _super);
        function CPgGuide(aPageName) {
            _super.call(this);
            this._page = null;
        }
        CPgGuide.prototype.create = function () {
            hx.svc.AV_CTRL.addEventCb('ChannelChangeSucceeded', this, this.onChannelChangeSucceeded);
            hx.svc.AV_CTRL.addEventCb('ChannelChangeError', this, this.onChannelChangeError);
            hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, this.onChannelListUpdate());
            var page = new __PAGE_TV_GUIDE__($('#wrap'));
            this.setKeyHookingControl(page);
            //        page.setUnInputTimeoutFunction(() => {
            //            this._on_alKey(hx.key.KEY_ESCAPE);
            //        }, 3 * 60 * 1000);
            page.onReqChannelList = function () {
                return hx.svc.CH_LIST.getChlist({}).chList || [];
            };
            this._page = page;
            hx.al.compose(this);
        };
        CPgGuide.prototype.destroy = function () {
            this.setKeyHookingControl(this._page);
            hx.svc.SETTING_UTIL.setFrontPanelString({ "text": "" });
            hx.al.decompose(this);
        };
        CPgGuide.prototype._on_alKey = function (aKeyCode) {
            this.sendEventToEntity({
                alEvent: 'alChangePage',
                target: 'CPgLiveController',
                me: this
            });
            this._page.destroy();
            return true;
        };
        CPgGuide.prototype.onChannelChangeSucceeded = function (aParam) {
            hx.svc.SETTING_UTIL.setFrontPanelString({ "text": FRONT_PANEL_TITLE });
        };
        CPgGuide.prototype.onChannelChangeError = function (aParam) {
            hx.svc.SETTING_UTIL.setFrontPanelString({ "text": FRONT_PANEL_TITLE });
        };
        CPgGuide.prototype.onChannelListUpdate = function () {
        };
        return CPgGuide;
    })(__PAGE_KERNEL__);
    return CPgGuide;
});
