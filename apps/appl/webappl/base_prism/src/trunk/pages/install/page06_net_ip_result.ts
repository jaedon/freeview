/// <reference path="../../def/sugar.d.ts" />
// file name : pages/install/page06_net_result.js
/*jslint sloppy:true, nomen:true, vars:true */
/* DESIGN : design_result/02_WIZ/WIZ_302.html (success) */
/* DESIGN : design_result/02_WIZ/WIZ_303.html (fail) */
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx: any;

class cCPgInstall06 extends __PAGE_KERNEL__ {
    _containerEl : HTMLElement;
    _stepEl : HTMLElement;
    _btnWrapEl : HTMLElement;
    _pre_btn : any;
    _next_btn : any;
    _isGoWizardPage : boolean;
    _$article : any;
    _show : string;
    _$hArea : any;
    _$title : any;
    _timeset : any;

    _infoItems : any = [];

    _$menuWrap : any;
    _$statusWrap : any;
    _$box : any;
    _$txt : any;
    _systemInformation : any;
    _$srcStbEl : any;
    _$srcRouterEl : any;
    _$srcInternetEl : any;
    progressMsg : any;

    TNetworkConnectionStatus : any = {
        NETWORK_ALL_DISCONNECTED : [hx.l('LOC_MESG_5985_ID'), hx.l('LOC_MESG_5985_ID')],
        NETWORK_INTERNET_FAIL : [hx.l('LOC_OK_ID'), hx.l('LOC_MESG_5985_ID')],
        NETWORK_ALL_CONNECTED : [hx.l('LOC_OK_ID'), hx.l('LOC_OK_ID')]
    };

    TNeworkStatusType : any = {
        GATEWAY : 'gateway',
        DNS : 'dns'
    };

    _status : any = [hx.l('LOC_MESG_5985_ID'), hx.l('LOC_MESG_5985_ID')];
    _localAreaNet : string = hx.l('LOC_MESG_5985_ID');
    _internet : string = hx.l('LOC_MESG_5985_ID');

