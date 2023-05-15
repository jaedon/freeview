/**
 * Created by bglee on 2014-10-29.
 */
///<reference path="../../def/sugar.d.ts"/>
///<reference path="../../def/prism.d.ts"/>
///<reference path="../../def/controls.d.ts"/>
///<reference path="../../layer_domain/service/ip_epg/engine/interface_freeview_play.d.ts" />

import __PAGE_KERNEL__ = require("pages/page_kernel");
import __PAGE_ON_DEMAND__ = require("prismux/prism.page.on_demand");

declare var hx: ifSugar;

var SERVER_URL = hx.svc.SETTING_UTIL.getTVPortalUrl();
var log = function() {
    var countMaxLine = 80;
    var charPadding = "=";
    var getPadder = function(aLen: number) {
        var ret = charPadding;
        var len = Math.floor(aLen);
        for (var i = 1; i < len; i++) {
            ret += charPadding;
        }
        return ret;
    };
    return function(aTitle: string, aData: any = "", aLevel: string = "debug") {
        var lenPadder = countMaxLine - aTitle.length - 2;
        hx.log(aLevel, charPadding + " " + aTitle + " " +  getPadder(lenPadder));
        if (typeof aData === "object") {
            for (var key in aData) {
                hx.log(aLevel, key + ": " + aData[key]);
            }
        } else {
            hx.log(aLevel, aData);
        }
        hx.log(aLevel, charPadding + " END " + aTitle + " " +  getPadder(lenPadder - 5));
    };
}();

