/// <reference path="../../def/sugar.d.ts" />
// file name : pages/install/page04_start_net.js
/*jslint sloppy:true, nomen:true, vars:true */
/* DESIGN : design_result/02_WIZ/WIZ_3.html */
/* DESIGN : design_result/02_WIZ/WIZ_301.html */
/* DESIGN : design_result/02_WIZ/WIZ_301A.html */
/* DESIGN : design_result/02_WIZ/WIZ_301B.html */
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx: any;

class cPgInstallStartNet extends __PAGE_KERNEL__{
    _msg: any;
    _connection_status: string; //wifi, DHCP_success, DHCP_fail
    _bIsGoWizardPage: boolean;

    _containerEl: HTMLElement;
    _$article: any;
    _pre_btn: any;
    _next_btn:any;
    _$hArea: any;
    _$title: HTMLElement;
    _stepEl: HTMLElement;
    _descriptEl: HTMLElement;
    _descriptEl2: HTMLElement;
    _incoEl: HTMLElement;
    _btnWrapEl: HTMLElement;

    constructor() {
        super();
    }

    create(aEntity, aPageUrl) {
        hx.logadd('CPgInstall04');
        var nowStep: string = '';

        this._msg = undefined;
        this._connection_status = 'wifi';

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(false);

        $.extend(this, {
            _containerEl : document.querySelector('#wiz'),
            _$article : $('<article class="cont">'),
            _pre_btn : this._make_btn('LOC_PREVIOUS_ID'),
            _next_btn : this._make_btn('LOC_NEXT_ID'),

            _$hArea : $('<h1>', {
                'class' : 'hArea'
            }),
            _$title : $('<span>', {
                'class' : 'tit',
                'html' : hx.l('LOC_NETWORK_CONNECTION_ID').toUpperCase(),
                'data-langID' : 'LOC_NETWORK_CONNECTION_ID'
            }),
            _stepEl : document.createElement('span'),
            _descriptEl : document.createElement('p'),
            _descriptEl2 : document.createElement('p'),
            _incoEl : document.createElement('article'),
            _btnWrapEl : document.createElement('div')
        });

        if (aPageUrl.query === 'prev') {
            this._$article.css('left', '-50px');
        } else {
            this._$article.css('left', '50px');
        }

        if (ENV.op === ENV.listOP.SES) {
            nowStep = 'step1';
        } else if (ENV.op === ENV.listOP.MIDDLE_EAST) {
            nowStep = 'step3';
        } else {
            //nowStep = 'step2';
            nowStep = hx.al.CPgInstall04.items.step;
        }

        this._stepEl.classList.add(nowStep);
        this._descriptEl.innerHTML = "Please make sure your Ethernet (LAN) cable is connected.";
        this._descriptEl2.innerHTML = "If you're using Wi-Fi, select NEXT and press OK to continue.";
        this._descriptEl.style.width = "1030px";
        this._descriptEl.style.margin = "0px 0px 0px 66px";
        this._descriptEl2.style.width = "1030px";
        this._descriptEl2.style.margin = "12px 0px 0px 66px";
        this._btnWrapEl.classList.add('btnBo');

        this._incoEl.innerHTML = '<div class="box lan" style="width:275px;">' + '   <p class="intxt"><em>' + 'ETHERNET CABLE (LAN)' + '</em> ' + '</p></div>' + '<div class="box wifi" style="width:275px;">' + '   <p class="intxt"><em>' + hx.l('LOC_BUILT_IN_WIFI_ID') + ' (WLAN)' + '</em></p></div>';
        this._incoEl.classList.add('inco');

        /////////////////append To DOM
        if (!this._containerEl) {
            this._containerEl = document.createElement('section');
            this._containerEl.id = 'wiz';
            document.querySelector('#wrap').appendChild(this._containerEl);
        }
        this._$hArea[0].appendChild(this._$title[0]);
        this._$hArea[0].appendChild(this._stepEl);
        this._btnWrapEl.appendChild(this._pre_btn.$[0]);
        this._btnWrapEl.appendChild(this._next_btn.$[0]);
        this._$article[0].appendChild(this._descriptEl);
        this._$article[0].appendChild(this._descriptEl2);
        this._$article[0].appendChild(this._incoEl);
        this._$article[0].appendChild(this._btnWrapEl);
        this._containerEl.style.display = 'none';
        this._containerEl.appendChild(this._$hArea[0]);
        this._containerEl.appendChild(this._$article[0]);
        this._containerEl.style.display = 'block';

        /////////////Do Initial Script
        hx.il.ani.action({
            targets : this._$hArea,
            cls : 'aniFadeIn'
        });
        hx.il.ani.action({
            targets : [this._$article],
            cls : 'aniFadeIn',
            style : 'left:0'
        });

        hx.al.compose(this).append(this._pre_btn).append(this._next_btn);
        this._next_btn.on();
        this._next_btn.$.addClass('on');
        this._bIsGoWizardPage = false;

        hx.svc.SETTING_UTIL.addEventCb('tryConnection', this, (aResult) => {
            this._on_tryConnection(aResult);
        });
    }
    destroy() {
        hx.svc.SETTING_UTIL.clearEventCb(this);
        if (this._containerEl) {
            if (!this._bIsGoWizardPage) {
                if (this._msg) {
                    hx.msg.close('progress', this._msg);
                }
                this._containerEl.parentElement.removeChild(this._containerEl);
            } else {
                this._containerEl.style.display = 'none';
                this._containerEl.removeChild(this._$hArea[0]);
                this._containerEl.removeChild(this._$article[0]);
                this._containerEl.style.display = 'block';
            }
            this._containerEl = undefined;
        }
        this._$article = undefined;
        this._pre_btn = undefined;
        this._next_btn = undefined;
        this._$hArea = undefined;
        this._$title = undefined;
        this._stepEl = undefined;
        this._descriptEl = undefined;
        this._descriptEl2 = undefined;
        this._incoEl = undefined;
        this._btnWrapEl = undefined;

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(true);
    }
    _on_alExit(aParam) {
        var data: any = aParam.alData, target: any = data.me,  bConsumed: boolean = false;

        switch (target) {
            case this._pre_btn:
            case this._next_btn:
                bConsumed = true;
                this._go_pre_page();
                break;
            default:
                break;
        }
        return bConsumed;
    }
    _on_alScrollLeft(aParam) {
        var data: any = aParam.alData, target: any = data.me, bConsumed: boolean = false;

        switch (target) {
            case this._pre_btn:
                bConsumed = true;
                this._pre_btn.$.removeClass('on');
                this._next_btn.on();
                this._next_btn.$.addClass('on');
                break;
            case this._next_btn:
                bConsumed = true;
                this._next_btn.$.removeClass('on');
                this._pre_btn.on();
                this._pre_btn.$.addClass('on');
                break;
        }
        return bConsumed;
    }
    _on_alScrollRight(aParam) {
        var data: any = aParam.alData, target: any = data.me, bConsumed: boolean = false;

        switch (target) {
            case this._pre_btn:
                bConsumed = true;
                this._pre_btn.$.removeClass('on');
                this._next_btn.on();
                this._next_btn.$.addClass('on');
                break;
            case this._next_btn:
                bConsumed = true;
                this._next_btn.$.removeClass('on');
                this._pre_btn.on();
                this._pre_btn.$.addClass('on');
                break;
        }
        return bConsumed;
    }
    _on_alClicked(aParam) {
        var bConsumed: boolean = false, _key: any = aParam.alKey;

        if (aParam.alData.me === this._pre_btn && _key === hx.key.KEY_OK) {
            bConsumed = true;
            this._go_pre_page();
        } else if (aParam.alData.me === this._next_btn && _key === hx.key.KEY_OK) {
            bConsumed = true;
            this._check_connect();
        }
        return bConsumed;
    }
    _on_alKey(aParam) {
        var bConsumed: boolean = false;

        switch( aParam.alKey ) {
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
        }
        return bConsumed;
    }
    _make_btn(aId) {
        return hx.al.component('button', {
            attr : {
                'class' : 'button',
                'data-langID' : aId
            },
            usePointerKey : true
        });
    }
    _on_tryConnection(aResult) {
        if (aResult === 'success') {//success
            this._connection_status = 'DHCP_success';
        } else if (aResult === 'fail') {//fail
            this._connection_status = 'DHCP_fail';
        }

        hx.msg.close('progress', this._msg);
    }
    _go_pre_page() {
        this._bIsGoWizardPage = true;
        hx.al.decompose(this);
        this._fadeOutAni(hx.al.CPgInstall04.items.prevPage + '?prev', true);
    }
    _check_connect() {
        //check connecting~!!!!!!
        var bResult: boolean = false;
        this._next_btn.$.attr('class', 'button on');

        bResult = hx.svc.SETTING_UTIL.checkWireConnection();

        if (bResult === true) {
            this._msg = hx.msg('progress', {
                text : hx.l('LOC_MESG_5986_ID'),
                auto_close : true,
                close_time : 30000,
                dia_class : 'osd',
                _use_backkey : true,
                callback_fn : (_status) => {
                    this._after_check_connect(_status);
                }
            });
            hx.svc.SETTING_UTIL.ConnectionTry();
            return;
        }
        this._connection_status = 'wifi';
        this._bIsGoWizardPage = true;
        hx.al.decompose(this);
        this._fadeOutAni('CPgInstall05', false);
    }
    _after_check_connect(aStatus) {
        //after check connect~!!!!!!!
        var szTarget: string = '';
        if (aStatus === 'auto_close' || aStatus === 'key_back' || aStatus === 'key_power' || aStatus === 'key_esc') {
            hx.svc.SETTING_UTIL.stopConnecting();
            szTarget = 'CPgInstall08_1';
            this._bIsGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(szTarget, false);
        }
        if (aStatus === 'close') {
            if (this._connection_status === 'DHCP_success') {
                szTarget = hx.al.CPgInstall04.items.nextPage + '?show=success';
            } else if (this._connection_status === 'DHCP_fail') {
                szTarget = 'CPgInstall08_1';
            } else {
                szTarget = 'CPgInstall08_1';
            }
            this._bIsGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(szTarget, false);
        }
    }
    _fadeOutAni(aTargetPage, aIsGoPrev) {
        var left: string = '';

        if (aIsGoPrev) {
            left = '20px';
        } else {
            left = '-20px';
        }

        hx.il.ani.action({
            targets : this._$article,
            cls : 'aniFadeOut',
            style : 'left:' + left
        });

        hx.il.ani.action({
            targets : this._$title,
            cls : 'aniFadeOut',
            cb : () => {
                this.sendEventToEntity({
                    alEvent : 'alChangeInstallPage',
                    target : aTargetPage,
                    me : this
                });
            }
        });
    }
}

hx.al.CPgInstall04 = cPgInstallStartNet;

/********************      Static Variables     ********************/
if (hx.config.wzd.page04) {
    hx.al.CPgInstall04.items = hx.config.wzd.page04;
} else {
    hx.al.CPgInstall04.items = {
        itemStr : ['LOC_CABLE_ID', ''],
        nextPage : 'CPgInstall06',
        prevPage : 'CPgInstall03',
        step : 'step3'
    };
}
export = cPgInstallStartNet;