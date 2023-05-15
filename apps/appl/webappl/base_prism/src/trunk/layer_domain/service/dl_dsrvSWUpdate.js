/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvSWUpdate.js
//    SW_UPDATE: ['layer_domain/service/dl_dsrvSWUpdate.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var cDLSW_UPDATE = (function (_super) {
        __extends(cDLSW_UPDATE, _super);
        function cDLSW_UPDATE(aServiceName) {
            _super.call(this, aServiceName);
            this.mLocalSystemOipfObj = this.getPluginInstance('LocalSystem');
            this.mOipfSwUpdateObj = this.mLocalSystemOipfObj.swupdate;
            this.bSwUpdateDownloadStart = false;
            this.bSwUpdateDownloadSuccess = false;
            this.TSWUSpec = {
                ESPEC_NONE: 0,
                ESPEC_ASTRA: 1,
                ESPEC_SATDVBSSU: 2,
                ESPEC_DTT: 3,
                ESPEC_SKYD: 4,
                ESPEC_APS: 5,
                ESPEC_IRDETO: 6,
                ESPEC_HUMAX: 7,
                ESPEC_FSATSSU: 8,
                ESPEC_UPC: 9,
                ESPEC_KDG: 10
            };
            this.TSWUEvent = {
                EEventConnectSuccess: 0,
                EEventConnectFail: 1,
                EEventDetectSuccess: 2,
                EEventDetectSuccessInLive: 3,
                EEventDetectFail: 4,
                EEventDownloadSuccess: 5,
                EEventDownloadFail: 6,
                EEventDownloadProgress: 7,
                EEventInstallSuccess: 8,
                EEventInstallFail: 9,
                EEventInstallProgress: 10,
                EEventErorr: 11,
                EEventFinish: 12
            };
            this.TSWUStep = {
                EStepUnknown: 0,
                EStepConfig: 1,
                EStepConnect: 2,
                EStepDetect: 3,
                EStepDownload: 4,
                EStepInstall: 5
            };
            this.TSWUSource = {
                ESourceNone: 0,
                ESourceRf: 1,
                ESourceIp: 2,
                ESourceFile: 3,
                ESourceUsb: 4
            };
            this.TSatCodeRate = {
                EAuto: 0,
                ENone: 1,
                E_1_2: 2,
                E_2_3: 3,
                E_3_4: 4,
                E_5_6: 5,
                E_7_8: 6,
                E_3_5: 7,
                E_4_5: 8,
                E_5_11: 9,
                E_6_7: 10,
                E_8_9: 11,
                E_9_10: 12
            };
            this.TSatPolarization = {
                EAuto: 0,
                EHorizontal: 1,
                EVertical: 2,
                ELeft: 3,
                ERight: 4
            };
            this.TSWUType = {
                EUSERSELECT: 0,
                EFORCED: 1,
                ECONDITIONAL: 2,
                EBURSTUPDATE: 3
            };
            this.eventHandlers = [];
            hx.log("dsvc", "initializer service - hx.svc.SW_UPDATE");
            // Event handler.
            this.eventHandlers = [];
            this.init();
        }
        /*********** Internal Function Start ****************/
        cDLSW_UPDATE.prototype.fnOnUpdateEvent = function (aSwUpdateEventInfo) {
            var that = this, enumSWUEvent, eventType, source, pro, proMax, curver, foundver;
            enumSWUEvent = that.TSWUEvent;
            eventType = aSwUpdateEventInfo.getInfo('event');
            source = aSwUpdateEventInfo.getInfo('source_type');
            if (eventType === enumSWUEvent.EEventDetectSuccess) {
                curver = aSwUpdateEventInfo.getInfo('current_version');
                foundver = aSwUpdateEventInfo.getInfo('found_version');
                that.bSwUpdateDownloadStart = false;
                if (source === that.TSWUSource.ESourceUsb) {
                    if (hx.svc.STATISTICS) {
                        hx.svc.STATISTICS.statisticsLog({
                            "CODE": "CKOTA",
                            "TRS": "I",
                            "TYPE": "USB",
                            "DEV_VER": curver,
                            "NEW_VER": ""
                        });
                    }
                }
                else {
                    if (hx.svc.STATISTICS) {
                        hx.svc.STATISTICS.statisticsLog({
                            "CODE": "CKOTA",
                            "TRS": "I",
                            "TYPE": source === that.TSWUSource.ESourceIp ? "IP" : "RF",
                            "DEV_VER": curver,
                            "NEW_VER": foundver
                        });
                    }
                }
            }
            else if (eventType === enumSWUEvent.EEventDownloadProgress) {
                pro = aSwUpdateEventInfo.getInfo('progress');
                proMax = aSwUpdateEventInfo.getInfo('progress_max');
                if (!that.bSwUpdateDownloadStart) {
                    that.bSwUpdateDownloadStart = true;
                    if (hx.svc.STATISTICS) {
                        hx.svc.STATISTICS.statisticsLog({
                            "CODE": "DLS",
                            "TRS": "I"
                        });
                    }
                }
            }
            else if (eventType === enumSWUEvent.EEventDownloadSuccess) {
                that.bSwUpdateDownloadSuccess = true;
            }
            else if (eventType === enumSWUEvent.EEventFinish) {
                that.bSwUpdateDownloadStart = false;
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "DLE",
                        "TRS": "I",
                        "SUCCESS": that.bSwUpdateDownloadSuccess
                    });
                    hx.svc.STATISTICS.transmitLog();
                }
                that.bSwUpdateDownloadSuccess = false;
            }
            this.fireEvent_Legacy('UpdateEvent', {
                'SwUpdateEventInfo': aSwUpdateEventInfo
            });
        };
        cDLSW_UPDATE.prototype.fnOnDetectEvent = function (aSource, aState, aCurver, aFoundver, aErrMsg) {
            this.fireEvent_Legacy('DetectEvent', {
                'source': aSource,
                "state": aState,
                "curver": aCurver,
                "foundver": aFoundver,
                "errMsg": aErrMsg
            });
        };
        cDLSW_UPDATE.prototype.fnOnUpdateProgress = function (aSource, aType, aProgress, aProgressMax, aErrMsg) {
            this.fireEvent_Legacy('UpdateProgress', {
                'source': aSource,
                "type": aType,
                "progress": aProgress,
                "progressMax": aProgressMax,
                "errMsg": aErrMsg
            });
        };
        cDLSW_UPDATE.prototype.init = function () {
            var _this = this;
            hx.log("dsvc", "hx.svc.SW_UPDATE's init() called");
            //       hx.svc.SW_UPDATE._initEventHandler();
            this.mOipfSwUpdateObj.onUpdateEvent = function (SwUpdateEventInfo) {
                _this.fnOnUpdateEvent(SwUpdateEventInfo);
            };
            this.mOipfSwUpdateObj.onDetectEvent = function (source, state, curver, foundver, errMsg) {
                hx.log("warning", hx.util.format("+++++ ### {0}.onDetectEvent ### +++++, source : {1}, state : {2}, curver : {3}, foundver : {4}, errMsg : {5}", 'dsrvSWUpdate', source, state, curver, foundver, errMsg));
                _this.fnOnDetectEvent(source, state, curver, foundver, errMsg);
            };
            this.mOipfSwUpdateObj.onUpdateProgress = function (source, type, progress, progressMax, errMsg) {
                hx.log("warning", hx.util.format("+++++ ### {0}.onUpdateProgress ### +++++, source : {1}, type : {2}, progress : {3}, progressMax : {4}, errMsg : {5}", 'dsrvSWUpdate', source, type, progress, progressMax, errMsg));
                _this.fnOnUpdateProgress(source, type, progress, progressMax, errMsg);
            };
        };
        /*********** Internal Function End ****************/
        /*********** External API Start ****************/
        cDLSW_UPDATE.prototype.getVerBootupHumaxOta = function () {
            var handle = this.mOipfSwUpdateObj;
            if (!handle) {
                hx.log("warning", hx.util.format("+++++ ### {0}.restartSystem ### +++++, handle is null", 'dsrvSWUpdate'));
                return;
            }
            if (!handle.verBootupHumaxOta) {
                hx.log("warning", hx.util.format("+++++ ### {0}.restartSystem ### +++++, handle.verBootupHumaxOta is null", 'dsrvSWUpdate'));
                return;
            }
            return handle.verBootupHumaxOta;
        };
        cDLSW_UPDATE.prototype.swupdateInit = function () {
            this.init();
            this.mOipfSwUpdateObj.swupdateInit();
        };
        cDLSW_UPDATE.prototype.startDetectWithType = function (aParam) {
            this.init();
            this.mOipfSwUpdateObj.startDetectWithType(aParam.otaType);
        };
        cDLSW_UPDATE.prototype.startDetect = function () {
            this.init();
            this.mOipfSwUpdateObj.startDetect();
        };
        cDLSW_UPDATE.prototype.startManualDetect = function (aInfo) {
            this.init();
            this.mOipfSwUpdateObj.startManualDetect(aInfo.frequency, aInfo.polarisation, aInfo.symbolRate, 0, 0, aInfo.FEC);
        };
        cDLSW_UPDATE.prototype.cancelDetect = function () {
            var handle = this.mOipfSwUpdateObj;
            if (!handle) {
                hx.log("warning", hx.util.format("+++++ ### {0}.cancelDetect ### +++++, handle is null", 'dsrvSWUpdate'));
                return;
            }
            handle.cancelDetect();
            handle = null;
        };
        cDLSW_UPDATE.prototype.startDownload = function (aParam) {
            var handle = this.mOipfSwUpdateObj;
            if (!handle) {
                hx.log("warning", hx.util.format("+++++ ### {0}.startDownload ### +++++, handle is null", 'dsrvSWUpdate'));
                return;
            }
            handle.startDownload(aParam.otaSpec);
        };
        cDLSW_UPDATE.prototype.startSwupdateState = function (aParam) {
            var handle = this.mOipfSwUpdateObj;
            if (!handle) {
                hx.log("warning", hx.util.format("+++++ ### {0}.startSwupdateState ### +++++, handle is null", 'dsrvSWUpdate'));
                return;
            }
            handle.startSwupdateState(aParam.otaSpec, aParam.otaStep);
        };
        cDLSW_UPDATE.prototype.cancelDownload = function () {
            var handle = this.mOipfSwUpdateObj;
            if (!handle) {
                hx.log("warning", hx.util.format("+++++ ### {0}.cancelDownload ### +++++, handle is null", 'dsrvSWUpdate'));
                return;
            }
            handle.cancelDownload();
            handle = null;
        };
        cDLSW_UPDATE.prototype.restartSystem = function () {
            var handle = this.mOipfSwUpdateObj;
            if (!handle) {
                hx.log("warning", hx.util.format("+++++ ### {0}.restartSystem ### +++++, handle is null", 'dsrvSWUpdate'));
                return;
            }
            hx.log("warning", "+++++ ### restartSystem ### +++++");
            handle.restartSystem();
        };
        return cDLSW_UPDATE;
    })(__KERNEL__);
    return cDLSW_UPDATE;
});