    constructor() {
        super();
    }
    create (aParam, aQuery) {
        hx.logadd('CPgInstall06');
        var that = this;
        var i : number, _result : string;
        var nowStep : string;
        var retAddr : any;

        for ( i = 0; i < hx.al.CPgInstall06.connectionMenu.length; i += 1) {
            that._infoItems[i] = hx.l(hx.al.CPgInstall06.connectionMenu[i]);
        }

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(false);
        $.extend(this, {
            _containerEl : document.querySelector('#wiz'),
            _$article : $('<article>', {
                'class' : 'cont'
            }),
            _$menuWrap : $('<article>', {
                'class' : 'wrap network_result'
            }),
            _systemInformation : hx.al.CPgInstall06.Menu(that._infoItems),
            _$statusWrap : $('<article>', {
                'class' : 'networkstatusInfo'
            }),
            _$box : $('<div>', {
                'class' : 'statusIcon'
            }),
            _$txt : $('<p>', {
                'class' : 'statusInfo'
            }),
            _$srcStbEl : $('<il>', {
                'html' : hx.l('LOC_STB_ID')
            }),
            _$srcRouterEl : $('<il>', {
                'html' : 'Router'
            }),
            _$srcInternetEl : $('<il>', {
                'html' : hx.l('LOC_NETWORK_CONNECTION_INTERNET_ID')
            }),
            _pre_btn : this.__make_btn('LOC_RETRY_ID'),
            _next_btn : null,
            _show : aQuery.data.show, //success, fail

            _$hArea : $('<h1>', {
                'class' : 'hArea'
            }),
            _$title : $('<span>', {
                'class' : 'tit',
                'html' : hx.l('LOC_NETWORK_CONNECTION_ID').toUpperCase()
            }),
            _stepEl : document.createElement('span'),
            _btnWrapEl : document.createElement('div')
        });

        //////////////////set Element attribute
        if (aQuery.query === 'prev') {
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
            nowStep = hx.al.CPgInstall06.items.step;
        }
        this._stepEl.classList.add(nowStep);

        this._btnWrapEl.classList.add('btnBo');
        this._btnWrapEl.classList.add('network_result');

        if (this._show === 'success') {
            this._next_btn = this.__make_btn('LOC_NEXT_ID');
        } else if (this._show === 'fail') {
            this._next_btn = this.__make_btn('LOC_SKIP_01_ID');
        }
        this._next_btn.$[0].style.whiteSpace = 'nowrap';

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

        that._$menuWrap.append(that._systemInformation.$).append(that._$statusWrap.append(that._$box).append(that._$txt.append(that._$srcStbEl).append(that._$srcRouterEl).append(that._$srcInternetEl)));
        //that._$menuWrap.append(this._btnWrapEl);

        this._$article.append(that._$menuWrap);

        this._$article[0].appendChild(this._btnWrapEl);
        this._containerEl.style.display = 'none';
        this._containerEl.appendChild(this._$hArea[0]);
        this._containerEl.appendChild(this._$article[0]);
        this._containerEl.style.display = 'block';

        /////////////Do Initial Script
        hx.al.compose(this).append(this._pre_btn).append(this._next_btn);
        if (this._show === 'success') {
            this._next_btn.on();
            this._next_btn.$.addClass('on');
        } else {
            this._pre_btn.on();
            this._pre_btn.$.addClass('on');
        }

        this._isGoWizardPage = false;
        hx.il.ani.action({
            targets : this._$hArea,
            cls : 'aniFadeIn'
        });
        hx.il.ani.action({
            targets : [this._$article],
            cls : 'aniFadeIn',
            style : 'left:0'
        });
        //update network status

        hx.svc.SETTING_UTIL.addEventCb('networkStatus', that, (aType : number, aResult : string) => {
            that._on_networkStatus(aType, aResult);
        });
        retAddr = hx.svc.SETTING_UTIL.getallAddress();
        that._drawAddress(retAddr);
        that._retry();
        hx.svc.SETTING_UTIL.checkGatewayStatus();
    }

    _retry (aParam?) {
        var that = this;
        this.progressMsg = hx.msg('progress', {
            text : hx.l('LOC_MESG_5986_ID'),
            _use_backkey : true,
            dia_class : 'osd',
            auto_close : true,
            close_time : 3000,
            callback_fn : function(_status) {
                if (_status === 'key_back' || _status === 'key_esc') {
                    that._$box.removeClass('netReady');
                    that._refreshNetworkStatus();
                } else if (_status === 'auto_close') {
                    that._$box.removeClass('netReady');
                    that._checkNetworkStatus(that._localAreaNet, that._internet);
                    that._refreshNetworkStatus();
                }
            }
        });

        this._systemInformation.getItem(0).html('');
        this._systemInformation.getItem(1).html('');

        switch (this._status) {
            case this.TNetworkConnectionStatus.NETWORK_ALL_DISCONNECTED :
                this._systemInformation.getItem(0).removeClass('discon');
                this._systemInformation.getItem(1).removeClass('discon');
                this._$box.removeClass('netAllFail');
                break;
            case this.TNetworkConnectionStatus.NETWORK_INTERNET_FAIL :
                this._systemInformation.getItem(0).removeClass('con');
                this._systemInformation.getItem(1).removeClass('discon');
                this._$box.removeClass('netIntFail');
                break;
            case this.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED :
                this._systemInformation.getItem(0).removeClass('con');
                this._systemInformation.getItem(1).removeClass('con');
                this._$box.removeClass('netAllConn');
                break;
        }
        this._$box.addClass('netReady');
    }

