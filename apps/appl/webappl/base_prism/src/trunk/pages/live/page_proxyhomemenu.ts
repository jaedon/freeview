///<reference path="../../def/sugar.d.ts" />
///<reference path="../../def/q.d.ts" />
// file name : pages/live/proxyHomemenu.js
/*jslint sloppy:true, nomen:true, vars:true */
import __PAGE_KERNEL__ = require("pages/page_kernel");
import __MODEL__ = require("model/model");
import __NETFLIX__ = require("layer_domain/service/dl_dsrvNetflix");
import __PROVIDER__ = require("pages/util/provider");
declare var hx;
declare var prism;

class cCPgHubProxy extends __PAGE_KERNEL__ {
    MAX_DATA_COUNT = 11;
    TIMEOUT_DATA_PROCESSING = 10000;
    //TIMEOUT_DATA_REFRESH = 1000 * 60 * 60 * 6; // 6 hours (4 times a day)
    $this : any;
    $wrap : JQuery;
    $wrap_smt : JQuery;
    _bActive = false;
    _bVisible : boolean = false;
    _imageDlg  : any;
    _appRpcUtil : any;
    _acceptPrivacyPolicy: boolean = false;
    _acceptTerms: boolean;
    responseMsg: any;
    _status: any = ['Disconnected', 'Disconnected'];
    _localAreaNet: any = 'Disconnected';
    _internet: any = 'Disconnected';
    _appList: any = [];
    _playerList: any = [];
    _contentList: any = [];
    _bAppListUpdate : boolean = false;
    TNetworkConnectionStatus = {
        NETWORK_ALL_DISCONNECTED : ['Disconnected', 'Disconnected'],
        NETWORK_INTERNET_FAIL : ['Connected', 'Disconnected'],
        NETWORK_ALL_CONNECTED : ['Connected', 'Connected'],
        NETWORK_CHECKING : ['Checking', 'Checking']
    };
    TNeworkStatusType = {
        GATEWAY : 'gateway',
        DNS : 'dns'
    };
    _bRespondedFVC: boolean = false;    // Freeview Players
    _bRespondedHMX: boolean = false;    // Humax Portal Apps
    _bRespondedTPK: boolean = false;    // Today's Picks
    _isProcessingFVC = null;    // check for processing Freeview Players
    _isProcessingHMX = null;    // check for processing Humax Portal Apps
    _isProcessingTPK = null;    // check for processing Today's Picks
    //_refreshTimerTPK = null;    // a timeout for refresh of Today's Picks

    TOrderPlayerType = ['/promoted_players', '/main_players', '/other_players', '/sub_players', '/etc_players'];
    // There are 5 types and 11 applications to use
    constructor() {
        super();
    }