class CPgOnDemand extends __PAGE_KERNEL__ {
    private _page: __PAGE_ON_DEMAND__;
    private _imageDlg;
    constructor() {
        super();
    }
    create(param) {
        SERVER_URL = hx.svc.SETTING_UTIL.getTVPortalUrl();
        if (hx.svc.SETTING_UTIL.getNetworkAvailable()) {
            hx.svc.SETTING_UTIL.checkInternetConnection();
        }
        var page = new __PAGE_ON_DEMAND__($('#wrap'));
        hx.il.updatePageBackground();
        hx.al.compose(this);
        hx.logadd('CPgOnDemand');

        hx.log("info", "on-demand page create");
        page.setDataRequestor(this.requestorPortalApi);
        page.setOperationRequester(this.requestorOperation);
        this._page = page;
        this._initEventHandler();

        //terminate all apps
        var dsrvAppMgr : any = hx.svc.APP_MGR;
        dsrvAppMgr.destroyHbbTvDeeplinkApplication();
    }
    destroy(param?) {
        if (this._imageDlg) {
            this._imageDlg.destroy();
            delete this._imageDlg;
        }
        this._page.destroy();
        hx.log("info", "on-demand page destroy");
    }
    _initEventHandler() {
        var dsrvAppMgr = hx.svc.APP_MGR;
        var dsrvAvCtrl = hx.svc.AV_CTRL;
        dsrvAppMgr.addEventCb('HbbTVAppUnloaded', this, () => {
            //dsrvAvCtrl.unBlockAV();
        });
    }
    _on_alKey(aParam) {
        var keyCode = aParam.alKey;
        var handled =  this._page.doKey(keyCode);
        if (!handled && (keyCode === hx.key.KEY_ESCAPE || keyCode === hx.key.KEY_BACK)) {
            this.sendEventToEntity({
                alEvent: 'alChangePage',
                target: 'CPgLiveController',
                me: this
            });
            handled = true;
        }
        return handled;
    }
    private requestorOperation = (() => {
        return {
            exit: (target?: string) => {
                this.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: target || 'CPgLiveController',
                    me: this
                });
            },
            getNetworkAvailable: function(aFnSuccess, aFnFail) {
                if (hx.svc.SETTING_UTIL.getInternetAvailable()) {
                    aFnSuccess();
                } else {
                    aFnFail();
                }
            },
            isNeedHelp: function() {
                if (!hx.config.useHelpPage) {
                    hx.log('debug', '[startHelp] !hx.config.useHelpPage');
                    return false;
                }
                return !hx.svc.WEB_STORAGE.getHelp().ondemand;
            },
            setVisit: function() {
                hx.svc.WEB_STORAGE.setHelp({'ondemand': true});
            },
            deleteCookies: function(): void {
                var category = [
                    'meta','image',
                    'moved', 'auth'
                ];
                var db = {};
                category.forEach((name) => {
                    db[name] = {};
                });
                hx.log('debug', 'db\'s category: ' + Object.keys(db));
                var dsrvIpEpg = hx.svc.IP_EPG;
                dsrvIpEpg._cache_controller.setDb(db);
                dsrvIpEpg._adapter._engine.setBaseUrl(null);
                hx.svc.SETTING_UTIL.deleteCookies();
                dsrvIpEpg.getAuthToken(-1);//reset All cached data on HTTP and reset Auth
            },
            isNeedTerms: function (): boolean {
                var ret = hx.svc.WEB_STORAGE.isNeedTerms();
                return ret;
            }
        }
    })();

    private requestorPortalApi : ifPortalRequestorAPI = {
        isLogged: () => {
            return hx.svc.TV_PORTAL.isLogged();
        },
        login: (aCallbackSuccess, aCallbackFail) => {
            hx.svc.TV_PORTAL.login({
                fn: (aData) => {
                    aCallbackSuccess(aData);
                },
                fnError: (aData) => {
                    aCallbackFail(aData);
                }
            });
        },
        getActiveProfile: (aCallbackSuccess, aCallbackFail) => {
            hx.svc.TV_PORTAL.getActiveProfile({
                fn: (aData) => {
                    aCallbackSuccess(aData);
                },
                fnError: (aData) => {
                    aCallbackFail(aData);
                }
            });
        },
        getPrivacyPolicy: (aCBSuccess: (aHTML: string) => void, aCBFail: (aHTML: string) => void) => {
            var privacyText = hx.svc.TV_PORTAL.getPrivacyPolicyText(aCBSuccess, aCBFail);
            if (privacyText) {
                aCBSuccess(privacyText);
            }
        },
        getTermsAndConditions: (aCBSuccess: (aHTML: string) => void, aCBFail: (aHTML: string) => void) => {
            var termsText = hx.svc.TV_PORTAL.getTermsAndConditionsText(aCBSuccess, aCBFail);
            if (termsText) {
                hx.svc.WEB_STORAGE.readTerms();
                aCBSuccess(termsText);
            }
        },
        getModelPlayers: (aCallback: (aModelPlayers: ifPortalModelApps[]) => void) => {
            var players = [];
            var TOrderPlayerType = ['/promoted_players', '/other_players'];
            var processRetObj = function() {
                players.sort(function(a, b) {
                    var ret = 0;
                    a = a instanceof Array ? a[0] : a;
                    b = b instanceof Array ? b[0] : b;
                    ret = TOrderPlayerType.indexOf(a.PlayerType) - TOrderPlayerType.indexOf(b.PlayerType);
                    if (ret === 0) {
                        ret = a.appid - b.appid;
                    }
                    return ret;
                });
                if (!hx.svc.TV_PORTAL.isPrivacyPolicyAccepted()) {
                    hx.log('debug', '[getModelPlayers] fail to HUMAX portal players, !hx.svc.TV_PORTAL.isPrivacyPolicyAccepted()');
                    aCallback(players);
                    return;
                }
                var paramReq = {
                    appsType: "STBLAUNCHER",
                    authTicket: hx.svc.TV_PORTAL.getAuthTicket()
                };
                log("=> api/getModelAppsList", paramReq);
                $.getJSON(_API("api/getModelAppsList"), paramReq).done((aData: ifPortalModelApps[]) => {
                    log("<= api/getModelAppsList", aData);
                    aData.map((aData) => {
                        aData.focusable = true;
                    });
                    aCallback(players.concat(aData));
                }).fail(function() {
                    hx.log("debug", "getModelPlayers fail");
                    log("<= api/aData");
                    aCallback(players);
                });
            };
            hx.svc.IP_EPG.getApplication().then((info) => {
                var queryList = [];
                if (info) {
                    var parsed = hx.svc.IP_EPG.parse.xml.application(info);
                    var fvcPlayers;
                    for (var key in parsed) {
                        if (TOrderPlayerType.indexOf(key) > -1) {    // to skip invalid_players for robustness spec
                            fvcPlayers = parsed[key];
                            for (var i = 0; i < fvcPlayers.length; i++) {
                                addAITQueryToQueryList({
                                    name: fvcPlayers[i].title,
                                    focusable: true,
                                    icons: [{
                                        ImageType: "STBLAUNCHER_NORMAL",
                                        Size: "276*140",
                                        Url: fvcPlayers[i].image
                                    }],
                                    contenturl: fvcPlayers[i].ait,
                                    PlayerType: key,
                                    appid: i
                                });
                            }
                        }
                    }
                }
                function checkTemplateAIT(xml) {
                    if (!xml) {
                        hx.log('debug', '[CPgOnDemand] FAIL!!! checkTemplateAIT : argu(xml) is null');
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
                    hx.log('debug', '[CPgOnDemand] FAIL!!! checkTemplateAIT : [' + version + '][' + type + '] / ' + this.name);
                    return false;
                }
                function addAITQueryToQueryList(aPlayer) {
                    var subRetDef = Q.defer();
                    subRetDef.promise.then((aXml) => {
                        if (checkTemplateAIT(aXml)) {
                            players.push(aPlayer);
                        }
                    });
                    var params = hx.svc.SETTING_UTIL.getNetworkId().map(function(nid) {
                        return 'nids[]=' + nid;
                    });
                    params.push('lloc=playerpage');
                    var query = {
                        url: [aPlayer.contenturl, params.join('&')].join(aPlayer.contenturl.indexOf('?') !== -1 ? '&' : '?'),
                        retDef: subRetDef
                    };
                    queryList.push(query);
                }
                hx.log('debug', '[CPgOnDemand] queryList.length : ' + queryList.length);
                hx.svc.IP_EPG.getAITList(queryList).then(function () {
                    hx.log('debug', '[CPgOnDemand] Success? players.length : ' + players.length);
                    processRetObj();
                }, () => {
                    hx.log('debug', '[CPgOnDemand] Fail? players.length : ' + players.length);
                    processRetObj();
                });
            }, () => {
                processRetObj();
            });
        },
        getModelApps: (aCallback: (aModelApps: ifPortalModelApps[], aIsUpdate: boolean, aIndex: number) => void) => {
            var countCompleate = 0;
            var countApps = 0;
            var appsType = [
                "DEFAULTAPP"
            ];
            if (!hx.svc.TV_PORTAL.isPrivacyPolicyAccepted()) {
                hx.log('debug', '[getModelApps] fail to HUMAX portal apps, !hx.svc.TV_PORTAL.isPrivacyPolicyAccepted()');
                aCallback([], false, 0);
                return;
            }
            appsType.map((aType, aIndex) => {
                var paramReq = {
                    appsType: aType,
                    authTicket: hx.svc.TV_PORTAL.getAuthTicket()
                };
                log("=> api/getModelAppsList", paramReq);
                var savedOrder = hx.svc.WEB_STORAGE.getItem("orderingTvApps");
                var listOrder = savedOrder ? savedOrder.split(",") : null;
                // XXX: if appsType.length > 1 may cause problem
                $.getJSON(_API("api/getModelAppsList"), paramReq).done((aData) => {
                    log("<= api/getModelAppsList", aData);
                    if (savedOrder) {
                        if (listOrder) {
                            var listToReplace = [];
                            var listDoesNotFound = [];
                            var index;
                            for (var i = 0; i < aData.length; i++) {
                                index = listOrder.indexOf(aData[i].appid)
                                listOrder[index] = null;
                                if (index !== -1) {
                                    listToReplace[index] = aData[i];
                                } else {
                                    listDoesNotFound.push(aData[i]);
                                }
                            }
                            for (var i = 0; i < listDoesNotFound.length; i++) {
                                listToReplace.push(listDoesNotFound[i]);
                            }
                            aData = listToReplace;
                        }
                    }

                    if (countCompleate++ === 0) {
                        aCallback(aData, false, 0);
                    } else {
                        if (aData.length) {
                            aCallback(aData, true, countApps);
                        }
                    }
                    countApps += aData.length;
                }).fail(function() {
                    hx.log("debug", "getModelApps fail");
                    log("<= api/getModelAppsList");
                    if (countCompleate++ === 0) {
                        aCallback([], false, 0);
                    }
                });
            });
        },
        launchApp: (app: any, aNeedToCheckUSB: boolean = false, aIsFVC: boolean = false): boolean => {
            var url = app.contenturl;
            var appid = app.appid;
            var name = app.name;
            var fnExit = () => {
                this.sendEventToEntity({
                    'alEvent' : 'alChangePage',
                    'target' : 'CPgLiveController',
                    'me' : this
                });
                this.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgInfoHub?backToLive',
                    me : this
                });
            };

            aNeedToCheckUSB = false; // the hbbtv jumper funtion is no more used here -> the function had moved to page_proxyhomemenu.js
            var svcSettingUtil = hx.svc.SETTING_UTIL;
            var hbbUrl = svcSettingUtil.getHbbTvUrl();
            if (hbbUrl && aNeedToCheckUSB) {
                hx.log("debug", "hbb url detect: " + hbbUrl);
                fnExit();
                hx.svc.APP_MGR.createPortalApp();
            } else if (url) {
                var address = svcSettingUtil.getallAddress();
                var dns = false;
                if (address) {
                    if (address.dns1st && (address.dns1st !== '0.0.0.0')) {
                        dns = true;
                    }
                    if (address.dns2nd && (address.dns2nd !== '0.0.0.0')) {
                        dns = true;
                    }
                }
                if (svcSettingUtil.getNetworkAvailable() && dns) {
                    var isTSR = hx.svc.TSR.isTsrOngoing();
                    var onLoaded = (appRpcUtil) => {
                        if (isTSR) {
                            hx.log("debug", "clearTimer()");
                            hx.svc.TSR.clearTimer();
                        }
                        this.sendEventToEntity({
                            alEvent : 'alMessageToPage',
                            target : 'CPgLiveController?hideNoChannelConfirm'
                        });
                    };
                    var onUnloaded = (appInfo, item) => {
                        this.sendEventToEntity({
                            alEvent : 'alMessageToPage',
                            target : 'CPgLiveController?restoreNoChannelConfirm'
                        });

                        if (!hx.svc.AV_CTRL.isLiveOnGoing()) {
                            if (!hx.svc.AV_CTRL.lastChannel()) {
                                hx.svc.AV_CTRL.availableChannel();
                            }
                        }
                    };
                    //if (_isServerURL(url) || aNeedToCheckUSB) {
                    //    url = svcSettingUtil.getTVPortalUrl() || url;
                    //    log("== open URL", url);
                    //    fnExit();
                    //    fnOpen(url);
                    //} else {
                    if (aIsFVC) {
                        log("<= request URL", url);
                        fnExit();
                        var params = hx.svc.SETTING_UTIL.getNetworkId().map(function(nid) {
                            return 'nids[]=' + nid;
                        });
                        params.push('lloc=playerpage');
                        url = [ url, params.join('&') ].join(url.indexOf('?') !== -1 ? '&' : '?');
                        hx.il.openDeepLink(name, url, undefined, undefined, undefined, this);
                    } else {
                        log("=> request portal app name", name);
                        fnExit();
                        hx.il.launchApp(name, appid, undefined, this);
                        //$.getJSON(url).done((aData) => {
                        //    log("<= request URL", aData);
                        //    fnExit();
                        //    hx.il.launchApp(aData.Url, aData.appid);
                        //    //fnOpen(aData.Url);
                        //}).fail(() => {
                        //    throw "response fail";
                        //});
                    }
                    //}
                } else {
                    return false;
                    //TODO: Network is unavailable.
                    //var imageDlg = prism.createImageDlg(null);
                    //this._imageDlg = imageDlg;
                    //this._imageDlg.open();
                }
            }
            return true;
        },
        setTvAppsArrangement(aAppIDList: string[]) {
            hx.svc.WEB_STORAGE.setItem("orderingTvApps", aAppIDList.join());
        },
        setAcceptPrivacyTerms: (aPrivacyAgree: boolean, aTermsAgree: boolean, aCallback: (aSuccess: boolean) => void) => {
            hx.svc.TV_PORTAL.acceptPrivacyTerms({
                privacy : aPrivacyAgree,
                terms : aTermsAgree,
                fn : (aData) => {
                    aCallback(aData ? aData.status : null);
                },
                fnError : (aData) => {
                    aCallback(aData ? aData.status : null);
                }
            });
        },
        setAcceptPrivacy: (aAgree: boolean, aCallback: (aSuccess: boolean) => void) => {
            hx.svc.TV_PORTAL.acceptPrivacyPolicy({
                accept : aAgree,
                fn : (aData) => {
                    aCallback(aData ? aData.status : null);
                },
                fnError : (aData) => {
                    aCallback(aData ? aData.status : null);
                }
            });
        },
        setAcceptTerms: (aAgree: boolean, aCallback: (aSuccess: boolean) => void) => {
            hx.svc.TV_PORTAL.acceptTermsAndConditions({
                accept : aAgree,
                fn : (aData) => {
                    aCallback(aData ? aData.status : null);
                },
                fnError : (aData) => {
                    aCallback(aData ? aData.status : null);
                }
            });
        }
    }
}
function _isServerURL(aURL: string) {
    return aURL === SERVER_URL;
}
function _API(aAPI: string) {
    var ret = SERVER_URL + "/" + aAPI;
    hx.log("debug", "API: " + ret);
    return ret;
}
export = CPgOnDemand;