    _refreshNetworkStatus () {
        if(!this._systemInformation){
            //after destroyed
            return;
        }
        this._systemInformation.getItem(0).html(this._localAreaNet);
        this._systemInformation.getItem(1).html(this._internet);

        switch (this._status) {
            case this.TNetworkConnectionStatus.NETWORK_ALL_DISCONNECTED :
                this._systemInformation.getItem(0).addClass('discon');
                this._systemInformation.getItem(1).addClass('discon');
                this._$box.addClass('netAllFail');
                break;
            case this.TNetworkConnectionStatus.NETWORK_INTERNET_FAIL :
                this._systemInformation.getItem(0).addClass('con');
                this._systemInformation.getItem(1).addClass('discon');
                this._$box.addClass('netIntFail');
                break;
            case this.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED :
                this._systemInformation.getItem(0).addClass('con');
                this._systemInformation.getItem(1).addClass('con');
                this._$box.addClass('netAllConn');
                break;
        }
    }

    _drawAddress (aRetAddr) {
        var localAreaNet : string = '', internet : string = '', ipAddr : string = '', netmask : string = '', gateway : string = '', dns1st : string = '', dns2nd : string = '';

        if (aRetAddr) {
            ipAddr = aRetAddr.ipAddress || '';
            netmask = aRetAddr.netmask || '';
            gateway = aRetAddr.gateway || '';
            dns1st = aRetAddr.dns1st || '';
            dns2nd = aRetAddr.dns2nd || '';
        }

        localAreaNet = this._localAreaNet;
        internet = this._internet;

        this._checkNetworkStatus(localAreaNet, internet);

        this._systemInformation.getItem(0).html(localAreaNet);
        this._systemInformation.getItem(1).html(internet);
        this._systemInformation.getItem(2).html(ipAddr);
        this._systemInformation.getItem(3).html(netmask);
        this._systemInformation.getItem(4).html(gateway);
        this._systemInformation.getItem(5).html(dns1st);
        this._systemInformation.getItem(6).html(dns2nd);

        this._refreshNetworkStatus();
    }

    _checkNetworkStatus (aParamLocalAreaNet : string, aRaramInternet : string) {
        if ((aParamLocalAreaNet === hx.l('LOC_OK_ID')) && (aRaramInternet === hx.l('LOC_OK_ID')))
            this._status = this.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED;
        else if ((aParamLocalAreaNet === hx.l('LOC_OK_ID')) && (aRaramInternet === hx.l('LOC_MESG_5985_ID')))
            this._status = this.TNetworkConnectionStatus.NETWORK_INTERNET_FAIL;
        else if ((aParamLocalAreaNet === hx.l('LOC_MESG_5985_ID')) && (aRaramInternet === hx.l('LOC_MESG_5985_ID')))
            this._status = this.TNetworkConnectionStatus.NETWORK_ALL_DISCONNECTED;
    }

    _on_networkStatus (aType : number, aResult : string) {
        var that = this, localAreaNet, internet, connectionState;

        switch(aType) {
            case that.TNeworkStatusType.GATEWAY:
                if (aResult === 'success') {
                    that._localAreaNet = hx.l('LOC_OK_ID');
                    //that._localAreaNet = 'Connected';
                    hx.svc.SETTING_UTIL.checkDnsStatus();
                } else {
                    that._localAreaNet = hx.l('LOC_MESG_5985_ID');
                    that._internet = hx.l('LOC_MESG_5985_ID');
                    //that._localAreaNet = 'Disconnected';
                    //that._internet = 'Disconnected';
                }
                break;
            case that.TNeworkStatusType.DNS:
                if (aResult === 'success') {
                    that._localAreaNet = hx.l('LOC_OK_ID');
                    that._internet = hx.l('LOC_OK_ID');

                    //that._localAreaNet = 'Connected';
                    //that._internet = 'Connected';
                    localAreaNet = that._localAreaNet;
                    internet = that._internet;
                }
                break;
        }
        connectionState = hx.svc.SETTING_UTIL.getConnectionState();
        if (aResult === 'success') {
            if (connectionState === 'dhcp') {
                that._systemInformation.getDtItem(0).html(hx.l('LOC_WIRED_CONNECTION_ID'));
            } else {
                that._systemInformation.getDtItem(0).html(hx.l('LOC_WIRELESS_CONNECTION_ID'));
            }
        }
    }