    create () : void {
        hx.logadd('hubProxy');
        var that = this;
        that.$this = $(this);
        hx.al.compose(this);
        that._bActive = false;
        that.$wrap = $('#wrap');
        that.$wrap_smt = $('#wrap_smt');
        var dsrvAppMgr : any = hx.svc.APP_MGR;
        var dsrvTVPortal : any = hx.svc.TV_PORTAL;
        var dsrvChlist : any = hx.svc.CH_LIST;
        var dsrvAvCtrl : any = hx.svc.AV_CTRL;
        var dsrvSettingUtil : any = hx.svc.SETTING_UTIL;
        var dsrvWebStorage : any = hx.svc.WEB_STORAGE;
        var help2 : any = hx.config.help2.tvapps;

        this._acceptPrivacyPolicy = hx.svc.TV_PORTAL.isPrivacyPolicyAccepted();

        hx.svc.SETTING_UTIL.addEventCb('networkStatus', that, (aType : number, aResult : string) => {
            that._on_networkStatus(aType, aResult);
        });
        hx.svc.IP_EPG.addEventCb('getAuthToken', that, () => {
            that._getFVCAppList(true);
            that._getTodaysPicks(true);
        });

        var bmNetworkState : boolean;
        var mTvAppsHelpStatus : any;
        var mHideAppList = hx.svc.WEB_STORAGE.getHideAppList();

        var hubApi : any = {
            setData : function(param) {
                var self = this;
                self._call('setData', param);
            },
            setConfig : function(config, value) {
                var a = this;
                a._call('setConfig', config, value);
            },
            setLanguage : function(language) {
                var a = this;
                a._call('setLanguage', language);
            },
            setHomeData : function(dataList, type) {
                if (!dataList || !dataList.length) {
                    that._bRespondedFVC = false;
                    that._bRespondedHMX = false;
                    that._bRespondedTPK = false;
                    if (that._isProcessingHMX) {
                        clearTimeout(that._isProcessingHMX);
                        that._isProcessingHMX = null;
                    }
                    if (that._isProcessingFVC) {
                        clearTimeout(that._isProcessingFVC);
                        that._isProcessingFVC = null;
                    }
                    if (that._isProcessingTPK) {
                        clearTimeout(that._isProcessingTPK);
                        that._isProcessingTPK = null;
                    }
                    dataList = null;
                }
                var a = this;
                a._call('setHomeData', dataList, type);
            },
            setHideAppList : function(hideList) {
                var a = this;
                a._call('setHideAppList', hideList);
            },
            setLocalizationText : function(setTranslationText) {
                var self = this;
                self._call('setLocalizationText', setTranslationText);
            },
            notifyHomeMenuSelected : function(fn) {
                var self = this;
                self._notify('notifyHomeMenuSelected', fn);
            },
            notifyHomeItemSelected : function(fn) {
                var a = this;
                a._notify('notifyHomeItemSelected', fn)
            },
            notifyAppItemDeleted : function(fn) {
                var a = this;
                a._notify('notifyAppItemDeleted', fn);
            },
            notifyAppItemMoved : function(fn) {
                var a = this;
                a._notify('notifyAppItemMoved', fn);
            },
            notifyStatisticsKeyDown : function(fn) {
                var self = this;
                self._notify('notifyStatisticsKeyDown', fn);
            },
            notifyRequestUserAppList : function(fn) {
                var a = this;
                a._notify('notifyRequestUserAppList', fn);
            },
            notifyRequestStandby : function(fn) {
                var a = this;
                a._notify('notifyRequestStandby', fn);
            },
            notifyLaunchNetflix : function(fn) {
                var a = this;
                a._notify('notifyLaunchNetflix', fn);
            },
            notifyUnHandledKeyDown : function(fn) {
                var self = this;
                self._notify('notifyUnHandledKeyDown', fn);
            },
            setTermsAndConditions : function(isAccept) {
                var self = this;
                self._call('setTermsAndConditions', isAccept);
            }
        };
        var fnGetNetworkAvailable : Function = function() : boolean {
            return dsrvSettingUtil.getNetworkAvailable();
        };

        var fnIsDnsAvailable : Function = function() : boolean {
            var address = dsrvSettingUtil.getallAddress(), available = false;
            if (address) {
                if (address.dns1st && (address.dns1st !== '0.0.0.0')) {
                    available = true;
                }
                if (address.dns2nd && (address.dns2nd !== '0.0.0.0')) {
                    available = true;
                }
            }
            return available;
        };

        var fnGetCheckUpTvAppsHelp : Function = function() : any {
            return dsrvWebStorage.getHelp();
        };

        var fnSetCheckUpTvAppsHelp : Function = function(param : any) : any {
            return dsrvWebStorage.setHelp({
                tvapps : param
            });
        };

        var fnChangePage = (homeMenuItem : string, path?) : void => {
            var result : any, request : string, nochannel : boolean = false;
            var pageTable : any = {
                tvguide : 'CPgGuide',
                ondemand: 'CPgOnDemand',
                toppicks: 'CPgTopPicks',
                recordings : 'CPgRecordings',
                media : 'CPgMediaCenter',
                settings : 'CPgSettings'
            };
            if (pageTable[homeMenuItem] === pageTable.tvguide) {
                result = hx.svc.CH_LIST.getChannelCount();
                if (result.chCnt === 0) {// show no service*/
                    nochannel = true;
                    that.sendEventToEntity({
                        'alEvent' : 'alChangePage',
                        'target' : 'CPgLiveController',
                        'me' : that
                    });
                    that.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgLiveController?nochannel'
                    });
                }
            }
            if (!nochannel) {
                // USB URL Launcher : /url/url_hbbtv.txt
                if (pageTable[homeMenuItem] === pageTable.ondemand) {
                    var hbbUrl = dsrvSettingUtil.getHbbTvUrl();
                    hx.log('hubProxy', '############################## hbbUrl is : ' + hbbUrl);
                    if (hbbUrl) {
                        dsrvAppMgr.createPortalApp();
                        return;
                    }
                }
                dsrvAppMgr.destroyHbbTvDeeplinkApplication();
                request = 'CPgMain/hub=' + pageTable[homeMenuItem] + (path || '') + '?refresh=true';
                that.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : request,
                    me : that
                });
            }
            if (that.getPageVisible('CPgNotifier')) {
                that.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgNotifier?alChangePage'
                });
            }
        };
        dsrvAppMgr.createApplication({
            name : 'HUB',
            url : 'builtin_apps/home_menu/home_menu_new.html',
            builtIn : true,
            onLoaded: (appRpcUtil: ifCAppRpcUtil) => {
                if (hx.emulMode) {
                    that._localAreaNet = 'Connected';
                    that._internet = 'Connected';
                    that._checkNetworkStatus(that._localAreaNet,that._internet);
                } else {
                    that._networkConnection();
                }
                that._appRpcUtil = $.extend(appRpcUtil, hubApi);
                that._appRpcUtil.notifyHidden(function() {
                    hx.log("debug", "notifyHidden");
                    that.setHomeMenuOpen(false);
                    that._bActive = false;
                    that._bVisible = false;
                    that.sendEventToEntity({
                        alEvent : 'alCheckPageVisible'
                    });
                    that.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgMain/hub?hide',
                        me : that
                    });
                    that.$wrap.css('opacity', '1');
                    that.$wrap_smt.css('opacity', '1');
                    if (hx.svc.STATISTICS) {
                        hx.svc.STATISTICS.statisticsLog({
                            "CODE" : "OSDOUT",
                            "TRS" : "I",
                            "OSD" : "CPgHOME"
                        });
                    }
                    //that._refreshProcessingTimeouts();
                });
                that._appRpcUtil.notifyShown(function() {
                    hx.log("debug", "notifyShown");
                    that.setHomeMenuOpen(true);
                    that._bActive = true;
                    that._bVisible = true;
                    that.sendEventToEntity({
                        alEvent : 'alCheckPageVisible'
                    });
                    that.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgMain/hub?show',
                        me : that
                    });
                    that.$wrap.css('opacity', '0.3');
                    that.$wrap_smt.css('opacity', '0.3');
                    if (hx.svc.STATISTICS) {
                        hx.svc.STATISTICS.statisticsLog({
                            "CODE" : "OSDIN",
                            "TRS" : "I",
                            "OSD" : "CPgHOME"
                        });
                    }
                });
                that._appRpcUtil.notifyHomeMenuSelected((homeMenuItem, path?) => {
                    hx.log("debug", "notifyHomeMenuSelected");
                    dsrvAppMgr.destroyAllApps();
                    //dsrvAppMgr.destroyPortalApp();
                    dsrvTVPortal.destroyActiveApp();
                    //if (hx.svc.TSR.isTsrOngoing() || hx.svc.PLAYBACK.isMpbOnGoing()) {
                    //    hx.svc.BRIDGE.requestStopPlayback();
                    //} else {
                        if (dsrvAvCtrl.isIdle()) {
                            dsrvAppMgr.createHbbTVApplication(true);
                            dsrvAvCtrl.unBlockAV();
                            if (!dsrvAvCtrl.lastChannel()) {
                                dsrvAvCtrl.availableChannel();
                            }
                        }
                    //}
                    fnChangePage(homeMenuItem, path);
                    this.setHomeMenuOpen(false);
                });
                that._appRpcUtil.notifyHomeItemSelected(function(aItem, aType) {
                    that._acceptPrivacyPolicy = dsrvTVPortal.isPrivacyPolicyAccepted();
                    that._acceptTerms = dsrvTVPortal.isTermsAndConditionsAccepted();
                    var fnShowAcceptPopup = function() {
                        var buttonDlg = prism.createButtonDlg(null, {
                            textHtml: hx.l('LOC_HOME_ACCEPT_PP_TC_ID'),
                            buttonList : [tr('LOC_OK_ID')],
                            timeOutValue : 30000,
                        }, function(index) {
                            setTimeout(function(){
                                if (index == 0) {
                                    that.sendEventToEntity({
                                        alEvent : 'alChangePage',
                                        target : 'CPgSettings?set=privacypolicy',
                                        me : that
                                    });
                                }
                            },10);
                        });
                        buttonDlg.open();
                    }
                    var fnLaunchDeepLink = function(aName, aDeeplinkUrl) {
                        // this is for Freeview Players & Contents
                        hx.il.openDeepLink(aName, aDeeplinkUrl, undefined, undefined, undefined, that);
                    }
                    var fnLaunchApp = function(aName, aAppId) {
                        // this is for Portal apps
                        if(hx.config.useNetflixPW){
                            if (hx.svc.NETFLIX.getPwStatus()) {
                                that.sendEventToEntity({
                                    alEvent : 'alMessageToPage',
                                    target : 'CPgMain/CloseNetflixPW'
                                });
                            } else {
                                that.sendEventToEntity({
                                    alEvent : 'alMessageToPage',
                                    target : 'CPgLiveController?hidepin=lunchApp'
                                });
                            }
                        }
                        hx.il.launchApp(aName, aAppId, function() {
                            that.responseMsg = hx.msg('response', {
                                text : hx.l('LOC_CONNECTION_HAS_FAILED_ID'),
                                auto_close : true,
                                timeout : 3000,
                                callback_fn : function(status) {
                                    that.responseMsg = undefined;
                                }
                            });
                        }, that);
                    }
                    switch(aType) {
                        case __MODEL__.EHomeDataType.EApp:
                            if (aItem.PlayerType === '/promoted_players' || aItem.PlayerType === '/other_players') {
                                if (that._acceptTerms
                                ) {
                                    fnLaunchDeepLink(aItem.name, aItem.launchUrl);
                                } else {
                                    fnShowAcceptPopup();
                                }
                            } else {
                                if (that._acceptPrivacyPolicy) {
                                    fnLaunchApp(aItem.name, aItem.appid);
                                } else {
                                    fnShowAcceptPopup();
                                }
                            }
                            break;
                        case __MODEL__.EHomeDataType.EContent:
                            if (that._acceptTerms) {
                                fnLaunchDeepLink(aItem.name, aItem.deeplinkUrl);
                            } else {
                                fnShowAcceptPopup();
                            }
                            break;
                        case __MODEL__.EHomeDataType.EMoreContent:
                            fnChangePage('toppicks', '/ondemand');
                        default:
                            break;
                    }
                });
                that._appRpcUtil.notifyStatisticsKeyDown(function(keycode) {
                    hx.log("debug", "notifyStatisticsKeyDown");
                    if (hx.svc.STATISTICS) {
                        hx.svc.STATISTICS.statisticsLog({
                            "CODE" : "KI", //KEY_INPUT
                            "TRS" : "I",
                            "KEY" : keycode
                        });
                    }
                });
                that._appRpcUtil.notifyRequestUserAppList(function() {
                    var userApp = dsrvWebStorage.getUserAppList();
                    var hideApp = dsrvWebStorage.getHideAppList();
                    if (userApp) {
                        //mInstalledAppList = userApp;
                        mHideAppList = hideApp;
                        that._appRpcUtil.setHideAppList(hideApp);
                        //a._appRpcUtil.setHomeData(userApp);
                    }
                });
                that._appRpcUtil.notifyRequestStandby(function() {
                    that.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgStandby',
                        me : that
                    });
                });
                that._appRpcUtil.notifyLaunchNetflix(function() {
                    var srvNetflix = hx.svc.NETFLIX;
                    if(hx.config.useNetflixPW && hx.msg.isOpendWithName('pinPC') && !srvNetflix.getPwStatus()){
                        that.sendEventToEntity({
                            alEvent : 'alMessageToPage',
                            target : 'CPgLiveController?hidepin=lunchApp'
                        });
                    }
                    hx.il.launchApp("Netflix", undefined, undefined, that, __NETFLIX__.START_TYPE.NETFLIX_BUTTON);
                });
                that._appRpcUtil.notifyUnHandledKeyDown(function(keyCode) {
                    hx.log("debug", "notifyUnHandledKeyDown");
                    //XXX
                    switch (keyCode) {
                        case hx.key.KEY_ESCAPE:
                        case hx.key.KEY_POWER:
                        case hx.key.KEY_BACK:
                            that._hideHub();
                            break;
                    }
                });
                that._updateLanguage();

                if (dsrvTVPortal &&  (that._status === that.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED) ) {
                    that._requestHomeDataList();
                } else {
                    that._appRpcUtil.setHomeData(undefined);
                }
            }
        });
        dsrvSettingUtil.addEventCb('NetworkAvailableChanged', that, function(networkAvailable) {
            hx.log('hubProxy', 'NetworkAvailableChanged ::' + networkAvailable +', _bVisible ::' + that._bVisible);
            if(networkAvailable) {
                that._networkConnection();
            }else{
                hx.log('error', 'Not Available Network!!!');
                that._localAreaNet = 'Disconnected';
                that._internet = 'Disconnected';
                that._checkNetworkStatus(that._localAreaNet, that._internet);
                that._appList = [];
                that._playerList = [];
                that._contentList = [];
                that._appRpcUtil.setHomeData(undefined);
            }
        });
        dsrvSettingUtil.addEventCb('wirelessStopConnect', that, function() {
            hx.log('hubProxy', 'wirelessStopConnect _bVisible ::' + that._bVisible);
            if(dsrvSettingUtil.checkWireConnection() === false && dsrvSettingUtil.checkWirelessConnection() === true){
                that._localAreaNet = 'Disconnected';
                that._internet = 'Disconnected';
                that._checkNetworkStatus(that._localAreaNet,that._internet);
                that._appList = [];
                that._playerList = [];
                that._contentList = [];
                if(that._bVisible === true){
                    that._appRpcUtil.setHomeData(that._appList, __MODEL__.EHomeDataType.EApp);
                }
            }
        });
        dsrvTVPortal.addEventCb('privacyPolicyChange', that, function (aAccept) {
            if (!aAccept) {
                hx.log('hubProxy', '[privacyPolicyChange] disagree -> clear humax app list.');
                that._bRespondedHMX = false;
                that._appList = [];
                that._rearrangeAppList(that._playerList);
                that._appRpcUtil.setHomeData(that._appList, __MODEL__.EHomeDataType.EApp);
            } else {
                hx.log('hubProxy', '[privacyPolicyChange] agree -> update humax app list.');
                that._getTvPortalAppList();
            }
        });
    }

    destroy () : void {
        if (this._imageDlg) {
            this._imageDlg.destroy();
            delete this._imageDlg;
        }
    }
    isActive () : boolean {
        return this._bActive;
    }

    //visible () : boolean {
    //    return this._bVisible;
    //}
    _requestHomeDataList() : void {
        var that = this;
        that._getFVCAppList();
        that._getTvPortalAppList();
        that._getTodaysPicks(true);
    }
    _refreshProcessingTimeouts() {
        var that = this;
        //if (that._bRespondedFVC) {
        //    hx.log('hubProxy', '[_refreshProcessingTimeouts] cannot request FVC for 10 secs! cos already completed.');
        //    if (that._isProcessingFVC) {
        //        clearTimeout(that._isProcessingFVC);
        //        that._isProcessingFVC = null;
        //    }
        //    that._isProcessingFVC = setTimeout(() => {
        //        if (that._isProcessingFVC) {
        //            clearTimeout(that._isProcessingFVC);
        //            that._isProcessingFVC = null;
        //        }
        //    }, that.TIMEOUT_DATA_PROCESSING);
        //}
        //if (that._bRespondedHMX) {
        //    hx.log('hubProxy', '[_refreshProcessingTimeouts] cannot request HMX for 10 secs! cos already completed.');
        //    if (that._isProcessingHMX) {
        //        clearTimeout(that._isProcessingHMX);
        //        that._isProcessingHMX = null;
        //    }
        //    that._isProcessingHMX = setTimeout(() => {
        //        if (that._isProcessingHMX) {
        //            clearTimeout(that._isProcessingHMX);
        //            that._isProcessingHMX = null;
        //        }
        //    }, that.TIMEOUT_DATA_PROCESSING);
        //}
        if (that._bRespondedTPK) {
            hx.log('hubProxy', '[_refreshProcessingTimeouts] cannot request TPK for 10 secs! cos already completed.');
            if (that._isProcessingTPK) {
                clearTimeout(that._isProcessingTPK);
                that._isProcessingTPK = null;
            }
            that._isProcessingTPK = setTimeout(() => {
                if (that._isProcessingTPK) {
                    clearTimeout(that._isProcessingTPK);
                    that._isProcessingTPK = null;
                }
            }, that.TIMEOUT_DATA_PROCESSING);
        }
    }
    _getFVCAppList(bForced?) {
        var that = this;
        if (!hx.svc.IP_EPG.isAvailableToUseAuth()) {
            hx.log('debug', '[_getFVCAppList] can not get this!(Auth is not available yet)');
            return;
        }
        if (that._isProcessingFVC) {
            hx.log('debug', '[_getFVCAppList] do not get again!(is processing to get FVC data)');
            return;
        }
        if (!bForced && that._bRespondedFVC) {
            hx.log('debug', '[_getFVCAppList] do not get again!(fvc players are completed)');
            return;
        }
        that._isProcessingFVC = setTimeout(() => {
            if (that._isProcessingFVC) {
                clearTimeout(that._isProcessingFVC);
                that._isProcessingFVC = null;
            }
        }, that.TIMEOUT_DATA_PROCESSING);
        hx.log('hubProxy', '[_getFVCAppList] request FVC Players!!!!!');
        hx.svc.IP_EPG.getApplication(__MODEL__.EImageVariant.EColour_1_1).then((info) => {
            var reqCount = 0, resCount = 0;
            var bAllReq = false;
            var queryList = [];
            if (info) {
                var fvcAppList = [];
                var parsed = hx.svc.IP_EPG.parse.xml.application(info);
                var fvcPlayers;
                var promotedPlayersCnt = 0;
                if (parsed && parsed['/promoted_players']) {
                    promotedPlayersCnt = parsed['/promoted_players'].length;
                }
                for (var key in parsed) {
                    if (that.TOrderPlayerType.indexOf(key) > -1) { // to skip invalid_players for robustness spec
                        fvcPlayers = parsed[key];
                        for (var i = 0; i < fvcPlayers.length; i++) {
                            reqCount++;
                            (function(index, memberOfType, title, contentUrl) {
                                hx.svc.IP_EPG.getThumbnail(fvcPlayers[i].image, 100).done((thumbnailUrl) => {
                                    resCount++;
                                    addAITQueryToQueryList({
                                        name: title,
                                        focusable: true,
                                        icons: [{
                                            ImageType: "HMS_FOCUS",
                                            Size: "100*56",
                                            Url: thumbnailUrl
                                        }],
                                        launchUrl: (function(aUrl) {
                                            var params = hx.svc.SETTING_UTIL.getNetworkId().map(function(nid) {
                                                return 'nids[]=' + nid;
                                            });
                                            params.push('lloc=homepage');
                                            return [aUrl, params.join('&')].join(aUrl.indexOf('?') !== -1 ? '&' : '?');
                                        })(contentUrl),
                                        PlayerType: memberOfType,
                                        appid: index
                                    });
                                    if (bAllReq === true && reqCount === resCount) {
                                        hx.log('hubProxy', '[hubProxy] queryList.length : ' + queryList.length);
                                        hx.svc.IP_EPG.getAITList(queryList).then(function () {
                                            hx.log('hubProxy', '[hubProxy] Success? fvcAppList.length : ' + fvcAppList.length);
                                            processRetObj();
                                        }, () => {
                                            hx.log('hubProxy', '[hubProxy] Fail? fvcAppList.length : ' + fvcAppList.length);
                                            processRetObj();
                                        });
                                    }
                                });
                            })(i, key, fvcPlayers[i].title, fvcPlayers[i].ait);
                        }
                    }
                }
            }
            var fnSortByOrder : Function = (list) => {
                list.sort((a, b) => {
                    var ret = 0;
                    a = a instanceof Array ? a[0] : a;
                    b = b instanceof Array ? b[0] : b;
                    ret = this.TOrderPlayerType.indexOf(a.PlayerType) - this.TOrderPlayerType.indexOf(b.PlayerType);
                    if (ret === 0 && typeof a.appid === "number" && typeof b.appid === "number") {
                        return a.appid - b.appid;
                    }
                    return ret;
                });
            };
            function processRetObj() {
                fnSortByOrder(fvcAppList);
                if (JSON.stringify(that._playerList) === JSON.stringify(fvcAppList)) {
                    hx.log('debug', '[_getFVCAppList] do not have to redraw!! it is same with previous one')
                    return;
                }
                that._playerList = fvcAppList;
                that._rearrangeAppList(fvcAppList);
                that._appRpcUtil.setHomeData(that._appList, __MODEL__.EHomeDataType.EApp);
                that._bRespondedFVC = true;
            }
            function checkTemplateAIT(xml) {
                if (!xml) {
                    hx.log('debug', '[hubProxy] FAIL!!! checkTemplateAIT : argu(xml) is null');
                    return false;
                }
                //check hbbtv application
                var type = xml.querySelector('OtherApp').textContent;
                //check version
                var major = xml.querySelector('versionMajor').textContent;
                var minor = xml.querySelector('versionMinor').textContent;
                var micro = xml.querySelector('versionMicro').textContent;
                var version = (major + minor + micro) * 1;
                if (version <= 131 && type === 'application/vnd.hbbtv.xhtml+xml') { // B2C Spec (v1.0) 7.1 Device Criteria available version 1.3.1
                    return true;
                }
                hx.log('debug', '[hubProxy] FAIL!!! checkTemplateAIT : [' + version + '][' + type + '] / ' + this.name);
                return false;
            }
            function addAITQueryToQueryList(aPlayer) {
                var subRetDef = Q.defer();
                subRetDef.promise.then((aXml) => {
                    if (checkTemplateAIT(aXml)) {
                        fvcAppList.push(aPlayer);
                    }
                });
                queryList.push({
                    url: aPlayer.launchUrl,
                    retDef: subRetDef
                });
            }
            bAllReq = true;
        }, () => {
            // nothing to do
        });
    }
    _getTvPortalAppList(bForced?) {
        var a = this;
        if (a._isProcessingHMX) {
            hx.log('debug', '[_getTvPortalAppList] do not get again!(is processing to get HMX data)');
            return;
        }
        if (!bForced && a._bRespondedHMX) {
            hx.log('debug', '[_getTvPortalAppList] do not get again!(portal apps are completed)');
            return;
        }
        a._isProcessingHMX = setTimeout(() => {
            if (a._isProcessingHMX) {
                clearTimeout(a._isProcessingHMX);
                a._isProcessingHMX = null;
            }
        }, a.TIMEOUT_DATA_PROCESSING);
        hx.log('hubProxy', '[_getTvPortalAppList] request Humax Apps!!!!!');
        if (hx.emulMode) {
            hx.svc.TV_PORTAL.getSmartSearchPlayers({
                fn: function (smartSearchAppList) {
                    a._rearrangeAppList(smartSearchAppList);
                    hx.svc.TV_PORTAL.getDefaultAppList({
                        fn: function (defaultAppList) {
                            a._rearrangeAppList(defaultAppList);
                            a._appRpcUtil.setHomeData(a._appList, __MODEL__.EHomeDataType.EApp);
                            a._bRespondedHMX = true;
                        },
                        fnError: function () {
                            a._appRpcUtil.setHomeData(a._appList, __MODEL__.EHomeDataType.EApp);
                        }
                    });
                },
                fnError: function() {
                    a._appRpcUtil.setHomeData(a._appList, __MODEL__.EHomeDataType.EApp);
                }
            });
            return;
        }
        if (hx.svc.TV_PORTAL.isLogged()) {
            hx.svc.TV_PORTAL.getSmartSearchPlayers({
                fn: function(smartSearchAppList) {
                    a._rearrangeAppList(smartSearchAppList);
                    a._appRpcUtil.setHomeData(a._appList, __MODEL__.EHomeDataType.EApp);
                    a._bRespondedHMX = true;
                },
                fnError: function() {
                    a._appRpcUtil.setHomeData(a._appList, __MODEL__.EHomeDataType.EApp);
                }
            });
        } else {
            hx.svc.TV_PORTAL.login({
                fn: function (logged) {
                    if (logged) {
                        hx.svc.TV_PORTAL.getSmartSearchPlayers({
                            fn: function (smartSearchAppList) {
                                a._rearrangeAppList(smartSearchAppList);
                                a._appRpcUtil.setHomeData(a._appList, __MODEL__.EHomeDataType.EApp);
                                a._bRespondedHMX = true;
                            },
                            fnError: function() {
                                a._appRpcUtil.setHomeData(a._appList, __MODEL__.EHomeDataType.EApp);
                            }
                        });
                    } else {
                        a._appRpcUtil.setHomeData(a._appList, __MODEL__.EHomeDataType.EApp);
                    }
                },
                fnError: function () {
                    a._appRpcUtil.setHomeData(a._appList, __MODEL__.EHomeDataType.EApp);
                }
            });
        }
    }
    _rearrangeAppList(aList) {
        // Netflix&Youtube should be placed at the end of FVC core player list.
        // (BBC News, BBC Sports are not FVC core players)
        var homeAppList = [], i = 0, length = aList ? aList.length : 0;
        for (i=0; i<length; i+=1) {
            if (!aList[i].PlayerType) {
                aList[i].PlayerType = (aList[i].ModelAppTypeName === "STBLAUNCHER") ?
                    ((aList[i].name === "Netflix" || aList[i].name === "YouTube") ? '/main_players' : '/sub_players') : '/etc_players';
            }
            homeAppList.push(aList[i]);
        }
        var fnMergeLists : Function = (oldList, newList) => {
            var bSame = false;
            $.each(newList, (idx1, newElement) => {
                bSame = false;
                $.each(oldList, (idx2, oldElement) => {
                    if ((newElement.PlayerType === oldElement.PlayerType) && (newElement.appid === oldElement.appid)) {
                        bSame = true;
                        return false;
                    }
                });
                if (!bSame) {
                    oldList.push(newElement);
                }
            });
        };
        var fnSortByOrder : Function = (list) => {
            list.sort((a, b) => {
                var ret = 0;
                a = a instanceof Array ? a[0] : a;
                b = b instanceof Array ? b[0] : b;
                ret = this.TOrderPlayerType.indexOf(a.PlayerType) - this.TOrderPlayerType.indexOf(b.PlayerType);
                if (ret === 0 && typeof a.appid === "number" && typeof b.appid === "number") {
                    return a.appid - b.appid;
                }
                return ret;
            });
        };
        fnMergeLists(this._appList, homeAppList);
        fnSortByOrder(this._appList);
    }
    _getTodaysPicks(bForced?) {
        var that = this;
        if (hx.emulMode) {
            // to debug with today's picks(Recommended)
            //that._appRpcUtil.setHomeData(that._contentList, __MODEL__.EHomeDataType.EContent);
            //that._bRespondedTPK = true;
            //return;
        }
        if (!hx.svc.IP_EPG.isAvailableToUseAuth()) {
            hx.log('debug', '[_getTodaysPicks] can not get this!(Auth is not available yet)');
            return;
        }
        if (that._isProcessingTPK) {
            hx.log('debug', '[_getTodaysPicks] do not get again! timeout 10 secs again! (is processing to get TPK data)');
            that._refreshProcessingTimeouts();
            return;
        }
        if (!bForced && that._bRespondedTPK) {
            hx.log('debug', '[_getTodaysPicks] do not get again!(recommendation contents are completed)');
            return;
        }
        that._isProcessingTPK = setTimeout(() => {
            if (that._isProcessingTPK) {
                clearTimeout(that._isProcessingTPK);
                that._isProcessingTPK = null;
            }
        }, that.TIMEOUT_DATA_PROCESSING);
        //if (that._refreshTimerTPK) {
        //    clearTimeout(that._refreshTimerTPK);
        //    that._refreshTimerTPK = null;
        //}
        //that._refreshTimerTPK = setTimeout(() => {
        //    hx.log('debug', '[_getTodaysPicks] timeout!! refresh Trending now!!!!!!');
        //    if (that._isProcessingTPK) {
        //        clearTimeout(that._isProcessingTPK);
        //        that._isProcessingTPK = null;
        //    }
        //    that._getTodaysPicks(true);
        //}, that.TIMEOUT_DATA_REFRESH);
        hx.log('hubProxy', '[_getTodaysPicks] request Trending now!!!!!');
        hx.svc.IP_EPG.getRecommendationCategories().then((categoryInfo) => {
            if (!categoryInfo) {
                return;
            }
            // this is an ordered list
            var categories = hx.svc.IP_EPG.parse.xml.category(categoryInfo);
            if (categories && categories.length) {
                // ALL category (or the first category)
                hx.svc.IP_EPG.getRecommendationResults({
                    'groupId' : categories[0].groupId,
                    //'serviceId' : '4164'    // for test 1.BBC One London
                }).then((recommendationInfo) => {
                    if (!recommendationInfo) {
                        return;
                    }
                    var recommendations = hx.svc.IP_EPG.parse.xml.recommendation(recommendationInfo);
                    // this is an ordered list
                    var crids = recommendations.crids;
                    //var numberOfItems = recommendations.numberOfItems;
                    var odProgs = [], i, odItem = null;
                    var queryList = [];
                    function addAITQueryToQueryList(aOndemand) {
                        var subRetDef = Q.defer();
                        subRetDef.promise.then((aXml) => {
                            if (aOndemand.checkTemplateAIT(aXml)) {
                                odProgs.push(aOndemand)
                            }
                        });
                        var query = {
                            url: aOndemand.getTemplateAitUrl(),
                            retDef: subRetDef
                        };
                        queryList.push(query);
                    }
                    if (crids) {
                        for (i in crids) {
                            odItem = new __PROVIDER__.COnDemandProvider(crids[i], 'homepage');
                            // delete raw data for sending message(json) between windows
                            delete odItem.mdsRaw;
                            if (odItem.isPlayAvailable()) {
                                addAITQueryToQueryList(odItem);
                            }
                        }
                        hx.log('hubProxy', '[MDS] Recommendations! queryList.length : ' + queryList.length);
                    }
                    var fnSort = function(array) {
                        array.sort((a, b) => {
                            a = a instanceof Array ? a[0] : a;
                            b = b instanceof Array ? b[0] : b;
                            return a.index - b.index;
                        });
                    };
                    var processRetObj = function() {
                        if (queryList.length - odProgs.length) {
                            hx.log('hubProxy', '[MDS] Recommendations! missing AIT list length : ' + (queryList.length - odProgs.length));
                        }
                        var resultLength = odProgs.length;
                        if (resultLength) {
                            fnSort(odProgs);
                            var resCount = 0;
                            var finalList = [];
                            resultLength = (resultLength > that.MAX_DATA_COUNT) ? that.MAX_DATA_COUNT : resultLength;
                            for (i = 0; i < resultLength; i += 1) {
                                (function(index, ondemandItem) {
                                    ondemandItem.getThumbnail({
                                        updateCb : (receivedUrl) => {
                                            ondemandItem.thumbnailURL = receivedUrl;
                                            ondemandItem.getContentOwningLogo({
                                                updateCb: (receivedUrl) => {
                                                    ondemandItem.logoURL = receivedUrl;
                                                    finalList.push(ondemandItem);
                                                    if (++resCount >= resultLength) {
                                                        fnSort(finalList);
                                                        if (JSON.stringify(that._contentList) === JSON.stringify(finalList)) {
                                                            hx.log('debug', '[_getTodaysPicks] do not have to redraw!! it is same with previous one')
                                                            return;
                                                        }
                                                        that._contentList = finalList;
                                                        that._appRpcUtil.setHomeData(that._contentList, __MODEL__.EHomeDataType.EContent);
                                                        that._bRespondedTPK = true;
                                                    }
                                                },
                                                width: 46
                                            });
                                        },
                                        width : 178,
                                        defaultPath : "../../"
                                    });
                                })(i, odProgs[i]);
                            }
                        }
                        odProgs = null;
                    };
                    hx.svc.IP_EPG.getAITList(queryList).then(() => {
                        hx.log('hubProxy', '[MDS] Recommendations! Success? odProgs.length : ' + odProgs.length);
                        processRetObj();
                    }, () => {
                        hx.log('hubProxy', '[MDS] Recommendations! Fail? odProgs.length : ' + odProgs.length);
                        processRetObj();
                    });
                }, () => {

                });
            }
        }, () => {
            // nothing to do
        });
    }
    _networkConnection() {
        var that = this;
        var retVal = hx.svc.SETTING_UTIL.checkWireConnection();
        hx.log('hubProxy', '_networkConnection checkWireConnection ::' + retVal);
        if (!retVal) {
            retVal = hx.svc.SETTING_UTIL.checkWirelessConnection();
            hx.log('hubProxy', '_networkConnection checkWirelessConnection ::' + retVal);
        }
        if (!retVal) { // Local Area Net Disconnected
            that._localAreaNet = 'Disconnected';
            that._internet = 'Disconnected';
            that._checkNetworkStatus(that._localAreaNet,that._internet);
            if (that._appRpcUtil){
                that._appRpcUtil.setHomeData(undefined);
            }
        } else {
            //checkGatewayStatus
            retVal = hx.svc.SETTING_UTIL.checkInternetConnection();
        }
    }
    _checkNetworkStatus(paramLocalAreaNet, paramInternet) {
        hx.log('hubProxy', '_checkNetworkStatus paramLocalAreaNet ::' + paramLocalAreaNet +', paramInternet ::::' + paramInternet);
        var that = this, prevStatus = that._status;
        if ((paramLocalAreaNet === 'Connected') && (paramInternet === 'Connected')) {
            that._status = that.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED;
        } else if ((paramLocalAreaNet === 'Connected') && (paramInternet === 'Disconnected')) {
            that._status = that.TNetworkConnectionStatus.NETWORK_INTERNET_FAIL;
        } else if ((paramLocalAreaNet === 'Disconnected') && (paramInternet === 'Disconnected')) {
            that._status = that.TNetworkConnectionStatus.NETWORK_ALL_DISCONNECTED;
        }
        hx.log('hubProxy', '_checkNetworkStatus prevStatus !== that._status  :: ' + (prevStatus !== that._status) +', _bVisible : ' + that._bVisible);
        if (prevStatus !== that._status) {
            that._bAppListUpdate = true;
        }
    }
    _on_networkStatus(type, result) {
        var that = this;
        hx.log('hubProxy', '_on_networkStatus type ::' + type +', result ::::' + result);
        switch(type) {
            case that.TNeworkStatusType.GATEWAY:
                if (result === 'success') {
                    that._localAreaNet = 'Connected';
                    hx.svc.SETTING_UTIL.checkDnsStatus();
                } else {
                    that._localAreaNet = 'Disconnected';
                    that._internet = 'Disconnected';
                }
                break;
            case that.TNeworkStatusType.DNS:
                if (result === 'success') {
                    that._localAreaNet = 'Connected';
                    that._internet = 'Connected';
                    that._checkNetworkStatus(that._localAreaNet, that._internet);
                } else {
                    that._internet = 'Disconnected';
                    that._checkNetworkStatus(that._localAreaNet, that._internet);
                }
                break;
        }
        if (that._bAppListUpdate) {
            if (that._status == that.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED) {
                that._requestHomeDataList();
            } else if (that._status == that.TNetworkConnectionStatus.NETWORK_INTERNET_FAIL || that._status == that.TNetworkConnectionStatus.NETWORK_ALL_DISCONNECTED) {
                that._appRpcUtil.setHomeData(undefined);
                that._appList = [];
                that._playerList = [];
                that._contentList = [];
            }
            that._bAppListUpdate = false;
        }
    }
    _on_alKey (param : any) : boolean {
        var bConsumed : boolean = false;
        switch (param.alKey) {
            case hx.key.KEY_ESCAPE:
            case hx.key.KEY_POWER:
            case hx.key.KEY_BACK:
                this._hideHub();
                break;
            /*case hx.key.KEY_OPTION:
             reutrn false;*/
            default:
                break;
        }
        return bConsumed;
    }
    _hideHub () : void {
        this._appRpcUtil.hide();
    }
    _showHub () : void {
        var that = this;
        if (that.startHelp()) {
            return;
        }
        hx.log('hubProxy', '_checkNetworkStatus (that._status !== that.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED) : ' + (that._status !== that.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED)+' , _bAppListUpdate : ' + that._bAppListUpdate);
        if ((that._status !== that.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED) || (that._bAppListUpdate === true))  {
            that._bAppListUpdate = false;
            that._networkConnection();
        }
        if (that._status === that.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED) {
            setTimeout(() => {
                that._requestHomeDataList();
            }, 0);
        } else {
            that._appRpcUtil.setHomeData(undefined);
        }
        if (hx.emulMode) {
            //that._localAreaNet = 'Connected';
            //that._internet = 'Connected';
            //that._status = that.TNetworkConnectionStatus.NETWORK_ALL_CONNECTED;
            //that._requestHomeDataList();
        }
        that._acceptPrivacyPolicy = hx.svc.TV_PORTAL.isPrivacyPolicyAccepted();
        that._acceptTerms = hx.svc.TV_PORTAL.isTermsAndConditionsAccepted();
        that._appRpcUtil.setTermsAndConditions(this._acceptTerms);
        try {
            that._appRpcUtil.show();
        } catch (ex) {
            hx.log("error", "homemenu is in loading...");
            return;
        }
    }
    _on_alMessageToPage (param : any) : void {
        var that = this;
        if (param.query === 'show') {
            hx.log("warning", "this._bActive: ", this._bActive);
            if (that._bActive) {
                that._hideHub();
            } else {
                that._showHub();
            }
        } else if (param.query === 'forcedHide') {
            that._hideHub();
        } else if (param.query === 'updateLanguage') {
            that._updateLanguage();
        } else if (param.query === 'updateTVPortalUrl') {
            that._requestHomeDataList();
        } else if (param.query === 'refresh') {
            if (param.param === 'ondemand') {
                that._getTodaysPicks(true);
            } else if (param.param === 'players') {
                that._getFVCAppList(true);
            } else if (param.param === 'humaxapps') {
                that._getTvPortalAppList(true);
            }
        }
        if (param.path === '/addRecent') {
            hx.log('error', 'DO NOT SEND "/addRecent" DEPRETCATTED');
        } else if (param.path === '/updateChannel') {
            hx.log('error', 'DO NOT SEND "/updateChannel" DEPRETCATTED');
        }
    }
    _updateLanguage () : void {
        this._appRpcUtil.setLocalizationText({
            LOC_ON_DEMAND_ID: hx.l('LOC_ON_DEMAND_ID'),
            LOC_CLIVETV_ID : hx.l('LOC_CLIVETV_ID'),
            LOC_CTVGUIDE_ID : hx.l('LOC_CTVGUIDE_ID'),
            LOC_CRECORDINGS_ID : hx.l('LOC_CRECORDINGS_ID'),
            LOC_CTVAPPS_ID : hx.l('LOC_CTVAPPS_ID'),
            LOC_CMEDIACENTER_ID : hx.l('LOC_MEDIA_CENTRE_ID'),
            LOC_CSETTINGS_ID : hx.l('LOC_CSETTINGS_ID'),
            LOC_CHOME_ID : hx.l('LOC_CHOME_ID'),
            LOC_CRECENT_ID : hx.l('LOC_CRECENT_ID'),
            LOC_MEDIA_ID : hx.l('LOC_MEDIA_ID'),
            LOC_TITLEDELETE_ID : hx.l('LOC_TITLEDELETE_ID'),
            LOC_HUB_RECENT_DELETE_ID : hx.l('LOC_HUB_RECENT_DELETE_ID'),
            LOC_CANNOT_ACCESS_APP_ID : hx.l('LOC_CANNOT_ACCESS_APP_ID'),
            LOC_YES_ID : hx.l('LOC_YES_ID'),
            LOC_NO_ID : hx.l('LOC_NO_ID'),
            LOC_EDIT_ID : hx.l('LOC_EDIT_ID'),
            LOC_MOVE_ID : hx.l('LOC_MOVE_ID'),
            LOC_DELETE_ID : hx.l('LOC_DELETE_ID'),
            LOC_TVAPPS_DISCONNECT_MSG_ID : hx.l('LOC_TVAPPS_DISCONNECT_MSG_ID'),
            LOC_RECENTLY_ID : hx.l('LOC_RECENTLY_ID'),
            LOC_ALL_APPS_02_ID : hx.l('LOC_ALL_APPS_02_ID')
        });
        this._appRpcUtil.setLanguage(hx.svc.SETTING_UTIL.getLanguage());
    }
    startHelp () : boolean {
        if (!hx.config.useHelpPage) {
            hx.log('debug', '[startHelp] !hx.config.useHelpPage');
            return false;
        }
        var helpViewObj: any;
        var helpCmpt : any;
        var wrap : any;
        helpViewObj = hx.svc.WEB_STORAGE.getHelp();
        var target = hx.al.entity._repos.get("CPgMain") || this;
        wrap = $(document.getElementById('wrap'));
        if (!helpViewObj.home) {
            helpCmpt = hx.al.component('HelpView', {
                'parent' : target,
                '_type' : 'home'
            });
            wrap[0].appendChild(helpCmpt.$[0]);
            hx.al.compose(target).append(helpCmpt);
            helpCmpt.on();
            return true;
        }
        return false;
    }
}
export = cCPgHubProxy;
