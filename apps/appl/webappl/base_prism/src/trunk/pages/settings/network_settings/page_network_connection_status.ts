///<reference path="../../../def/sugar.d.ts" />
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx;
/*file name : pages/settings/network_settings/network_connection_status.js */
/*jslint sloppy:true, nomen:true, vars:true */
class cCPgNetworkConnectionStatus extends __KERNEL_SETTINGS__{
    TNetworkConnectionStatus : any = {
        NETWORK_ALL_DISCONNECTED : [hx.l('LOC_MESG_5985_ID'), hx.l('LOC_MESG_5985_ID')],
        NETWORK_INTERNET_FAIL : [hx.l('LOC_OK_ID'), hx.l('LOC_MESG_5985_ID')],
        NETWORK_ALL_CONNECTED : [hx.l('LOC_OK_ID'), hx.l('LOC_OK_ID')]
    };

    TNeworkStatusType : any = {
        GATEWAY : 'gateway',
        DNS : 'dns'
    };
    aniConfig : any;
    progressMsg : any;

    _status : any = [hx.l('LOC_MESG_5985_ID'), hx.l('LOC_MESG_5985_ID')];
    _localAreaNet : string = hx.l('LOC_MESG_5985_ID');
    _internet : string = hx.l('LOC_MESG_5985_ID');
    _TITLE : string = '';
    _infoItems : any = [];
    _isChangePageInSetting : any;
    _systemInformation : any;
    _doneBtn : any;
    _$box : any;
    _$menuWrap : any;
    _$srcStbEl : any;
    _$srcRouterEl : any;
    _$srcInternetEl : any;
    _isOtaWorking : any;
    _$wrapBtn : any;
    _$statusWrap : any;
    _$txt : any;
    _checkNetwork : boolean = false;

    _progress_on : boolean = false;
    _progressMsg : any;
    _test1Msg : any;
    _test2Msg : any;
    _progress_timeout : number = 3500;

    constructor() {
        super();
    }

    create (aEntity, aPageUrl) {
        super.create();
        var that = this, i : number = 0;
        var retAddr : any;

        that._TITLE = hx.l('LOC_NETWORK_CONNECTION_STATUS_ID').toUpperCase();

        for ( i = 0; i < hx.al.CPgNetworkConnectionStatus.connectionMenu.length; i += 1) {
            that._infoItems[i] = hx.l(hx.al.CPgNetworkConnectionStatus.connectionMenu[i]);
        }

        hx.logadd('CPgNetworkConnectionStatus');
        that._init();

        $.extend(that, {
            _$menuWrap : $('<article>', {
                'class' : 'wrap'
            }),
            _systemInformation : hx.al.CPgNetworkConnectionStatus.Menu(that._infoItems),
            _$wrapBtn : $('<div>', {
                'class' : 'wrap_btn'
            }),
            _doneBtn : hx.al.CPgNetworkConnectionStatus.DoneButton('LOC_DONE_ID'),
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
        });
        var $title : any = $('#set .title');
        if ($title[0]) {
            $title[0].children[0].innerHTML = that._TITLE;
        }

        $('#set').append(that._$menuWrap.append(that._systemInformation.$).append(that._$statusWrap.append(that._$box).append(that._$txt.append(that._$srcStbEl).append(that._$srcRouterEl).append(that._$srcInternetEl))).append(that._$wrapBtn).append(that._doneBtn.$));

        hx.svc.SETTING_UTIL.addEventCb('networkStatus', that, (aType : number, aResult : string) => {
            if(that._checkNetwork === true){
                that._checkNetwork = false;
                that._on_networkStatus(aType, aResult);
            }
        });
        hx.svc.SETTING_UTIL.addEventCb('NetworkAvailableChanged', that, (networkAvailable : boolean) => {
            that._networkConnection();
        });
        retAddr = hx.svc.SETTING_UTIL.getallAddress();

        that._drawAddress(retAddr);

        hx.al.compose(that);

        $title.ani({
            style : that.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });
        that._$menuWrap.ani({
            style : that.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            },
            cb : function() {
                that.append(that._doneBtn);
                that._doneBtn.on();
                that._isChangePageInSetting = false;
            }
        });

        //that._retry();
        that._networkConnection();