    destroy () {
        clearInterval(this._timeset);
        hx.svc.SETTING_UTIL.clearEventCb(this);
        if (this._containerEl) {
            if (!this._isGoWizardPage) {
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
        this._show = undefined;
        this._$hArea = undefined;
        this._$title = undefined;
        this._stepEl = undefined;

        this._btnWrapEl = undefined;

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(true);
    }

    __make_btn (aId) : any {
        return hx.al.component('button', {
            attr : {
                'class' : 'button',
                'data-langID' : aId
            },
            usePointerKey : true
        });
    }
    _on_alScrollLeft (aParam) : boolean {
        var data = aParam.alData, target = data.me, bConsumed : boolean = false;

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
    _on_alScrollRight (aParam) : boolean {
        var data = aParam.alData, target = data.me, bConsumed : boolean = false;

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
    _on_alExit (aParam) : boolean {
        var data = aParam.alData, target = data.me, bConsumed : boolean = false;

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
    _on_alClicked (aParam) : boolean {
        var bConsumed : boolean = false;
        var _key = aParam.alKey;
        if (aParam.alData.me === this._pre_btn && _key === hx.key.KEY_OK) {
            bConsumed = true;
            this._go_pre_page();
        } else if (aParam.alData.me === this._next_btn && _key === hx.key.KEY_OK) {
            bConsumed = true;
            this._go_next_page();
        }
        return bConsumed;
    }
    _on_alKey (aParam) : boolean {
        var bConsumed : boolean = false;
        switch( aParam.alKey ) {
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
        }
        return bConsumed;
    }
    _go_pre_page () : void {
        this._isGoWizardPage = true;
        hx.al.decompose(this);
        this._fadeOutAni(hx.al.CPgInstall06.items.prevPage + '?prev', true);
    }
    _go_next_page () {
        this._isGoWizardPage = true;
        hx.al.decompose(this);
        if (this._show === 'success') {
            this._fadeOutAni(hx.al.CPgInstall06.items.nextPage + '?show=success', false);
        } else {
            this._fadeOutAni(hx.al.CPgInstall06.items.nextPage + '?show=fail', false);
        }
    }
    _fadeOutAni (aTargetPage, aIsGoPrev : boolean) : void {
        var left : string;

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
            cb : ()=> {
                this.sendEventToEntity({
                    alEvent : 'alChangeInstallPage',
                    target : aTargetPage,
                    me : this
                });
            }
        });
    }
}
hx.al.CPgInstall06 = cCPgInstall06;
hx.al.CPgInstall06.connectionMenu = ['LOC_WIRED_CONNECTION_ID', 'LOC_NETWORK_CONNECTION_INTERNET_ID', 'LOC_IPADDR_ID', 'LOC_NETMASK_ID', 'LOC_GATEADDR_ID', 'LOC_PRIDNS_ID', 'LOC_SECONDNS_ID'];
hx.al.CPgInstall06.Menu = function(aItems) {
    return hx.al.component('table', {
        attr : {
            'class' : 'info box'
        },
        langID : false,
        items : aItems,
        describe : aItems,
        count : hx.al.CPgInstall06.connectionMenu.length
    });
};

/********************      Static Variables     ********************/
if (hx.config.wzd.page06) {
    hx.al.CPgInstall06.items = hx.config.wzd.page06;
} else {
    hx.al.CPgInstall06.items = {
        itemStr : [''],
        nextPage : 'CPgInstall06_1',
        prevPage : 'CPgInstall04',
        step    : 'step2'
    };
}
export = cCPgInstall06;
