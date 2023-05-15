/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvWebStorage.js
/* jslint sloppy:true, nomen:true, vars:true */
//    WEB_STORAGE: ['layer_domain/service/dl_dsrvWebStorage.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var setTVAppInfo = function (obj) {
        localStorage.setItem('TVAPPINFO', JSON.stringify(obj));
    };
    var getTVAppInfo = function () {
        var ret = localStorage.getItem('TVAPPINFO');
        if (ret) {
            return JSON.parse(ret);
        }
        return null;
    };
    var mZapInfo = {
        channelCcid: null,
        svcListGroup: null,
        deliveryType: null,
        timeStamp: 0
    };
    var mZapBackInfo = {
        channelCcid: null,
        svcListGroup: null,
        deliveryType: null
    };
    var fnLastTvRadioSvcInfo = {
        channelTvCcid: null,
        channelRadioCcid: null
    };
    var otanotification = {
        otanoti: null,
        otacheck: null
    };
    var cDLWebStorage = (function (_super) {
        __extends(cDLWebStorage, _super);
        function cDLWebStorage(aServiceName) {
            _super.call(this, aServiceName);
            hx.log("dsvc", "initializer service - hx.svc.WEB_STORAGE");
            var storage = this.fnGetLocalStorage('zapInfo', ['channelCcid', 'svcListGroup', 'deliveryType']);
            mZapInfo.channelCcid = storage.channelCcid;
            mZapInfo.svcListGroup = storage.svcListGroup;
            mZapInfo.deliveryType = storage.deliveryType;
            this.fnInitLastTvRadioInfo();
        }
        cDLWebStorage.prototype.deleteWholeItems = function () {
            this.fnDeleteWholeLocalStorage();
        };
        cDLWebStorage.prototype.getLastService = function (param) {
            // Temporary work around.
            if ((Number(mZapInfo.svcListGroup)) || (0 === mZapInfo.svcListGroup) || (null === mZapInfo.svcListGroup)) {
                mZapInfo.svcListGroup = 'TV';
                this.fnSetLocalStorage('zapInfo', {
                    'svcListGroup': 'TV'
                });
                if (param !== undefined) {
                    if (param.cb !== undefined) {
                        param.cb();
                    }
                }
            }
            return {
                ccid: mZapInfo.channelCcid,
                group: mZapInfo.svcListGroup.toString(),
                delivery: Number(mZapInfo.deliveryType),
                timeStamp: Number(mZapInfo.timeStamp)
            };
        };
        cDLWebStorage.prototype.updateLastService = function (p) {
            if (p.ccid !== undefined) {
                this.fnUpdateLastService(p.ccid);
            }
            if (p.group !== undefined) {
                this.fnUpdateLastGroup(p.group);
            }
            if (p.deliveryType !== undefined) {
                this.fnUpdateLastDeliveryType(p.deliveryType);
            }
            mZapInfo.timeStamp = new Date().getTime();
            // update time stamp
            this.fnUpdateLastTvRadioService(p.ccid, p.group);
        };
        cDLWebStorage.prototype.getLastGroup = function (param) {
            // Temporary work around.
            if ((Number(mZapInfo.svcListGroup)) || (0 === mZapInfo.svcListGroup) || (null === mZapInfo.svcListGroup)) {
                mZapInfo.svcListGroup = 'TV';
                this.fnSetLocalStorage('zapInfo', {
                    'svcListGroup': 'TV'
                });
                if (param !== undefined) {
                    if (param.cb !== undefined) {
                        param.cb();
                    }
                }
            }
            return mZapInfo.svcListGroup.toString();
        };
        cDLWebStorage.prototype.updateLastGroup = function (p) {
            if (p !== undefined) {
                this.fnUpdateLastGroup(p);
            }
        };
        cDLWebStorage.prototype.getLastDeliveryType = function () {
            return Number(mZapInfo.deliveryType);
        };
        cDLWebStorage.prototype.setPowerTimerLastGroup = function (param) {
            this.fnSetLocalStorage('PowerTimerLastGroup', {
                'grpName': param.grpName
            });
        };
        cDLWebStorage.prototype.getPowerTimerLastGroup = function () {
            var grpName = this.fnGetLocalStorage('PowerTimerLastGroup', ['grpName']);
            return grpName;
        };
        /*
         * getting getBackService
         */
        cDLWebStorage.prototype.getBackService = function () {
            if (mZapBackInfo === undefined) {
                return {
                    ccid: null,
                    group: null,
                    delivery: null
                };
            }
            if ((mZapBackInfo.channelCcid === mZapInfo.channelCcid) && (mZapBackInfo.svcListGroup === mZapInfo.svcListGroup) && (mZapBackInfo.deliveryType === mZapInfo.deliveryType)) {
                // not allow first service and same service
                return {
                    ccid: null,
                    group: null,
                    delivery: null
                };
            }
            return {
                ccid: mZapBackInfo.channelCcid,
                group: (mZapBackInfo.svcListGroup === null) ? null : mZapBackInfo.svcListGroup.toString(),
                delivery: (mZapBackInfo.deliveryType === null) ? null : Number(mZapBackInfo.deliveryType)
            };
        };
        cDLWebStorage.prototype.clearBackService = function () {
            mZapBackInfo.channelCcid = null;
            mZapBackInfo.svcListGroup = null;
            mZapBackInfo.deliveryType = null;
        };
        cDLWebStorage.prototype.updateLastDeliveryType = function (p) {
            if (p.deliveryType !== undefined) {
                this.fnUpdateLastDeliveryType(p.deliveryType);
            }
        };
        cDLWebStorage.prototype.getFavListID = function () {
            var a = this.fnGetLocalStorage('channelList', ['favID1', 'favID2', 'favID3', 'favID4', 'favID5']);
            return {
                'favID1': a.favID1,
                'favID2': a.favID2,
                'favID3': a.favID3,
                'favID4': a.favID4,
                'favID5': a.favID5
            };
        };
        cDLWebStorage.prototype.setFavListID = function (p) {
            if (p.favID1) {
                this.fnSetLocalStorage('channelList', {
                    'favID1': p.favID1
                });
            }
            if (p.favID2) {
                this.fnSetLocalStorage('channelList', {
                    'favID2': p.favID2
                });
            }
            if (p.favID3) {
                this.fnSetLocalStorage('channelList', {
                    'favID3': p.favID3
                });
            }
            if (p.favID4) {
                this.fnSetLocalStorage('channelList', {
                    'favID4': p.favID4
                });
            }
            if (p.favID5) {
                this.fnSetLocalStorage('channelList', {
                    'favID5': p.favID5
                });
            }
        };
        cDLWebStorage.prototype.defaultZapInfo = function () {
            var a;
            this.fnRemoveLocalStorage('zapInfo', ['channelCcid', 'svcListGroup', 'deliveryType']);
            a = this.fnGetLocalStorage('zapInfo', ['channelCcid', 'svcListGroup', 'deliveryType']);
            mZapInfo.channelCcid = a.channelCcid;
            mZapInfo.svcListGroup = a.svcListGroup;
            mZapInfo.deliveryType = a.deliveryType;
            this.fnDefaultLastTvRadioInfo();
        };
        /*
         * Information of Recents
         */
        cDLWebStorage.prototype.updateRecentItems = function (p) {
            this.fnSetLocalStorage('recent', {
                'recentData': p.recentData,
                'recentType': p.recentType
            });
        };
        cDLWebStorage.prototype.getRecentItems = function (p) {
            var ret = this.fnGetLocalStorage('recent', ['recentData', 'recentType']);
            return ret;
        };
        cDLWebStorage.prototype.getLastTvRadioService = function (data) {
            var lastSvcCcid = null;
            if (data === 'TV') {
                lastSvcCcid = fnLastTvRadioSvcInfo.channelTvCcid;
            }
            else if (data === 'RADIO') {
                lastSvcCcid = fnLastTvRadioSvcInfo.channelRadioCcid;
            }
            return lastSvcCcid;
        };
        /*
         * //////antenna setting//////
         */
        cDLWebStorage.prototype.saveAntIdToLocal = function (param) {
            if (param.index === 0) {
                this.fnSetLocalStorage('AntId', {
                    'antId1': param.antId
                });
            }
            else if (param.index === 1) {
                this.fnSetLocalStorage('AntId', {
                    'antId2': param.antId
                });
            }
            else if (param.index === 2) {
                this.fnSetLocalStorage('AntId', {
                    'antId3': param.antId
                });
            }
            else if (param.index === 3) {
                this.fnSetLocalStorage('AntId', {
                    'antId4': param.antId
                });
            }
            else if (param.index === 4) {
                this.fnSetLocalStorage('AntId', {
                    'antId5': param.antId
                });
            }
            else if (param.index === 5) {
                this.fnSetLocalStorage('AntId', {
                    'antId6': param.antId
                });
            }
            else if (param.index === 6) {
                this.fnSetLocalStorage('AntId', {
                    'antId7': param.antId
                });
            }
            else if (param.index === 7) {
                this.fnSetLocalStorage('AntId', {
                    'antId8': param.antId
                });
            }
        };
        cDLWebStorage.prototype.getAntIdListFromLocal = function () {
            var antIdList = this.fnGetLocalStorage('AntId', ['antId1', 'antId2', 'antId3', 'antId4', 'antId5', 'antId6', 'antId7', 'antId8']);
            return antIdList;
        };
        cDLWebStorage.prototype.saveSearchOptionsToLocal = function (param) {
            this.fnSetLocalStorage('searchOptions', {
                'network': param.network,
                'scramble': param.scramble,
                'channel': param.channel
            });
        };
        cDLWebStorage.prototype.getSearchOptionsFromLocal = function () {
            var searchOptions = this.fnGetLocalStorage('searchOptions', ['network', 'scramble', 'channel']);
            return searchOptions;
        };
        cDLWebStorage.prototype.saveTpIdxToLocal = function (param) {
            if (param.position === 0) {
                this.fnSetLocalStorage('tpIdx', {
                    'Idx1': param.Idx
                });
            }
            else if (param.position === 1) {
                this.fnSetLocalStorage('tpIdx', {
                    'Idx2': param.Idx
                });
            }
            else if (param.position === 2) {
                this.fnSetLocalStorage('tpIdx', {
                    'Idx3': param.Idx
                });
            }
            else if (param.position === 3) {
                this.fnSetLocalStorage('tpIdx', {
                    'Idx4': param.Idx
                });
            }
            else if (param.position === 4) {
                this.fnSetLocalStorage('tpIdx', {
                    'Idx5': param.Idx
                });
            }
            else if (param.position === 5) {
                this.fnSetLocalStorage('tpIdx', {
                    'Idx6': param.Idx
                });
            }
            else if (param.position === 6) {
                this.fnSetLocalStorage('tpIdx', {
                    'Idx7': param.Idx
                });
            }
            else if (param.position === 7) {
                this.fnSetLocalStorage('tpIdx', {
                    'Idx8': param.Idx
                });
            }
        };
        cDLWebStorage.prototype.getTpIdxListFromLocal = function () {
            var tpIdxList = this.fnGetLocalStorage('tpIdx', ['Idx1', 'Idx2', 'Idx3', 'Idx4', 'Idx5', 'Idx6', 'Idx7', 'Idx8']);
            return tpIdxList;
        };
        cDLWebStorage.prototype.saveUserTpParamToLocal = function (param) {
            this.fnSetLocalStorage('userTpParam' + (param.index + 1), {
                'userFreq': param.userFreq,
                'userPola': param.userPola,
                'userSymbol': param.userSymbol,
                'userTrans': param.userTrans,
                'userFEC': param.userFEC
            });
        };
        cDLWebStorage.prototype.saveUserTpParamToLocalT = function (param) {
            this.fnSetLocalStorage('userTpParamT', {
                'userChannel': param.userChannel,
                'userFreq': param.userFreq,
                'userBand': param.userBand,
                'userNetworkSearch': param.userNetworkSearch
            });
        };
        cDLWebStorage.prototype.saveUserTpParamToLocalT2 = function (param) {
            this.fnSetLocalStorage('userTpParamT', {
                'userChannel': param.userChannel,
                'userFreq': param.userFreq,
                'userTransmission': param.userTransmission,
                'userBand': param.userBand,
                'userNetworkSearch': param.userNetworkSearch
            });
        };
        cDLWebStorage.prototype.getUserTpParamFromLocalT = function () {
            var userTpParam = this.fnGetLocalStorage('userTpParamT', ['userChannel', 'userFreq', 'userBand', 'userNetworkSearch']);
            return userTpParam;
        };
        cDLWebStorage.prototype.getUserTpParamFromLocalT2 = function () {
            var userTpParam = this.fnGetLocalStorage('userTpParamT', ['userChannel', 'userFreq', 'userTransmission', 'userBand', 'userNetworkSearch']);
            return userTpParam;
        };
        cDLWebStorage.prototype.getUserTpParam1FromLocal = function () {
            var userTpParam = this.fnGetLocalStorage('userTpParam1', ['userFreq', 'userPola', 'userSymbol', 'userTrans', 'userFEC']);
            return userTpParam;
        };
        cDLWebStorage.prototype.getUserTpParam2FromLocal = function () {
            var userTpParam = this.fnGetLocalStorage('userTpParam2', ['userFreq', 'userPola', 'userSymbol', 'userTrans', 'userFEC']);
            return userTpParam;
        };
        cDLWebStorage.prototype.getUserTpParam3FromLocal = function () {
            var userTpParam = this.fnGetLocalStorage('userTpParam3', ['userFreq', 'userPola', 'userSymbol', 'userTrans', 'userFEC']);
            return userTpParam;
        };
        cDLWebStorage.prototype.getUserTpParam4FromLocal = function () {
            var userTpParam = this.fnGetLocalStorage('userTpParam4', ['userFreq', 'userPola', 'userSymbol', 'userTrans', 'userFEC']);
            return userTpParam;
        };
        cDLWebStorage.prototype.getUserTpParam5FromLocal = function () {
            var userTpParam = this.fnGetLocalStorage('userTpParam5', ['userFreq', 'userPola', 'userSymbol', 'userTrans', 'userFEC']);
            return userTpParam;
        };
        cDLWebStorage.prototype.getUserTpParam6FromLocal = function () {
            var userTpParam = this.fnGetLocalStorage('userTpParam6', ['userFreq', 'userPola', 'userSymbol', 'userTrans', 'userFEC']);
            return userTpParam;
        };
        cDLWebStorage.prototype.getUserTpParam7FromLocal = function () {
            var userTpParam = this.fnGetLocalStorage('userTpParam7', ['userFreq', 'userPola', 'userSymbol', 'userTrans', 'userFEC']);
            return userTpParam;
        };
        cDLWebStorage.prototype.getUserTpParam8FromLocal = function () {
            var userTpParam = this.fnGetLocalStorage('userTpParam8', ['userFreq', 'userPola', 'userSymbol', 'userTrans', 'userFEC']);
            return userTpParam;
        };
        cDLWebStorage.prototype.setLastSearchType = function (param) {
            if (param.lastSearchType === undefined || param.lastSearchType === null) {
                return;
            }
            this.fnSetLocalStorage('LastSearchType', {
                'searchType': param.lastSearchType
            });
        };
        cDLWebStorage.prototype.getLastSearchType = function () {
            var ret = this.fnGetLocalStorage('LastSearchType', ['searchType']);
            return ret;
        };
        /*
         * //////other settings//////
         */
        cDLWebStorage.prototype.setAutoPowerDown = function (param) {
            var setVal;
            if (param.bIsOn === true) {
                setVal = true;
            }
            else {
                setVal = false;
            }
            this.fnSetLocalStorage('AutomaticPowerDown', {
                'isOn': setVal
            });
        };
        cDLWebStorage.prototype.getAutoPowerDown = function () {
            var ret = this.fnGetLocalStorage('AutomaticPowerDown', ['isOn']);
            if (ret.isOn === null || ret.isOn === undefined) {
                // set default
                if (hx.config.useAutoPowerDown === 0) {
                    this.fnSetLocalStorage('AutomaticPowerDown', {
                        'isOn': false
                    });
                }
                else {
                    this.fnSetLocalStorage('AutomaticPowerDown', {
                        'isOn': true
                    });
                }
                ret = this.fnGetLocalStorage('AutomaticPowerDown', ['isOn']);
            }
            return ret;
        };
        cDLWebStorage.prototype.setPowerOffTimer = function (param) {
            if (param.hour === null || param.min === null) {
                this.fnRemoveLocalStorage('PowerOffTimer', ['hour', 'min', 'isOn']);
                return;
            }
            this.fnSetLocalStorage('PowerOffTimer', {
                'hour': param.hour,
                'min': param.min,
                'isOn': param.isOn
            });
        };
        cDLWebStorage.prototype.getPowerOffTimer = function () {
            var ret = this.fnGetLocalStorage('PowerOffTimer', ['hour', 'min', 'isOn']);
            if ((ret.hour === null || ret.hour === undefined) || (ret.min === null || ret.min === undefined) || (ret.isOn === null || ret.isOn === undefined)) {
                this.fnSetLocalStorage('PowerOffTimer', {
                    'hour': '00',
                    'min': '00',
                    'isOn': 'Off'
                });
                ret = this.fnGetLocalStorage('PowerOffTimer', ['hour', 'min', 'isOn']);
            }
            return ret;
        };
        cDLWebStorage.prototype.setServerActivationTime = function (param) {
            if (param.startHour === null || param.startMin === null || param.endHour === null || param.endMin === null) {
                this.fnRemoveLocalStorage('ServerActivationTime', ['startHour', 'startMin', 'endHour', 'endMin', 'durationMin', 'isOn']);
                return;
            }
            this.fnSetLocalStorage('ServerActivationTime', {
                'startHour': param.startHour,
                'startMin': param.startMin,
                'endHour': param.endHour,
                'endMin': param.endMin,
                'durationMin': param.durationMin,
                'isOn': param.isOn
            });
        };
        cDLWebStorage.prototype.getServerActivationTime = function () {
            var ret = this.fnGetLocalStorage('ServerActivationTime', ['startHour', 'startMin', 'endHour', 'endMin', 'durationMin', 'isOn']);
            return ret;
        };
        cDLWebStorage.prototype.setTVTVEPGUpdateTimer = function (param) {
            if (param.hour === null || param.min === null) {
                this.fnRemoveLocalStorage('TVTVEPGUpdateTimer', ['hour', 'min', 'isOn']);
                return;
            }
            this.fnSetLocalStorage('TVTVEPGUpdateTimer', {
                'hour': param.hour,
                'min': param.min,
                'isStandby': param.isStandby
            });
        };
        cDLWebStorage.prototype.getTVTVEPGUpdateTimer = function () {
            var ret = this.fnGetLocalStorage('TVTVEPGUpdateTimer', ['hour', 'min', 'isStandby']);
            return ret;
        };
        cDLWebStorage.prototype.getHelp = function () {
            var a = this.fnGetLocalStorage('help', ['ltv', 'tvg', 'rec', 'tvapps', 'media', 'ondemand', 'home', 'tvg_update1']);
            return {
                'ltv': a.ltv,
                'tvg': a.tvg,
                'rec': a.rec,
                'tvapps': a.tvapps,
                'media': a.media,
                'ondemand': a.ondemand,
                'home': a.home,
                'tvg_update1': a.tvg_update1
            };
        };
        cDLWebStorage.prototype.setHelp = function (p) {
            if (p.ltv) {
                this.fnSetLocalStorage('help', {
                    'ltv': p.ltv
                });
            }
            if (p.tvg) {
                this.fnSetLocalStorage('help', {
                    'tvg': p.tvg
                });
            }
            if (p.rec) {
                this.fnSetLocalStorage('help', {
                    'rec': p.rec
                });
            }
            if (p.tvapps) {
                this.fnSetLocalStorage('help', {
                    'tvapps': p.tvapps
                });
            }
            if (p.media) {
                this.fnSetLocalStorage('help', {
                    'media': p.media
                });
            }
            if (p.ondemand) {
                this.fnSetLocalStorage('help', {
                    'ondemand': p.ondemand
                });
            }
            if (p.home) {
                this.fnSetLocalStorage('help', {
                    'home': p.home
                });
            }
            if (p.tvg_update1) {
                this.fnSetLocalStorage('help', {
                    'tvg_update1': p.tvg_update1
                });
            }
        };
        cDLWebStorage.prototype.getStorageUUID = function () {
            var a = this.fnGetLocalStorage('detachableHdd', ['detachableHdd']);
            hx.logadd('DetachableHDD - MSG');
            hx.log('DetachableHDD - MSG', 'dsrvWebStorage a.detachableHdd : ' + a.detachableHdd);
            return {
                'detachableHdd': a.detachableHdd //true
            };
        };
        cDLWebStorage.prototype.setStorageUUID = function (p) {
            hx.log('DetachableHDD - MSG', 'dsrvWebStorage p.detachableHdd : ' + p.detachableHdd);
            this.fnSetLocalStorage('detachableHdd', {
                'detachableHdd': p.detachableHdd
            });
        };
        cDLWebStorage.prototype.getOtaNoti = function () {
            var a = this.fnGetLocalStorage('otanotification', ['otanoti', 'otacheck']);
            if ((a.otanoti === null || a.otanoti === undefined) && (a.otacheck === null || a.otacheck === undefined)) {
                return {
                    'otanoti': otanotification.otanoti,
                    'otacheck': otanotification.otacheck
                };
            }
            else {
                return {
                    'otanoti': a.otanoti,
                    'otacheck': Number(a.otacheck)
                };
            }
        };
        cDLWebStorage.prototype.setOtaNoti = function (p) {
            if (p.otanoti) {
                this.fnSetLocalStorage('otanotification', {
                    'otanoti': p.otanoti
                });
            }
            else if (p.otacheck) {
                this.fnSetLocalStorage('otanotification', {
                    'otacheck': p.otacheck
                });
            }
        };
        cDLWebStorage.prototype.isPreInstalledFinished = function () {
            var tvAppInfo = getTVAppInfo();
            if (tvAppInfo) {
                return tvAppInfo.preInstalledFinished;
            }
            return false;
        };
        cDLWebStorage.prototype.setPreInstalledFinished = function (param) {
            var tvAppInfo = getTVAppInfo();
            if (tvAppInfo) {
                tvAppInfo.preInstalledFinished = param.preInstalled;
            }
            else {
                tvAppInfo = {
                    preInstalledFinished: param.preInstalled
                };
            }
            setTVAppInfo(tvAppInfo);
        };
        cDLWebStorage.prototype.setVideoOption = function (param) {
            if (param.repeatOption !== undefined) {
                this.fnSetLocalStorage('videoOption', {
                    'repeatOption': param.repeatOption
                });
            }
        };
        cDLWebStorage.prototype.getVideoOption = function () {
            var result = this.fnGetLocalStorage('videoOption', ['repeatOption']);
            if (result.repeatOption === null) {
                this.fnSetLocalStorage('videoOption', {
                    'repeatOption': 1
                });
                result.repeatOption = 1;
            }
            return {
                'repeatOption': result.repeatOption === undefined ? 0 : result.repeatOption
            };
        };
        cDLWebStorage.prototype.setMusicOption = function (param) {
            if (param.repeatOption !== undefined) {
                this.fnSetLocalStorage('musicOption', {
                    'repeatOption': param.repeatOption
                });
            }
            if (param.playMode !== undefined) {
                this.fnSetLocalStorage('musicOption', {
                    'playMode': param.playMode
                });
            }
        };
        cDLWebStorage.prototype.getMusicOption = function () {
            var result = this.fnGetLocalStorage('musicOption', ['repeatOption', 'playMode']);
            if (result.repeatOption === null || result.playMode === null) {
                this.fnSetLocalStorage('musicOption', {
                    'repeatOption': 0,
                    'playMode': 0
                });
                result = this.fnGetLocalStorage('musicOption', ['repeatOption', 'playMode']);
            }
            return {
                'repeatOption': result.repeatOption === undefined ? 0 : result.repeatOption,
                'playMode': result.playMode === undefined ? 0 : result.playMode
            };
        };
        cDLWebStorage.prototype.setPhotoOption = function (param) {
            if (param.slideshowSpeed !== undefined) {
                this.fnSetLocalStorage('photoOption', {
                    'slideshowSpeed': param.slideshowSpeed
                });
            }
            if (param.playMusic !== undefined) {
                this.fnSetLocalStorage('photoOption', {
                    'playMusic': param.playMusic
                });
            }
        };
        cDLWebStorage.prototype.getPhotoOption = function () {
            var result = this.fnGetLocalStorage('photoOption', ['slideshowSpeed', 'playMusic']), i;
            if (result.slideshowSpeed === null || result.playMusic === null) {
                this.fnSetLocalStorage('photoOption', {
                    'slideshowSpeed': 1,
                    'playMusic': 0
                });
                result = this.fnGetLocalStorage('photoOption', ['slideshowSpeed', 'playMusic']);
            }
            return {
                'slideshowSpeed': result.slideshowSpeed === undefined ? 0 : result.slideshowSpeed,
                'playMusic': result.playMusic === undefined ? 0 : result.playMusic
            };
        };
        cDLWebStorage.prototype.getSambaUserInfo = function (param) {
            var serverName = param.uid, id = param.id, refo = localStorage, result = refo.getItem('sambaInfo'), ret = {
                anonymous: false
            };
            if (result) {
                result = JSON.parse(result);
                result = result[serverName];
                if (result) {
                    if (result.anonymous) {
                        ret.anonymous = result.anonymous;
                    }
                    else {
                        result = result[id];
                        if (result) {
                            ret.pw = result;
                        }
                    }
                }
            }
            return ret;
        };
        cDLWebStorage.prototype.setSambaUserInfo = function (param) {
            var serverName = param.uid, id = param.id, pw = param.pw, refo = localStorage, result = refo.getItem('sambaInfo'), account;
            if (result) {
                result = JSON.parse(result);
            }
            else {
                result = {};
            }
            if (!result[serverName]) {
                result[serverName] = {};
            }
            account = result[serverName];
            if (id) {
                account.anonymous = false;
                account[id] = pw || '';
            }
            else {
                account.anonymous = true;
            }
            refo.setItem('sambaInfo', JSON.stringify(result));
        };
        cDLWebStorage.prototype.getStatistics = function () {
            var refo = localStorage;
            var result = refo.getItem('StatisticsLog');
            if (result) {
                result = JSON.parse(result);
            }
            return result;
        };
        cDLWebStorage.prototype.setStatistics = function (param) {
            var refo = localStorage;
            refo.setItem('StatisticsLog', JSON.stringify(param));
        };
        /*********** External API End ****************/
        /*********** Internal Function Start ****************/
        cDLWebStorage.prototype.fnUpdateLastService = function (ccid) {
            // update back service
            this.fnUpdateBackService();
            mZapInfo.channelCcid = ccid;
            this.fnSetLocalStorage('zapInfo', {
                'channelCcid': ccid
            });
        };
        cDLWebStorage.prototype.fnUpdateLastGroup = function (group) {
            mZapInfo.svcListGroup = group;
            this.fnSetLocalStorage('zapInfo', {
                'svcListGroup': group
            });
        };
        cDLWebStorage.prototype.fnUpdateLastDeliveryType = function (delivery) {
            mZapInfo.deliveryType = delivery;
            this.fnSetLocalStorage('zapInfo', {
                'deliveryType': delivery
            });
        };
        // this will provide "BACK" key service
        cDLWebStorage.prototype.fnUpdateBackService = function () {
            if ((mZapBackInfo.channelCcid === mZapInfo.channelCcid) && (mZapBackInfo.svcListGroup === mZapInfo.svcListGroup) && (mZapBackInfo.deliveryType === mZapInfo.deliveryType)) {
                // do not need update
                return;
            }
            mZapBackInfo.channelCcid = mZapInfo.channelCcid;
            mZapBackInfo.svcListGroup = mZapInfo.svcListGroup;
            mZapBackInfo.deliveryType = mZapInfo.deliveryType;
        };
        cDLWebStorage.prototype.fnSetLocalStorage = function (groupName, object) {
            var param;
            for (param in object) {
                localStorage.setItem(groupName + '.' + param, object[param]);
            }
        };
        cDLWebStorage.prototype.fnGetLocalStorage = function (groupName, keyList) {
            var ret = {};
            for (var i = 0, len = keyList.length; i < len; i++) {
                ret[keyList[i]] = localStorage.getItem(groupName + '.' + keyList[i]);
            }
            /*`
            $.each(keyList, function() {
                var key = this;
                ret[key] = localStorage.getItem(groupName + '.' + key);
            });
            */
            return ret;
        };
        cDLWebStorage.prototype.fnRemoveLocalStorage = function (groupName, keyList) {
            var ret = {};
            $.each(keyList, function () {
                var key = this;
                ret[key] = localStorage.removeItem(groupName + '.' + key);
            });
            return ret;
        };
        cDLWebStorage.prototype.fnDeleteWholeLocalStorage = function () {
            hx.log("warning", "delete whole localStorage datas!!!!!!");
            localStorage.clear();
        };
        cDLWebStorage.prototype.fnInitLastTvRadioInfo = function () {
            var a;
            hx.log("dsvc", "hx.svc.WEB_STORAGE's fnInitLastTvRadioInfo() called");
            a = this.fnGetLocalStorage('lastTvRadioInfo', ['channelTvCcid', 'channelRadioCcid']);
            fnLastTvRadioSvcInfo.channelTvCcid = a.channelTvCcid;
            fnLastTvRadioSvcInfo.channelRadioCcid = a.channelRadioCcid;
        };
        cDLWebStorage.prototype.fnDefaultLastTvRadioInfo = function () {
            var a;
            this.fnRemoveLocalStorage('lastTvRadioInfo', ['channelTvCcid', 'channelRadioCcid']);
            a = this.fnGetLocalStorage('lastTvRadioInfo', ['channelTvCcid', 'channelRadioCcid']);
            fnLastTvRadioSvcInfo.channelTvCcid = a.channelTvCcid;
            fnLastTvRadioSvcInfo.channelRadioCcid = a.channelRadioCcid;
        };
        cDLWebStorage.prototype.fnUpdateLastTvRadioService = function (ccid, group) {
            var svcInfo = hx.svc.CH_LIST.getChannel({
                ccid: ccid,
                group: group
            });
            if (svcInfo && svcInfo.channel) {
                if (svcInfo.channel.channelType === 0) {
                    fnLastTvRadioSvcInfo.channelTvCcid = ccid;
                    this.fnSetLocalStorage('lastTvRadioInfo', {
                        'channelTvCcid': ccid
                    });
                }
                else if (svcInfo.channel.channelType === 1) {
                    fnLastTvRadioSvcInfo.channelRadioCcid = ccid;
                    this.fnSetLocalStorage('lastTvRadioInfo', {
                        'channelRadioCcid': ccid
                    });
                }
            }
        };
        cDLWebStorage.prototype.setFileSharePasswd = function (param) {
            if (param === null) {
                this.fnRemoveLocalStorage('FileSharePasswd', ['passwd']);
                return;
            }
            this.fnSetLocalStorage('FileSharePasswd', {
                'passwd': param
            });
        };
        cDLWebStorage.prototype.getFileSharePasswd = function () {
            var ret = this.fnGetLocalStorage('FileSharePasswd', ['passwd']);
            return ret;
        };
        cDLWebStorage.prototype.getPrivacyPolicy = function () {
            var ret = this.fnGetLocalStorage('PrivacyPolicy', ['state', 'text', 'version']);
            return ret;
        };
        cDLWebStorage.prototype.setPrivacyPolicyVersion = function (aVersion) {
            if (aVersion === null) {
                this.fnRemoveLocalStorage('PrivacyPolicy', ['version']);
                return;
            }
            this.fnSetLocalStorage('PrivacyPolicy', {
                'version': aVersion
            });
        };
        cDLWebStorage.prototype.setPrivacyPolicyState = function (aState) {
            if (aState === null) {
                this.fnRemoveLocalStorage('PrivacyPolicy', ['state']);
                return;
            }
            this.fnSetLocalStorage('PrivacyPolicy', {
                'state': aState
            });
        };
        cDLWebStorage.prototype.setPrivacyPolicyText = function (aText) {
            if (aText === null) {
                this.fnRemoveLocalStorage('PrivacyPolicy', ['text']);
                return;
            }
            this.fnSetLocalStorage('PrivacyPolicy', {
                'text': aText
            });
        };
        cDLWebStorage.prototype.getTermsAndConditions = function () {
            var ret = this.fnGetLocalStorage('TermsAndConditions', ['state', 'text', 'version']);
            return ret;
        };
        cDLWebStorage.prototype.setTermsAndConditionsVersion = function (aVersion) {
            if (aVersion === null) {
                this.fnRemoveLocalStorage('TermsAndConditions', ['version']);
                return;
            }
            this.fnSetLocalStorage('TermsAndConditions', {
                'version': aVersion
            });
        };
        cDLWebStorage.prototype.setTermsAndConditionsState = function (aState) {
            if (aState === null) {
                this.fnRemoveLocalStorage('TermsAndConditions', ['state']);
                return;
            }
            this.fnSetLocalStorage('TermsAndConditions', {
                'state': aState
            });
        };
        cDLWebStorage.prototype.setTermsAndConditionsText = function (aText) {
            if (aText === null) {
                this.fnRemoveLocalStorage('TermsAndConditions', ['text']);
                return;
            }
            this.fnSetLocalStorage('TermsAndConditions', {
                'text': aText
            });
        };
        cDLWebStorage.prototype.readTerms = function () {
            this.fnSetLocalStorage('NeedTerms', {
                'state': false
            });
        };
        cDLWebStorage.prototype.isNeedTerms = function () {
            var ret = this.fnGetLocalStorage('NeedTerms', ['state']);
            if (ret.state === 'false') {
                return false;
            }
            return true;
        };
        cDLWebStorage.prototype.setLastMediaPageName = function (param) {
            this.fnSetLocalStorage('LastMediaPageType', {
                'name': param
            });
        };
        cDLWebStorage.prototype.getLastMediaPageName = function (param) {
            var obj = this.fnGetLocalStorage('LastMediaPageType', ['name']);
            return obj ? obj.name : '';
        };
        cDLWebStorage.prototype.setUserAppList = function (param) {
            var that = hx.dl.dsrvWebStorage;
            var tvPortal = hx.dl.dsrvTVPortal;
            var item = JSON.stringify(param);
            that.fnSetLocalStorage('userAppInfo', {
                'appList': item
            });
        };
        cDLWebStorage.prototype.clearUserAppList = function () {
            var that = this;
            that.fnRemoveLocalStorage('userAppInfo', ['appList']);
        };
        cDLWebStorage.prototype.getUserAppList = function () {
            var that = this, ret = that.fnGetLocalStorage('userAppInfo', ['appList']);
            var list = undefined;
            if (ret.appList) {
                list = JSON.parse(ret.appList);
            }
            if (list === 'null') {
                list = undefined;
            }
            return list;
        };
        cDLWebStorage.prototype.setHideAppList = function (param) {
            var that = this;
            var item = JSON.stringify(param);
            that.fnSetLocalStorage('hideAppInfo', {
                'appList': item
            });
        };
        cDLWebStorage.prototype.clearHideAppList = function () {
            var that = this;
            that.fnRemoveLocalStorage('hideAppInfo', ['appList']);
        };
        cDLWebStorage.prototype.setStatisticsIpEpgDownloadStartEnable = function (param) {
            if (param === null) {
                this.fnRemoveLocalStorage('DownloadStartEnable', ['downloadenable']);
                return;
            }
            this.fnSetLocalStorage('DownloadStartEnable', {
                'downloadenable': param
            });
        };
        cDLWebStorage.prototype.getStatisticsIpEpgDownloadStartEnable = function () {
            var ret = this.fnGetLocalStorage('DownloadStartEnable', ['downloadenable']);
            if (ret.downloadenable === 'true') {
                ret.downloadenable = true;
            }
            else {
                ret.downloadenable = false;
            }
            return ret.downloadenable;
        };
        cDLWebStorage.prototype.setStatisticsLastUpdateIpEpgTime = function (param) {
            if (param === null) {
                this.fnRemoveLocalStorage('LastUpdateIpEpgTime', ['updatetime']);
                return;
            }
            this.fnSetLocalStorage('LastUpdateIpEpgTime', {
                'updatetime': param
            });
        };
        cDLWebStorage.prototype.getStatisticsLastUpdateIpEpgTime = function () {
            var ret = this.fnGetLocalStorage('LastUpdateIpEpgTime', ['updatetime']);
            if (ret.updatetime) {
                ret.updatetime = parseInt(ret.updatetime, 10);
            }
            return ret.updatetime;
        };
        cDLWebStorage.prototype.setStatisticsLastSendingTimeOfSettingInfo = function (param) {
            if (param === null) {
                this.fnRemoveLocalStorage('LastSendingTimeOfSettingInfo', ['sendsettingtime']);
                return;
            }
            this.fnSetLocalStorage('LastSendingTimeOfSettingInfo', {
                'sendsettingtime': param
            });
        };
        cDLWebStorage.prototype.getStatisticsLastSendingTimeOfSettingInfo = function () {
            var ret = this.fnGetLocalStorage('LastSendingTimeOfSettingInfo', ['sendsettingtime']);
            return ret.sendsettingtime;
        };
        cDLWebStorage.prototype.setStatisticsLastSendingTimeOfCheckUpload = function (param) {
            if (param === null) {
                this.fnRemoveLocalStorage('LastSendingTimeOfCheckUpload', ['sendcheckuploadtime']);
                return;
            }
            this.fnSetLocalStorage('LastSendingTimeOfCheckUpload', {
                'sendcheckuploadtime': param
            });
        };
        cDLWebStorage.prototype.getStatisticsLastSendingTimeOfCheckUpload = function () {
            var ret = this.fnGetLocalStorage('LastSendingTimeOfCheckUpload', ['sendcheckuploadtime']);
            return ret.sendcheckuploadtime;
        };
        cDLWebStorage.prototype.setItem = function (aKey, aData) {
            localStorage.setItem(aKey, aData);
        };
        cDLWebStorage.prototype.getItem = function (aKey) {
            return localStorage.getItem(aKey);
        };
        cDLWebStorage.prototype.getHideAppList = function (param) {
            var that = this, ret = that.fnGetLocalStorage('hideAppInfo', ['appList']);
            var list = JSON.parse(ret.appList);
            return list;
        };
        cDLWebStorage.prototype.getSortTypeInRecordings = function () {
            var ret = this.fnGetLocalStorage('SortType', ['Recordings']);
            return Number(ret.Recordings);
        };
        cDLWebStorage.prototype.setSortTypeInRecordings = function (aParam) {
            if (aParam === null) {
                this.fnRemoveLocalStorage('SortType', ['Recordings']);
                return;
            }
            this.fnSetLocalStorage('SortType', {
                'Recordings': aParam
            });
        };
        cDLWebStorage.prototype.getSearchHistory = function () {
            var ret = this.fnGetLocalStorage('History', ['search']);
            return JSON.parse(ret.search);
        };
        cDLWebStorage.prototype.setSearchHistory = function (aHistoryList) {
            if (!aHistoryList) {
                this.fnRemoveLocalStorage('History', ['search']);
                return;
            }
            this.fnSetLocalStorage('History', {
                'search': JSON.stringify(aHistoryList)
            });
        };
        cDLWebStorage.prototype.getBackgroundTransparency = function () {
            var ret = this.fnGetLocalStorage('Settings', ['transparency']);
            return Number(ret.transparency || 0.8);
        };
        cDLWebStorage.prototype.setBackgroundTransparency = function (aOpacity) {
            if (!aOpacity) {
                this.fnRemoveLocalStorage('Settings', ['transparency']);
                return;
            }
            this.fnSetLocalStorage('Settings', {
                'transparency': aOpacity
            });
        };
        cDLWebStorage.prototype.getAlexaDeviceEnable = function () {
            if (!hx.config.voiceAssistant) {
                return false;
            }
            var ret = this.fnGetLocalStorage('AlexaDevice', ['enable']);
            if (ret.enable === 'true') {
                return true;
            }
            else {
                return false;
            }
        };
        cDLWebStorage.prototype.setAlexaDeviceEnable = function (enable) {
            if (!enable) {
                this.fnRemoveLocalStorage('AlexaDevice', ['enable']);
                return;
            }
            this.fnSetLocalStorage('AlexaDevice', {
                'enable': enable
            });
        };
        cDLWebStorage.eventHandlers = [];
        return cDLWebStorage;
    })(__KERNEL__);
    return cDLWebStorage;
});