        //hx.svc.SETTING_UTIL.checkGatewayStatus();
    }
    _retry (aParam?) {
        var that = this;
        that._progress_on = true;
        that._progressMsg = hx.msg('progress', {
            text : hx.l('LOC_MESG_5986_ID'),
            _use_backkey : true,
            dia_class : 'osd',
            auto_close : false,
            close_time : that._progress_timeout,
            callback_fn : function(_status) {
                hx.log("info","[_retry] _status :" + _status);
                if (_status === 'key_back' || _status === 'key_esc') {
                    that._$box.removeClass('netReady');
                    that._refreshNetworkStatus();
                } else if (_status === 'auto_close') {
                    that._$box.removeClass('netReady');
                    that._checkNetworkStatus(that._localAreaNet, that._internet);
                    that._refreshNetworkStatus();
                } else if (_status === 'close') {
                    that._$box.removeClass('netReady');
                    that._checkNetworkStatus(that._localAreaNet, that._internet);
                    that._refreshNetworkStatus();
                    that._drawResultMsg();
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

    _on_networkStatus (aType : number, aResult : string) {
        var that = this, localAreaNet, internet;
        var timeout = that._progress_timeout;
        var req_prss_close = false;
        var connectionState;
        hx.log('info', '[netSetting]_on_networkStatus aType ::' + aType +', aResult ::::' + aResult);
        hx.log('debug','page_network_connection_status _on_networkStatus aType : '+aType);
        switch(aType) {
            case that.TNeworkStatusType.GATEWAY:
                localAreaNet = that._localAreaNet;
                if (aResult === 'success') {
                    that._localAreaNet = hx.l('LOC_OK_ID');
                    //that._internet = hx.l('LOC_MESG_5985_ID');
                    that._checkNetwork = true;
                    hx.svc.SETTING_UTIL.checkDnsStatus();
                } else {
                    that._localAreaNet = hx.l('LOC_MESG_5985_ID');
                    that._internet = hx.l('LOC_MESG_5985_ID');
                    timeout = 1000;
                    req_prss_close = true;
                }
                break;
            case that.TNeworkStatusType.DNS:
                localAreaNet = that._localAreaNet;
                internet = that._internet;
                if (aResult === 'success') {
                    that._localAreaNet = hx.l('LOC_OK_ID');
                    that._internet = hx.l('LOC_OK_ID');
                    //https://digitaluk.atlassian.net/browse/FVCHUM-226
                    //After updating Network Environment, do re-auth to MDS
                    //var dsrvIpEpg = hx.svc.IP_EPG;
                    //if(dsrvIpEpg){
                    //    dsrvIpEpg.getAuthToken(0);
                    //}
                    // moved it to svc layer to handle all together properly
                }
                else{
                    that._internet = hx.l('LOC_MESG_5985_ID');
                    timeout = 1000;
                }
                req_prss_close = true;
                break;
        }
        if (req_prss_close) {
            setTimeout(function() {
                if ( that._progress_on && that._progressMsg && that._checkNetwork === false) {
                    that._progress_on = false;
                    hx.msg.close('progress', that._progressMsg);
                    that._progressMsg = null;
                }
            }, timeout);
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
    _drawAddress (aRetAddr) {
        var localAreaNet : string = '', internet : string = '', ipAddr : string = '', netmask : string = '', gateway : string = '', dns1st : string = '', dns2nd : string = '';
        if (aRetAddr) {
            ipAddr = aRetAddr.ipAddress || '';
            netmask = aRetAddr.netmask || '';
            gateway = aRetAddr.gateway || '';
            dns1st = aRetAddr.dns1st || '';
            dns2nd = aRetAddr.dns2nd || '';
        }
        hx.log("info","[_drawAddress] ::::::::::::: ipAddr " + ipAddr );
        hx.log("info","[_drawAddress] ::::::::::::: netmask " + netmask );
        hx.log("info","[_drawAddress] ::::::::::::: gateway " + gateway );
        hx.log("info","[_drawAddress] ::::::::::::: dns1st " + dns1st );
        hx.log("info","[_drawAddress] ::::::::::::: dns2nd " + dns2nd );

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
        hx.log("info","[_checkNetworkStatus] aParamLocalAreaNet " + aParamLocalAreaNet + ", aRaramInternet :" + aRaramInternet );
        if ((aParamLocalAreaNet === hx.l('LOC_OK_ID')) && (aRaramInternet === hx.l('LOC_OK_ID')))
            this._status = this.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED;
        else if ((aParamLocalAreaNet === hx.l('LOC_OK_ID')) && (aRaramInternet === hx.l('LOC_MESG_5985_ID')))
            this._status = this.TNetworkConnectionStatus.NETWORK_INTERNET_FAIL;
        else if ((aParamLocalAreaNet === hx.l('LOC_MESG_5985_ID')))
            this._status = this.TNetworkConnectionStatus.NETWORK_ALL_DISCONNECTED;
    }
    _refreshNetworkStatus () {
        if(!this._systemInformation){
            //after destroyed
            return;
        }
        hx.log("info","[_refreshNetworkStatus] this._status :" + this._status);
        this._systemInformation.getItem(0).removeClass('con');
        this._systemInformation.getItem(1).removeClass('con');
        this._systemInformation.getItem(0).removeClass('discon');
        this._systemInformation.getItem(1).removeClass('discon');
        this._$box.removeClass('netAllFail');
        this._$box.removeClass('netIntFail');
        this._$box.removeClass('netAllConn');

        this._systemInformation.getItem(0).html(this._localAreaNet);
        this._systemInformation.getItem(1).html(this._internet);

        switch (this._status) {
            case this.TNetworkConnectionStatus.NETWORK_ALL_DISCONNECTED :
                this._systemInformation.getItem(0).addClass('discon');
                this._systemInformation.getItem(1).addClass('discon');
                this._$box.addClass('netAllFail');
                this._doneBtn.$.addClass('long');
                this._doneBtn.$.html(hx.l('LOC_RETRY_YOUR_CONNECTION_ID'));
                break;
            case this.TNetworkConnectionStatus.NETWORK_INTERNET_FAIL :
                this._systemInformation.getItem(0).addClass('con');
                this._systemInformation.getItem(1).addClass('discon');
                this._$box.addClass('netIntFail');
                this._doneBtn.$.addClass('long');
                this._doneBtn.$.html(hx.l('LOC_RETRY_YOUR_CONNECTION_ID'));
                break;
            case this.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED :
                var retAddr:any;
                var ipAddr : string = '', netmask : string = '', gateway : string = '', dns1st : string = '', dns2nd : string = '';
                retAddr = hx.svc.SETTING_UTIL.getallAddress();
                if (retAddr) {
                    ipAddr = retAddr.ipAddress || '';
                    netmask = retAddr.netmask || '';
                    gateway = retAddr.gateway || '';
                    dns1st = retAddr.dns1st || '';
                    dns2nd = retAddr.dns2nd || '';
                }

                this._systemInformation.getItem(0).addClass('con');
                this._systemInformation.getItem(1).addClass('con');
                this._systemInformation.getItem(2).html(ipAddr);
                this._systemInformation.getItem(3).html(netmask);
                this._systemInformation.getItem(4).html(gateway);
                this._systemInformation.getItem(5).html(dns1st);
                this._systemInformation.getItem(6).html(dns2nd);

                this._$box.addClass('netAllConn');
                this._doneBtn.$.removeClass('long');
                this._doneBtn.$.html(hx.l('LOC_DONE_ID'));
                break;
        }
    }
    _drawResultMsg () {
        var that = this;
        var msgText = null;
        switch (that._status) {
            case that.TNetworkConnectionStatus.NETWORK_ALL_DISCONNECTED :
                msgText = hx.l('LOC_NETWORK_CON_FAILED_ID');
                break;
            case that.TNetworkConnectionStatus.NETWORK_INTERNET_FAIL :
                msgText = hx.l('LOC_LOCAL_AREA_NETWORK_BUT_INTERNET_FAILED_ID');
                break;
            case that.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED :
                msgText = hx.l('LOC_NETWORK_CON_SUCCESS_ID');
                break;
        }
        if (msgText) {
            setTimeout(function() {
                hx.msg('response', {
                    text : msgText,
                    dia_class : 'res vaTop',
                    auto_close : true,
                    close_time : 3000,
                });
            }, 500);
        }
    }
    _networkConnection () {
        var that = this, retVal = false, wirelessRetVal = false;
        var dsrvSettingUtil = hx.svc.SETTING_UTIL;
        if(!that._progress_on) {
            that._retry();
        }

        retVal = dsrvSettingUtil.checkWireConnection();
        if (!retVal) {
            wirelessRetVal = dsrvSettingUtil.checkWirelessConnection();
        }
        if (!retVal && !wirelessRetVal) { // Local Area Net Disconnected
            that._localAreaNet = hx.l('LOC_MESG_5985_ID');
            that._internet = hx.l('LOC_MESG_5985_ID');

            setTimeout(function() {
                if ( that._progress_on && that._progressMsg ) {
                    that._progress_on = false;
                    hx.msg.close('progress', that._progressMsg);
                    that._progressMsg = null;
                }
            }, that._progress_timeout);
        } else {
            that._checkNetwork = true;
            if(retVal){
                that._localAreaNet = hx.l('LOC_OK_ID');
                retVal = false;
                retVal = dsrvSettingUtil.checkInternetConnection();
                if (!retVal) {
                    that._internet = hx.l('LOC_MESG_5985_ID');
                }
            }else{
                dsrvSettingUtil.checkGatewayStatus();
            }
        }
    }
    _removeEventListeners () {
        hx.svc.SETTING_UTIL.setFrontPanelString({
            'text' : ''
        });
        hx.svc.SETTING_UTIL.clearEventCb(this);
    }
    destroy () {
        super.destroy();
        this._removeEventListeners();
        this._$menuWrap.remove();
        if (!this._isChangePageInSetting) {
            $('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }
        this._progress_on = false;
        if (this._progressMsg) {
            hx.msg.close('progress', this._progressMsg);
            this._progressMsg = null;
        }
        this._checkNetwork = false;
        this._$menuWrap = undefined;
        this._systemInformation = undefined;
        this._doneBtn = undefined;
    }
    _init () {
    }
    _on_alClicked (aParam) : boolean {
        var that = this, bConsumed : boolean = false;
        that._checkNetworkStatus(that._localAreaNet, that._internet);

        if (that._status === that.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED) {
            bConsumed = true;
            that._isChangePageInSetting = true;
            hx.al.decompose(that);
            that._$menuWrap.ani({
                style : that.aniConfig.menu.changeOut3,
                trans : {
                    dur : 200,
                    delay : 100
                },
                cb : function() {
                    that.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgSettings',
                        me : that
                    });
                }
            });
        } else {
            bConsumed = true;
            that._networkConnection();
        }
        return bConsumed;
    }
    _on_alExit (aParam) : boolean {
        var that = this, bConsumed : boolean = false;

        if (aParam.alData) {
            bConsumed = true;
            that._isChangePageInSetting = true;
            hx.al.decompose(that);
            that._$menuWrap.ani({
                style : that.aniConfig.menu.changeOut3,
                trans : {
                    dur : 200,
                    delay : 100
                },
                cb : function() {
                    that.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgSettings',
                        me : that
                    });
                }
            });
        }
        return bConsumed;
    }
    _on_alKey (aParam) : boolean {
        var that = this, bConsumed : boolean = false;

        switch (aParam.alKey) {
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
            case hx.key.KEY_POWER :
                var isOtaWorking = that._isOtaWorking ? that._isOtaWorking() : false
                if (isOtaWorking) {
                    bConsumed = true;
                }
                break;
            case hx.key.KEY_ESCAPE:
                if (aParam.alData) {
                    bConsumed = true;
                    hx.svc.SETTING_UTIL.setFrontPanelString({
                        text : ''
                    });
                    that.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgLiveController',
                        me : that
                    });
                }
                break;
        }
        return bConsumed;
    }
}
hx.al.CPgNetworkConnectionStatus =  cCPgNetworkConnectionStatus;


hx.al.CPgNetworkConnectionStatus.connectionMenu = ['LOC_WIRED_CONNECTION_ID', 'LOC_NETWORK_CONNECTION_INTERNET_ID', 'LOC_IPADDR_ID', 'LOC_NETMASK_ID', 'LOC_GATEADDR_ID', 'LOC_PRIDNS_ID', 'LOC_SECONDNS_ID'];

hx.al.CPgNetworkConnectionStatus.Menu = function(aItems) {
    return hx.al.component('table', {
        attr : {
            'class' : 'info box'
        },
        langID : false,
        items : aItems,
        describe : aItems,
        count : hx.al.CPgNetworkConnectionStatus.connectionMenu.length
    });
};

hx.al.CPgNetworkConnectionStatus.DoneButton = function(aLangID) {
    return hx.al.component('button', {
        attr : {
            'class' : 'button on',
            'data-langID' : aLangID
        },
        usePointerKey : true,
    });
};

hx.al.CPgNetworkConnectionStatus.prototype.aniConfig = {
    fadeIn : {
        start : {
            'opacity' : 0
        },
        end : {
            'opacity' : 1
        }
    },
    menu : {
        changeIn : {
            start : {
                'opacity' : 0,
                'left' : '20px'
            },
            end : {
                'opacity' : 1,
                'left' : 0
            }
        },
        changeOut3 : {
            start : {
                'opacity' : 1,
                'left' : 0
            },
            end : {
                'opacity' : 0,
                'left' : '20px'
            }
        }
    }
};
export =  cCPgNetworkConnectionStatus;