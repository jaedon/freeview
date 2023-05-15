/**
 * Created by bglee@humaxdigital.com on 2014. 9. 1..
 */
///<reference path="../../def/sugar.d.ts"/>
///<reference path="../../def/prism.d.ts"/>

import __PAGE_KERNEL__ = require("pages/page_kernel");
import __PAGE_TV_GUIDE__ = require("prismux/prism.page.tvguide");

declare var hx: ifSugar;

var FRONT_PANEL_TITLE = "-TV GUIDE-";
class CPgGuide extends __PAGE_KERNEL__ {
    private _page: __PAGE_TV_GUIDE__ = null;
    constructor(aPageName) {
        super();
    }
    create() {
        hx.svc.AV_CTRL.addEventCb('ChannelChangeSucceeded', this, this.onChannelChangeSucceeded);
        hx.svc.AV_CTRL.addEventCb('ChannelChangeError', this, this.onChannelChangeError);
        hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, this.onChannelListUpdate());
        var page = new __PAGE_TV_GUIDE__($('#wrap'));

        this.setKeyHookingControl(page);
//        page.setUnInputTimeoutFunction(() => {
//            this._on_alKey(hx.key.KEY_ESCAPE);
//        }, 3 * 60 * 1000);
        page.onReqChannelList = () => {
            return hx.svc.CH_LIST.getChlist({}).chList || [];
        };
        this._page = page;
        hx.al.compose(this);
    }
    destroy() {
        this.setKeyHookingControl(this._page);
        hx.svc.SETTING_UTIL.setFrontPanelString({"text": ""});
        hx.al.decompose(this);
    }
    _on_alKey(aKeyCode) {
        this.sendEventToEntity({
            alEvent: 'alChangePage',
            target: 'CPgLiveController',
            me: this
        });
        this._page.destroy();
        return true;
    }
    onChannelChangeSucceeded(aParam) {
        hx.svc.SETTING_UTIL.setFrontPanelString({"text": FRONT_PANEL_TITLE});
    }
    onChannelChangeError(aParam) {
        hx.svc.SETTING_UTIL.setFrontPanelString({"text": FRONT_PANEL_TITLE});
    }
    onChannelListUpdate() {

    }
}
export = CPgGuide;
