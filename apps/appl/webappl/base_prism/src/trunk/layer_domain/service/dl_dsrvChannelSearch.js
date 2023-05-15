/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvChannelSearch.js
/*jslint sloppy:true, nomen:true, vars:true */
//    CHANNEL_SEARCH: ['layer_domain/service/dl_dsrvChannelSearch.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var cDLCHANNEL_SEARCH = (function (_super) {
        __extends(cDLCHANNEL_SEARCH, _super);
        function cDLCHANNEL_SEARCH(aServiceName) {
            _super.call(this, aServiceName);
            this.lcnScanType = {
                UPDATE: 0,
                CHANGE: 1
            };
            ////////////// Defined Data ///////////////////
            this._MEdefaultAntName = 'HOTBIRD';
            this._defaultAntName = 'ASTRA_1';
            this._dfaultEmptyAntName = 'NONE';
            this.INDEX_MOTOR_USERDEFINE = 29;
            this.MAX_NUM_MOTOR_SATELLITE = 63;
            ////////////////////// srv cashed Data /////////////////////
            this._transponderList = {};
            this._satPosDataList = {};
            this._searchOptions = {
                network: 'true',
                scramble: 'All',
                channel: 'All'
            };
            this._curUsingSatelliteIdx = 0;
            this._curSettingTpIdx = 0;
            this._antlength = 0;
            this._isStartScan = false;
            this.curLcnScanType = 0;
            //scd tuner param
            this._lastSettingTuner = 1;
            this._tuner1Param = {
                scdBandNo: 1,
                scdBandFreq: 1210
            };
            this._tuner2Param = {
                scdBandNo: 2,
                scdBandFreq: 1400
            };
            this._tuner3Param = {
                scdBandNo: 3,
                scdBandFreq: 1516
            };
            this._tuner4Param = {
                scdBandNo: 4,
                scdBandFreq: 1632
            };
            //motor param
            this._motorSetJson = {
                motorType: 'DiSEqC 1.2',
                longitude: '0.0',
                ewDirection: 'East',
                latitude: '0.0',
                nsDirection: 'North'
            };
            this._motorDriving = 'Continuous'; //Continuous; Step; Time
            this._drivingLevel = 0;
            this._motorLimit = 'Off';
            this._limitDirection = 'East';
            this._activationList = [];
            this.defineEvents(['channelScan', 'diseqcScanResult', 'antennaConnection']);
            this.oipfChannelConfigObj = this.getPluginInstance('ChannelConfig');
            this.oipfLocalSystemObj = this.getPluginInstance('LocalSystem');
            if (hx.config.useLcnUpdate) {
                this.oipfHLcnUpdate = this.oipfLocalSystemObj.lcnupdate;
            }
            this._initializeAntData();
            this._loadTpData();
            this._loadSatPosData();
            this._getSearchOptionsFromLocal();
            this._getTpIdxListFromLocal();
            this._getTpParamFromLocal();
            this._remakeNotPreDefinedMotorAntennaPos();
            this.getAntIdListFromLocal();
            this.antennaSetting();
            //hx.logadd('dsrvChannelSearch');
            hx.log('dsrvChannelSearch', 'create');
        }
        // ####################################################################################################
        // Public Methods
        //////////////////////////*  Get & Set SearchOption START *//////////////////////////
        cDLCHANNEL_SEARCH.prototype.setSearchOptionParam = function (param) {
            this._searchOptions.network = param.network;
            this._searchOptions.scramble = param.scramble;
            this._searchOptions.channel = param.channel;
            this._saveSearchOptionsToLocal();
        };
        cDLCHANNEL_SEARCH.prototype.getSearchOptionParam = function () {
            return {
                network: this._searchOptions.network,
                scramble: this._searchOptions.scramble,
                channel: this._searchOptions.channel
            };
        };
        //////////////////////////*  Get & Set SearchOption END *//////////////////////////
        /////////////////////////*   Channel Scan Control START *//////////////////////////////////////
        cDLCHANNEL_SEARCH.prototype.StartChannelScan = function () {
            var _this = this;
            var fn = function (type, progress, frequency, signalStrength, channelNumber, channelType, channelCount, transponderCount, owSearchedInfos, tpInfo) {
                _this.fireEvent('channelScan', {
                    progress: progress,
                    channelNumber: channelNumber,
                    channelCount: channelCount,
                    transponderCount: transponderCount,
                    signalStrength: signalStrength,
                    tpInfo: tpInfo,
                    owSearchedInfos: owSearchedInfos
                });
            };
            var _antId = [parseInt(this._searchAntennaId[0], 10), parseInt(this._searchAntennaId[1], 10), parseInt(this._searchAntennaId[2], 10), parseInt(this._searchAntennaId[3], 10), parseInt(this._searchAntennaId[4], 10), parseInt(this._searchAntennaId[5], 10), parseInt(this._searchAntennaId[6], 10), parseInt(this._searchAntennaId[7], 10)];
            this._startScan({
                satName: this._searchSatelliteName,
                antId: _antId,
                tsIndex: this._tsIndex,
                searchOptions: this._searchOptions,
                fn: fn,
                usrParam: this._userTPParam,
                isStartScan: this._isStartScan
            });
        };
        cDLCHANNEL_SEARCH.prototype.startDiseqcScan = function () {
            var _this = this;
            var callBack = function (type, progress, frequency, signalStrength, channelNumber, channelType, channelCount, transponderCount /*, owSearchedInfos, tpInfo*/) {
                hx.logadd('DiseqcScan');
                hx.log('DiseqcScan', 'type : ' + type);
                hx.log('DiseqcScan', 'progress : ' + progress);
                hx.log('DiseqcScan', 'channelCount : ' + channelCount);
                if (progress === 100) {
                    var scanData = _this._disecScanCompleted();
                    if (channelCount === 1) {
                        hx.log('DiseqcScan', 'scanData - true ');
                        if (ENV.op === ENV.listOP.MIDDLE_EAST) {
                            var noWork;
                        }
                        else {
                            _this._save_disecScanCompletedAntenna(scanData);
                        }
                        _this.fireEvent('diseqcScanResult', {
                            'scanSuccess': true,
                            'scanData': scanData
                        });
                    }
                    else {
                        hx.log('DiseqcScan', 'scanData - false ');
                        _this.fireEvent('diseqcScanResult', {
                            'scanSuccess': false,
                            'scanData': scanData
                        });
                    }
                }
            };
            hx.log('dsrvChannelSearch', 'startDiseqcScan() called!!!');
            var scanParamCollection = this._createChannelScanParamCollectionObject();
            var scanAntName = '';
            function _set_channel_param(sr, freq, coderate, polar, modul, trans) {
                var channelScanParams = this.oipfChannelConfigObj.createChannelScanParametersObject(11);
                hx.log('dsrvChannelSearch', 'scanParamCollection() called!!!');
                channelScanParams.symbolRate = sr;
                channelScanParams.startFrequency = freq;
                channelScanParams.endFrequency = freq;
                if (ENV.op === ENV.listOP.MIDDLE_EAST) {
                    channelScanParams.antennaId = this.getAntennaIdByName(this._MEdefaultAntName);
                    channelScanParams.antennaName = this._MEdefaultAntName;
                }
                else {
                    scanAntName = this._searchSatelliteName[this._curUsingSatelliteIdx];
                    channelScanParams.antennaId = this.getAntennaIdByName(scanAntName);
                    channelScanParams.antennaName = scanAntName;
                }
                channelScanParams.codeRate = coderate;
                channelScanParams.polarisation = polar;
                channelScanParams.modulationModes = modul;
                if (trans === 0) {
                    channelScanParams.modulationModes = 0;
                }
                else {
                    channelScanParams.modulationModes = modul;
                }
                scanParamCollection.addChannelScanParam(channelScanParams);
            }
            var data;
            if (ENV.listModel.IR4000HD === ENV.model) {
                var szAntenaName = '', antenaInfo;
                szAntenaName = this._MEdefaultAntName;
                data = this._getTuningInfo(szAntenaName, 1);
            }
            else {
                var curAntName = '';
                curAntName = this._searchSatelliteName[this._curUsingSatelliteIdx];
                data = this._getTuningInfo(curAntName, 1);
            }
            var i, length = data.length;
            for (i = 0; i < length; i += 1) {
                _set_channel_param(data[i][1].toString(), data[i][0], data[i][3], data[i][2], data[i][4], data[i][5]);
            }
            this.oipfChannelConfigObj.onChannelScan = callBack;
            this.oipfChannelConfigObj.startScan(null, scanParamCollection, 'diseqcdetection');
            this._isStartScan = true;
        };
        cDLCHANNEL_SEARCH.prototype.getstartDiseqcScanAntennaListCount = function () {
            var antInfolist, len = 0;
            antInfolist = this._getAllAntennaInfo();
            len = antInfolist.length;
            return len;
        };
        cDLCHANNEL_SEARCH.prototype.getstartDiseqcScanAntennaInfo = function (data) {
            var index = data.index, antInfolist, antInfo, convAntInfo;
            antInfolist = this._getAllAntennaInfo();
            if (antInfolist.length > 0) {
                antInfo = antInfolist.item(index);
                convAntInfo = this._convertAntennaData(antInfo);
                return convAntInfo;
            }
            return null;
        };
        cDLCHANNEL_SEARCH.prototype.startAntennaConnection = function (data) {
            var _this = this;
            var i;
            hx.logadd('antCon');
            for (i = 0; i < data.lockingDataList.length; i += 1) {
                hx.log('antCon', 'lockingTPList - Index : ' + i + ' - antId : ' + data.lockingDataList[i].antId);
                hx.log('antCon', 'lockingTPList - Index : ' + i + ' - tpData : ' + data.lockingDataList[i].tpData[0] + ', ' + data.lockingDataList[i].tpData[1] + ', ' + data.lockingDataList[i].tpData[2] + ', ' + data.lockingDataList[i].tpData[3] + ', ' + data.lockingDataList[i].tpData[4] + ', ' + data.lockingDataList[i].tpData[5] + ', ' + data.lockingDataList[i].tpData[6]);
            }
            var fn = function (type, progress, frequency, signalStrength, channelNumber, channelType, channelCount, transponderCount, owSearchedInfos, tpInfo) {
                _this.fireEvent('antennaConnection', {
                    type: type,
                    progress: progress,
                    connectionType: channelCount
                });
            };
            var _antId = [parseInt(this._searchAntennaId[0], 10), parseInt(this._searchAntennaId[1], 10), parseInt(this._searchAntennaId[2], 10), parseInt(this._searchAntennaId[3], 10), parseInt(this._searchAntennaId[4], 10), parseInt(this._searchAntennaId[5], 10), parseInt(this._searchAntennaId[6], 10), parseInt(this._searchAntennaId[7], 10)];
            this._startAntennaConnection({
                satName: this._searchSatelliteName,
                antId: _antId,
                fn: fn,
                lockingDataList: data.lockingDataList,
                isStartScan: this._isStartScan
            });
        };
        cDLCHANNEL_SEARCH.prototype.antennaConnectionComplete = function (data) {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            this.oipfChannelConfigObj.stopScan();
            this._isStartScan = false;
            antInfoLists.setConnectionType(data.connectionType);
        };
        cDLCHANNEL_SEARCH.prototype.StartTune = function (data) {
            var tsIndexNum = 0, tpParam = {};
            if (this._TestSatelliteName === 'NONE') {
                return;
            }
            if (data) {
                if (data.isTpSetPage) {
                    tsIndexNum = 0;
                    tpParam = this._userTempTPParam;
                }
                else {
                    tsIndexNum = this._testtsIndex;
                    tpParam = this._userTestTPParam;
                }
            }
            else {
                tsIndexNum = this._testtsIndex;
                tpParam = this._userTestTPParam;
            }
            this._startTune({
                satName: this._TestSatelliteName,
                antId: parseInt(this._testAntennaId, 10),
                tsIndex: tsIndexNum,
                usrParam: tpParam,
                isStartScan: this._isStartScan
            });
        };
        cDLCHANNEL_SEARCH.prototype.StartTuneWithNotTestAntenna = function (data) {
            var satelliteName = '', antId = 0, tsIndexNum = 0, tpParam = {};
            satelliteName = this._searchSatelliteName[this._curSettingTpIdx];
            antId = this._searchAntennaId[this._curSettingTpIdx];
            if (satelliteName === 'NONE') {
                return;
            }
            if (data) {
                if (data.isTpSetPage) {
                    tsIndexNum = 0;
                    tpParam = this._userTempTPParam;
                }
                else {
                    tsIndexNum = this._testtsIndex;
                    tpParam = this._userTestTPParam;
                }
            }
            else {
                tsIndexNum = this._testtsIndex;
                tpParam = this._userTestTPParam;
            }
            this._startTune({
                satName: satelliteName,
                antId: parseInt(antId + "", 10),
                tsIndex: tsIndexNum,
                usrParam: tpParam,
                isStartScan: this._isStartScan
            });
        };
        cDLCHANNEL_SEARCH.prototype.loadChannelSearchResult = function () {
            this.oipfChannelConfigObj.controlScan('load');
        };
        cDLCHANNEL_SEARCH.prototype.loadChannelSearchResultWithDefCh = function () {
            this.oipfChannelConfigObj.controlScan('loadDefChannels');
            this.oipfChannelConfigObj.controlScan('load');
        };
        cDLCHANNEL_SEARCH.prototype.saveChannelSearchResult = function () {
            this.oipfChannelConfigObj.controlScan('save');
            this.oipfChannelConfigObj.stopScan();
            this._isStartScan = false;
        };
        cDLCHANNEL_SEARCH.prototype.stopScan = function () {
            this.oipfChannelConfigObj.stopScan();
            this._isStartScan = false;
        };
        cDLCHANNEL_SEARCH.prototype.pauseScan = function () {
            this.oipfChannelConfigObj.controlScan('pause');
        };
        cDLCHANNEL_SEARCH.prototype.resumeScan = function () {
            this.oipfChannelConfigObj.controlScan('resume');
        };
        cDLCHANNEL_SEARCH.prototype.updateLcnChannelList = function () {
            hx.logadd('updateLcnChannelList');
            this.curLcnScanType = this.lcnScanType.UPDATE;
            this._updateChannelList();
        };
        cDLCHANNEL_SEARCH.prototype.changeLcnChannelList = function () {
            hx.logadd('changeLcnChannelList');
            this.curLcnScanType = this.lcnScanType.CHANGE;
            this._changeChannelList();
        };
        cDLCHANNEL_SEARCH.prototype.stopLcnUpdate = function () {
            hx.logadd('stopLcnUpdate');
            hx.log('stopLcnUpdate', 'call oipfHLcnUpdate.stopLcnUpdate()');
            this.oipfHLcnUpdate.stopLcnUpdate();
        };
        cDLCHANNEL_SEARCH.prototype.saveLcnUpdate = function (param) {
            hx.logadd('saveLcnUpdate');
            this._saveLcnUpdate({
                searchMode: param.searchMode,
                index: param.index
            });
        };
        cDLCHANNEL_SEARCH.prototype.getCurrentLcnListInfo = function () {
            this._getLcnUpdateInfo();
        };
        cDLCHANNEL_SEARCH.prototype.removeCurrentLcnListInfo = function () {
            this._removeLcnUpdateInfo();
        };
        /////////////////////////*   Channel Scan Control END *//////////////////////////////////////
        ////////////////////////*  Manage Satellite START  */////////////////////////////////////////
        cDLCHANNEL_SEARCH.prototype.getSearchTypeFromAnt = function () {
            if ((this._searchAntennaId[0] === 0) || (this._searchAntennaId[0] === null)) {
                return 0;
            }
            var antInfo = this._getAntennaInfoFromId(this._searchAntennaId[0]);
            if ((this.oipfLocalSystemObj.antennaInfoLists.length === 0) || (antInfo.satelliteName === 'NONE')) {
                return 0;
            }
            if (antInfo.antennaType === 1 && antInfo.switchInput === antInfo.SW_N) {
                return 3;
            }
            return antInfo.antennaType;
        };
        cDLCHANNEL_SEARCH.prototype.getAntIdListFromLocal = function () {
            var i;
            var antIdList;
            antIdList = hx.svc.WEB_STORAGE.getAntIdListFromLocal();
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                var antId = antIdList['antId' + (i + 1)];
                if (antId && antId !== 'undefined') {
                    this._searchAntennaId[i] = parseInt(antId, 10);
                }
            }
        };
        cDLCHANNEL_SEARCH.prototype.getSatelliteNameById = function (param) {
            var satName = '';
            var antInfo = this._getAntennaInfoFromId(param.antId);
            if (antInfo) {
                satName = antInfo.satelliteType;
                satName = this._SatNameConverter(satName).convName;
            }
            else {
                satName = 'Unknown';
            }
            return satName;
        };
        cDLCHANNEL_SEARCH.prototype.getAntennaIdByName = function (name) {
            var len, i, antInfoLists = this.oipfLocalSystemObj.antennaInfoLists, antInfo;
            antInfoLists.refresh();
            len = antInfoLists.length;
            for (i = 0; i < len; i += 1) {
                antInfo = antInfoLists.item(i);
                if (antInfo.satelliteType.toLowerCase() === name.toLowerCase()) {
                    return antInfo.id;
                }
            }
            return 0;
        };
        cDLCHANNEL_SEARCH.prototype.setTestSatelliteName = function (param) {
            var convName = this._SatNameConverter(param.satName).convName;
            this._TestSatelliteName = convName;
        };
        cDLCHANNEL_SEARCH.prototype.getTestSatelliteName = function () {
            return {
                satName: this._SatNameConverter(this._TestSatelliteName).convName,
                satelNameIndex: this._SatNameConverter(this._TestSatelliteName).index
            };
        };
        cDLCHANNEL_SEARCH.prototype.setAntennaInfo = function (param) {
            var antId = param.antId;
            var antInfo;
            var antLength = this.oipfLocalSystemObj.antennaInfoLists.length;
            hx.logadd('SetSatellite');
            hx.log('SetSatellite', '******subChannelSearch - setAntennaInfo before makedefault*******antennaInfoLists Length : ' + antLength);
            hx.log('SetSatellite', '******subChannelSearch - setAntennaInfo before makedefault*******antId : ' + antId);
            if ((param.antId === 0) || (param.antId === '0') || (param.antId === null) || (antLength === 0)) {
                antId = this._makeAntennaWithDefault(param.satName);
                hx.log('SetSatellite', '******subChannelSearch - _makeAntennaWithDefault *******antId : ' + antId);
            }
            else {
                hx.log('SetSatellite', '******subChannelSearch - Why??? *******antId : ' + antId);
            }
            antLength = this.oipfLocalSystemObj.antennaInfoLists.length;
            hx.log('SetSatellite', '******subChannelSearch - setAntennaInfo after makedefault*******antennaInfoLists Length : ' + antLength);
            hx.log('SetSatellite', '******subChannelSearch - setAntennaInfo after makedefault*******antId : ' + antId);
            antInfo = this._getAntennaInfoFromId(antId);
            this._setAntennaInfoWithMenu(antInfo, param.antSet, param.satName, param.userlnbFreq, param.tuner1Param, param.tuner2Param, param.tuner3Param, param.tuner4Param, param.motorParam);
            return antId;
        };
        cDLCHANNEL_SEARCH.prototype.saveAntennaInfo = function (param) {
            var antId;
            hx.log('dsrvChannelSearch', 'saveAntennaInfo() called!!!');
            this._antennaSetJson[this._curUsingSatelliteIdx].searchType = param.searchType;
            if (param.searchType === 2) {
                var scdPosition = 0;
                // positon A = 0, postion B = 1;
                if (this._curUsingSatelliteIdx > 0) {
                    scdPosition = 1;
                }
                this._antennaSetJson[this._curUsingSatelliteIdx].switchInput = scdPosition;
            }
            antId = this.setAntennaInfo({
                satName: this._searchSatelliteName[this._curUsingSatelliteIdx],
                antSet: this._antennaSetJson[this._curUsingSatelliteIdx],
                antId: this._searchAntennaId[this._curUsingSatelliteIdx],
                userlnbFreq: this._userlnbFreq[this._curUsingSatelliteIdx],
                tuner1Param: this._tuner1Param,
                tuner2Param: this._tuner2Param,
                tuner3Param: this._tuner3Param,
                tuner4Param: this._tuner4Param
            });
            this._setAntennaId(antId, this._curUsingSatelliteIdx);
            this._saveAntIdToLocal(this._curUsingSatelliteIdx);
        };
        cDLCHANNEL_SEARCH.prototype.saveTestAntennaInfo = function (param) {
            if (param.searchType !== undefined) {
                this._testAntennaSetJson.searchType = param.searchType;
            }
            if (param.switchInput !== undefined) {
                if (param.searchType === 2) {
                    var scdPosition = 0;
                    // positon A = 0, postion B = 1;
                    if (this._curUsingSatelliteIdx > 0) {
                        scdPosition = 1;
                    }
                    param.switchInput = scdPosition;
                }
                this._testAntennaSetJson.switchInput = param.switchInput;
            }
            if (param.lnbFreq !== undefined) {
                if (param.searchType === 3) {
                    this._testAntennaSetJson.lnbFreq = this._antennaSetJson[0].lnbFreq;
                }
                else {
                    this._testAntennaSetJson.lnbFreq = param.lnbFreq;
                }
            }
            if (param.toneEnable !== undefined) {
                this._testAntennaSetJson.toneEnable = param.toneEnable;
            }
            this._testAntennaId = this._searchAntennaId[this._curUsingSatelliteIdx];
            var antId = this.setAntennaInfo({
                satName: this._TestSatelliteName,
                antSet: this._testAntennaSetJson,
                antId: this._testAntennaId,
                userlnbFreq: this._userTestlnbFreq,
                tuner1Param: this._tuner1Param,
                tuner2Param: this._tuner2Param,
                tuner3Param: this._tuner3Param,
                tuner4Param: this._tuner4Param,
                motorParam: this._motorSetJson
            });
            this._setTestAntennaId(antId);
            this._searchAntennaId[this._curUsingSatelliteIdx] = antId;
            this._saveAntIdToLocal(this._curUsingSatelliteIdx);
        };
        cDLCHANNEL_SEARCH.prototype.removeAntennaList = function () {
            var i;
            hx.log('dsrvChannelSearch', 'removeAntennaList() called!!!');
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                this._setAntennaId(0, i);
                this._saveAntIdToLocal(i);
                this._searchSatelliteName[i] = 'NONE';
                this._userlnbFreq[i] = 0;
                this._tsIndex[i] = 0;
                this._antennaSetJson[i] = {
                    searchType: 0,
                    switchInput: 0,
                    lnbFreq: 0,
                    toneEnable: 0,
                    diseqcVersion: 0
                };
                this._userTPParam[i] = {
                    userFreq: '0',
                    userPola: 'auto',
                    userSymbol: '27500',
                    userTrans: 'DVB',
                    userFEC: 'auto'
                };
                this._saveTpIdxToLocal(i);
                this._saveTpParamToLocal(i);
            }
            this._lastSettingTuner = 1;
            this._tuner1Param.scdBandNo = 1;
            this._tuner1Param.scdBandFreq = 1210;
            this._tuner2Param.scdBandNo = 2;
            this._tuner2Param.scdBandFreq = 1400;
            this._tuner3Param.scdBandNo = 3;
            this._tuner3Param.scdBandFreq = 1516;
            this._tuner4Param.scdBandNo = 4;
            this._tuner4Param.scdBandFreq = 1632;
            this._initAllAntennaInfo();
            this._removeAntennaList();
        };
        cDLCHANNEL_SEARCH.prototype.removeAnt = function () {
            this._removeAnt(this._curUsingSatelliteIdx);
            this._calcAntlength();
        };
        cDLCHANNEL_SEARCH.prototype.updateNumSatelSetting = function (param) {
            this._curUsingSatelliteIdx = param.numSat;
        };
        cDLCHANNEL_SEARCH.prototype.updateNumTpSetting = function (param) {
            this._curSettingTpIdx = param.numTp;
        };
        cDLCHANNEL_SEARCH.prototype.setSatelliteSetting = function (param) {
            hx.log('dsrvChannelSearch', 'setSatelliteSetting() called!!!');
            if (param.satName !== undefined) {
                this._searchSatelliteName[this._curUsingSatelliteIdx] = this._SatNameConverter(param.satName).convName;
            }
            if (param.switchInput !== undefined) {
                this._antennaSetJson[this._curUsingSatelliteIdx].switchInput = param.switchInput;
            }
            if (param.lnbFreq !== undefined) {
                this._antennaSetJson[this._curUsingSatelliteIdx].lnbFreq = param.lnbFreq;
            }
            if (param.toneEnable !== undefined) {
                this._antennaSetJson[this._curUsingSatelliteIdx].toneEnable = param.toneEnable;
            }
            if (this._curUsingSatelliteIdx === 0) {
                /*motor param Setting*/
                if (param.motorType !== undefined) {
                    this._motorSetJson.motorType = param.motorType;
                }
                if (param.longitude !== undefined) {
                    this._motorSetJson.longitude = param.longitude;
                }
                if (param.ewDirection !== undefined) {
                    this._motorSetJson.ewDirection = param.ewDirection;
                }
                if (param.latitude !== undefined) {
                    this._motorSetJson.latitude = param.latitude;
                }
                if (param.nsDirection !== undefined) {
                    this._motorSetJson.nsDirection = param.nsDirection;
                }
            }
            this._calcAntlength();
        };
        cDLCHANNEL_SEARCH.prototype.getSatelliteSetting = function () {
            var satName = this._SatNameConverter(this._searchSatelliteName[this._curUsingSatelliteIdx]);
            var antennaJson = this._antennaSetJson[this._curUsingSatelliteIdx];
            return {
                satName: satName.convName,
                satNameIndex: satName.index,
                switchInput: antennaJson.switchInput,
                lnbFreq: antennaJson.lnbFreq,
                toneEnable: antennaJson.toneEnable,
                diseqcVersion: antennaJson.diseqcVersion,
                userlnbFreq: this._userlnbFreq[this._curUsingSatelliteIdx]
            };
        };
        cDLCHANNEL_SEARCH.prototype.getAllSatelliteSetting = function () {
            var i;
            var searchSatelName = [];
            var antennaJson = [];
            var userlnbFreq = [];
            var antLength = this._antlength;
            for (i = 0; i < antLength; i += 1) {
                searchSatelName.push(this._SatNameConverter(this._searchSatelliteName[i]).convName);
                antennaJson.push(this._antennaSetJson[i]);
                userlnbFreq.push(this._userlnbFreq[i]);
            }
            return {
                satName: searchSatelName,
                antennaJson: antennaJson,
                userlnbFreq: userlnbFreq,
                antLength: antLength
            };
        };
        cDLCHANNEL_SEARCH.prototype.antennaSetting = function () {
            var i;
            var antInfoList = this._antennaSetting();
            hx.log('dsrvChannelSearch', 'antennaSetting() called!!!');
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                this._searchSatelliteName[i] = 'NONE';
                this._antennaSetJson[i].searchType = 0;
                this._antennaSetJson[i].switchInput = 0;
                this._antennaSetJson[i].lnbFreq = 0;
                this._antennaSetJson[i].toneEnable = 0;
                this._antennaSetJson[i].diseqcVersion = 0;
            }
            this._antlength = antInfoList.length;
            for (i = 0; i < this._antlength; i += 1) {
                this._searchSatelliteName[i] = antInfoList[i].satelliteName;
                this._antennaSetJson[i].lnbFreq = antInfoList[i].lnbFrequency;
                if (this._antennaSetJson[i].lnbFreq === 5) {
                    this._userlnbFreq[i] = antInfoList[i].userlnbFreq;
                }
                this._antennaSetJson[i].switchInput = antInfoList[i].switchInput;
                this._antennaSetJson[i].toneEnable = antInfoList[i].enable22KHzTone;
                this._antennaSetJson[i].searchType = antInfoList[i].antennaType;
                if (i === 0) {
                    this._tuner1Param.scdBandNo = antInfoList[i].scdBandNo1;
                    this._tuner2Param.scdBandNo = antInfoList[i].scdBandNo2;
                    this._tuner3Param.scdBandNo = antInfoList[i].scdBandNo3;
                    this._tuner4Param.scdBandNo = antInfoList[i].scdBandNo4;
                    this._tuner1Param.scdBandFreq = antInfoList[i].scdBandFreq1;
                    this._tuner2Param.scdBandFreq = antInfoList[i].scdBandFreq2;
                    this._tuner3Param.scdBandFreq = antInfoList[i].scdBandFreq3;
                    this._tuner4Param.scdBandFreq = antInfoList[i].scdBandFreq4;
                }
            }
            return antInfoList.length;
        };
        cDLCHANNEL_SEARCH.prototype.getConvertSatName = function (data) {
            var that = this, satName, param = data;
            satName = that._SatNameConverter(param);
            satName.convName = satName.convName.split("&amp;").join("&");
            return satName.convName;
        };
        ////////////////////////*  Manage Satellite END  */////////////////////////////////////////
        /////////////////////* Manage Transponder List START *////////////////////////////////////////////
        cDLCHANNEL_SEARCH.prototype.getTransponderSelectorAll = function (data) {
            // It is used in searchStart Page so I shoud check _tsIndex
            var returnValue = [], index = 0, value, tpList, tpInfo, i;
            // _searchSatelliteName's TPList
            function _getSearchingTP(satName, tsIndex, userTPParam, returnValue) {
                if (tsIndex === 0) {
                    if (satName !== null && satName !== 'NONE') {
                        var idx, idx2;
                        for (idx = 0; idx < this._transponderList.length; idx += 1) {
                            if (this._transponderList[idx].satName === satName) {
                                tpList = this._transponderList[idx].tpList;
                                for (idx2 = 0; idx2 < tpList.length; idx2 += 1) {
                                    tpInfo = tpList[idx2];
                                    value = this._SatNameConverter(satName).convName;
                                    value += (tpInfo[0] / 1000) + ', ';
                                    switch (tpInfo[2]) {
                                        case 1:
                                            value += 'H';
                                            break;
                                        case 2:
                                            value += 'V';
                                            break;
                                        case 4:
                                            value += 'CR';
                                            break;
                                        case 8:
                                            value += 'CL';
                                            break;
                                        default:
                                            value += 'A';
                                            break;
                                    }
                                    value += ', ';
                                    value += (tpInfo[1] / 1000) + ', ';
                                    if (tpInfo[3] === 'auto') {
                                        value += 'A';
                                    }
                                    else {
                                        value += tpInfo[3];
                                    }
                                    returnValue[index] = value;
                                    index += 1;
                                }
                            }
                        }
                    }
                }
                else if (tsIndex === 2) {
                    if (satName !== null && satName !== 'NONE') {
                        value = this._SatNameConverter(satName).convName;
                        value += (userTPParam.userFreq) + ', ';
                        switch (userTPParam.userPola) {
                            case 'horizontal':
                            case 'Horizontal':
                                value += 'H';
                                break;
                            case 'vertical':
                            case 'Vertical':
                                value += 'V';
                                break;
                            default:
                                value += 'A';
                                break;
                        }
                        value += ', ';
                        value += (userTPParam.userSymbol) + ', ';
                        if (userTPParam.userFEC === 'auto') {
                            value += 'A';
                        }
                        else {
                            value += userTPParam.userFEC;
                        }
                        returnValue[index] = value;
                        index += 1;
                    }
                }
                else if (tsIndex > 2) {
                    if (satName !== null && satName !== 'NONE') {
                        var i;
                        for (i = 0; i < this._transponderList.length; i += 1) {
                            if (this._transponderList[i].satName === satName) {
                                tpList = this._transponderList[i].tpList;
                                tpInfo = tpList[tsIndex - 3];
                                value = this._SatNameConverter(satName).convName;
                                value += (tpInfo[0] / 1000) + ', ';
                                switch (tpInfo[2]) {
                                    case 1:
                                        value += 'H';
                                        break;
                                    case 2:
                                        value += 'V';
                                        break;
                                    case 4:
                                        value += 'CR';
                                        break;
                                    case 8:
                                        value += 'CL';
                                        break;
                                    default:
                                        value += 'A';
                                        break;
                                }
                                value += ', ';
                                value += (tpInfo[1] / 1000) + ', ';
                                if (tpInfo[3] === 'auto') {
                                    value += 'A';
                                }
                                else {
                                    value += tpInfo[3];
                                }
                                returnValue[index] = value;
                                index += 1;
                            }
                        }
                    }
                }
            }
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                _getSearchingTP(this._searchSatelliteName[i], this._tsIndex[i], this._userTPParam[i], returnValue);
            }
            return {
                transponderSelector: returnValue
            };
        };
        cDLCHANNEL_SEARCH.prototype.getTpUserSelector = function () {
            //It is used in lnbSearch, diseqcSearch page to show UserSettingTp
            var tpIndex = [], paramToGet = [], tpUserSelector = [], satNameList = [], i;
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                paramToGet.push(this._userTPParam[i]);
                tpIndex.push(this._tsIndex[i]);
                satNameList.push(this._searchSatelliteName[i]);
            }
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                var value = (paramToGet[i].userFreq) + ', ';
                switch (paramToGet[i].userPola) {
                    case 'Horizontal':
                    case 'horizontal':
                        value += 'H';
                        break;
                    case 'Vertical':
                    case 'vertical':
                        value += 'V';
                        break;
                    default:
                        value += 'A';
                        break;
                }
                value += ', ';
                value += (paramToGet[i].userSymbol) + ', ';
                if (paramToGet[i].userFEC === 'auto' || paramToGet[i].userFEC === 'Auto' || paramToGet[i].userFEC === hx.l('LOC_AUTO_ID')) {
                    value += 'A';
                }
                else {
                    value += paramToGet[i].userFEC;
                }
                tpUserSelector.push(value);
            }
            return {
                tpUserSelector: tpUserSelector,
                tpIndex: tpIndex,
                satNameList: satNameList
            };
        };
        cDLCHANNEL_SEARCH.prototype.getUserSettingTestTp = function () {
            //It is used in satelliteSetting page to show UserSettingTp
            var i;
            var value = (this._userTestTPParam.userFreq) + ', ';
            switch (this._userTestTPParam.userPola) {
                case 'Horizontal':
                case 'horizontal':
                    value += 'H';
                    break;
                case 'Vertical':
                case 'vertical':
                    value += 'V';
                    break;
                default:
                    value += 'A';
                    break;
            }
            value += ', ';
            value += (this._userTestTPParam.userSymbol) + ', ';
            if (this._userTestTPParam.userFEC === 'auto') {
                value += 'A';
            }
            else {
                value += this._userTestTPParam.userFEC;
            }
            return {
                value: value,
                userTestTp: this._userTestTPParam,
                testTpIndex: this._testtsIndex
            };
        };
        cDLCHANNEL_SEARCH.prototype.getTransponderSelector = function () {
            //It is used in lbnSearch, diseqcSearch page to show TpList
            var i;
            var allreturnValue = [];
            var returnValue = [];
            var index = 0;
            var tsIndex = [];
            for (i = 0; i < this._antlength; i += 1) {
                tsIndex.push(this._tsIndex[i]);
                returnValue = [];
                if (this._searchSatelliteName[i] === 'NONE') {
                    returnValue.push(hx.l('LOC_NONE_ID'));
                }
                else {
                    var idx, idx2;
                    returnValue.push(hx.l('LOC_ALL_ID'));
                    returnValue.push(hx.l('LOC_NONE_ID'));
                    returnValue.push(hx.l('LOC_USERDEFINE_ID'));
                    for (idx = 0; idx < this._transponderList.length; idx += 1) {
                        if (this._transponderList[idx].satName === this._searchSatelliteName[i]) {
                            var tpList = this._transponderList[idx].tpList;
                            for (idx2 = 0; idx2 < tpList.length; idx2 += 1) {
                                var tpInfo = tpList[idx2];
                                var value;
                                value = (tpInfo[0] / 1000) + ', ';
                                switch (tpInfo[2]) {
                                    case 1:
                                        value += 'H';
                                        break;
                                    case 2:
                                        value += 'V';
                                        break;
                                    case 4:
                                        value += 'CR';
                                        break;
                                    case 8:
                                        value += 'CL';
                                        break;
                                    default:
                                        value += 'A';
                                        break;
                                }
                                value += ', ';
                                value += (tpInfo[1] / 1000) + ', ';
                                if (tpInfo[3] === 'auto') {
                                    value += 'A';
                                }
                                else {
                                    value += tpInfo[3];
                                }
                                returnValue.push(value);
                            }
                        }
                    }
                }
                allreturnValue.push(returnValue);
            }
            // return returnValue;
            return {
                transponderSelector: allreturnValue,
                tsIndex: tsIndex
            };
        };
        cDLCHANNEL_SEARCH.prototype.getTestTransponderSelector = function () {
            //It is used in satelliteSetting page to show testTpList
            var returnValue = [];
            var index = 0;
            // DBG('getValueList for transponderSelector');
            returnValue[index] = hx.l('LOC_USERDEFINE_ID') + ' ';
            index += 1;
            var i;
            for (i = 0; i < this._transponderList.length; i += 1) {
                if (this._transponderList[i].satName === this._TestSatelliteName) {
                    var j, tpList = this._transponderList[i].tpList;
                    for (j = 0; j < tpList.length; j += 1) {
                        var tpInfo = tpList[j];
                        var value;
                        value = (tpInfo[0] / 1000) + ', ';
                        switch (tpInfo[2]) {
                            case 1:
                                value += 'H';
                                break;
                            case 2:
                                value += 'V';
                                break;
                            case 4:
                                value += 'CR';
                                break;
                            case 8:
                                value += 'CL';
                                break;
                            default:
                                value += 'A';
                                break;
                        }
                        value += ', ';
                        value += (tpInfo[1] / 1000) + ', ';
                        if (tpInfo[3] === 'auto') {
                            value += 'A';
                        }
                        else {
                            value += tpInfo[3];
                        }
                        returnValue[index] = value;
                        index += 1;
                    }
                }
            }
            // return returnValue;
            if (returnValue.length <= 1) {
                this._testtsIndex = 0;
            }
            else if (returnValue.length > 1) {
                this._testtsIndex = 1;
            }
            return {
                transponderSelector: returnValue
            };
            //dlEvent : 'dlgetTestTpSelector'
        };
        cDLCHANNEL_SEARCH.prototype.setUserlnbFreq = function (param) {
            this._userlnbFreq[this._curUsingSatelliteIdx] = param.userlnbFreq;
            this._userTestlnbFreq = param.userlnbFreq;
        };
        cDLCHANNEL_SEARCH.prototype.setUserTPParam = function (param) {
            var paramToSet;
            paramToSet = this._userTPParam[this._curSettingTpIdx];
            paramToSet.userFreq = param.userFreq;
            paramToSet.userPola = param.userPola;
            paramToSet.userSymbol = param.userSymbol;
            paramToSet.userTrans = param.userTrans;
            paramToSet.userFEC = param.userFEC;
            this._saveTpParamToLocal(this._curSettingTpIdx);
        };
        cDLCHANNEL_SEARCH.prototype.setUserTestTPParam = function (param) {
            this._userTestTPParam.userFreq = param.userFreq;
            this._userTestTPParam.userPola = param.userPola;
            this._userTestTPParam.userSymbol = param.userSymbol;
            this._userTestTPParam.userTrans = param.userTrans;
            this._userTestTPParam.userFEC = param.userFEC;
        };
        cDLCHANNEL_SEARCH.prototype.setUserTempTPParam = function (param) {
            this._userTempTPParam.userFreq = param.userFreq;
            this._userTempTPParam.userPola = param.userPola;
            this._userTempTPParam.userSymbol = param.userSymbol;
            this._userTempTPParam.userTrans = param.userTrans;
            this._userTempTPParam.userFEC = param.userFEC;
        };
        cDLCHANNEL_SEARCH.prototype.getUserTPParam = function () {
            return {
                tpParam: this._userTPParam[this._curSettingTpIdx]
            };
        };
        cDLCHANNEL_SEARCH.prototype.setTpIndex = function (param) {
            this._tsIndex[this._curSettingTpIdx] = param.tsIndex;
            this._saveTpIdxToLocal(this._curSettingTpIdx);
        };
        cDLCHANNEL_SEARCH.prototype.setTestTpIndex = function (param) {
            this._testtsIndex = param.testtsIndex;
        };
        /////////////////////* Manage Transponder List END *////////////////////////////////////////////
        ///////////////////////////* TODO : Motor ChannelSearch Start *////////////////////////////
        cDLCHANNEL_SEARCH.prototype.motorGetMyPosition = function () {
            var myPosition = this._motorGetMyPosition();
            return {
                myLongitude: myPosition.myLongitude,
                myLatitude: myPosition.myLatitude
            };
        };
        cDLCHANNEL_SEARCH.prototype.motorSetMyPosition = function (data) {
            this._motorSetMyPosition({
                myLongitude: data.myLongitude,
                myLatitude: data.myLatitude,
                ewDirection: data.ewDirection,
                nsDirection: data._nsDirection
            });
        };
        cDLCHANNEL_SEARCH.prototype.setActivationList = function (data) {
            this._activationList = data.activationList;
        };
        cDLCHANNEL_SEARCH.prototype.getActivationList = function () {
            return {
                activationList: this._activationList
            };
        };
        cDLCHANNEL_SEARCH.prototype.setMotorLimit = function (data) {
            if (this._motorLimit !== data.motorLimit && data.motorLimit !== undefined) {
                this._motorLimit = data.motorLimit;
                if (this._motorLimit === 'Off') {
                    this._motorDisableLimit();
                }
                else if (this._motorLimit === 'On') {
                    this._motorEnableLimit();
                }
            }
            if (data.limitDirection !== undefined) {
                this._limitDirection = data.limitDirection;
            }
        };
        cDLCHANNEL_SEARCH.prototype.getMotorLimit = function () {
            return {
                motorLimit: this._motorLimit,
                limitDirection: this._limitDirection
            };
        };
        cDLCHANNEL_SEARCH.prototype.setMotorMoving = function (data) {
            if (data.motorDriving !== undefined) {
                this._motorDriving = data.motorDriving;
            }
            if (data.drivingLevel !== undefined) {
                this._drivingLevel = data.drivingLevel;
            }
        };
        cDLCHANNEL_SEARCH.prototype.getMotorMoving = function () {
            return {
                motorDriving: this._motorDriving,
                drivingLevel: this._drivingLevel
            };
        };
        cDLCHANNEL_SEARCH.prototype.setMotorSetting = function (data) {
            var motorSet = this._motorSetJson;
            if (data.motorType !== undefined) {
                motorSet.motorType = data.motorType;
            }
            if (data.longitude !== undefined) {
                motorSet.longitude = data.longitude;
            }
            if (data.ewDirection !== undefined) {
                motorSet.ewDirection = data.ewDirection;
            }
            if (data.latitude !== undefined) {
                motorSet.latitude = data.latitude;
            }
            if (data.nsDirection !== undefined) {
                motorSet.nsDirection = data.nsDirection;
            }
        };
        cDLCHANNEL_SEARCH.prototype.getMotorType = function () {
            var motorSet = this._motorSetJson;
            return {
                motorType: motorSet.motorType
            };
        };
        cDLCHANNEL_SEARCH.prototype.motorDrive = function (data) {
            this._motorDrive({
                direction: data.direction,
                mode: this._motorDriving,
                step: this._drivingLevel
            });
        };
        cDLCHANNEL_SEARCH.prototype.motorStop = function () {
            this._motorStop();
        };
        cDLCHANNEL_SEARCH.prototype.motorGotoPosition = function () {
            this._motorGotoPosition({
                antId: parseInt(this._testAntennaId, 10)
            });
        };
        cDLCHANNEL_SEARCH.prototype.motorStorePosition = function () {
            this._motorStorePosition({
                antId: parseInt(this._testAntennaId, 10)
            });
        };
        cDLCHANNEL_SEARCH.prototype.motorSetLimit = function () {
            this._motorSetLimit({
                direction: this._limitDirection
            });
        };
        cDLCHANNEL_SEARCH.prototype.motorGotoSatellite = function () {
            this._motorGotoSatellite({
                antId: parseInt(this._testAntennaId, 10)
            });
        };
        cDLCHANNEL_SEARCH.prototype.motorRecalculate = function () {
            this._motorRecalculate({
                antId: parseInt(this._searchAntennaId[0], 10)
            });
        };
        ///////////////////////////* Motor ChannelSearch END *////////////////////////////
        ///////////////////////////*  SCD Channel Search START *///////////////////////////////
        cDLCHANNEL_SEARCH.prototype.setlastSettingTuner = function (param) {
            this._lastSettingTuner = param.lastSettingTuner;
        };
        cDLCHANNEL_SEARCH.prototype.getlastSettingTuner = function () {
            return this._lastSettingTuner;
        };
        cDLCHANNEL_SEARCH.prototype.setScdSetting = function (param) {
            this._tuner1Param.scdBandNo = param.scdBandNo;
            this._tuner2Param.scdBandNo = param.scdBandNo2;
            this._tuner3Param.scdBandNo = param.scdBandNo3;
            this._tuner4Param.scdBandNo = param.scdBandNo4;
            this._tuner1Param.scdBandFreq = param.scdBandFreq;
            this._tuner2Param.scdBandFreq = param.scdBandFreq2;
            this._tuner3Param.scdBandFreq = param.scdBandFreq3;
            this._tuner4Param.scdBandFreq = param.scdBandFreq4;
        };
        cDLCHANNEL_SEARCH.prototype.getScdSetting = function () {
            return {
                scdBandNo: this._tuner1Param.scdBandNo,
                scdBandNo2: this._tuner2Param.scdBandNo,
                scdBandNo3: this._tuner3Param.scdBandNo,
                scdBandNo4: this._tuner4Param.scdBandNo,
                scdBandFreq: this._tuner1Param.scdBandFreq,
                scdBandFreq2: this._tuner2Param.scdBandFreq,
                scdBandFreq3: this._tuner3Param.scdBandFreq,
                scdBandFreq4: this._tuner4Param.scdBandFreq
            };
        };
        ///////////////////////////*  SCD Channel Search END *////////////////////////
        // ####################################################################################################
        // Private Methods
        /////////////////////////// Initialize START ///////////////////////////////////
        cDLCHANNEL_SEARCH.prototype._initializeAntData = function () {
            var i;
            if (ENV.op === ENV.listOP.MIDDLE_EAST) {
                this.SATELLITE_NUMBER_TO_MAKE = 8;
            }
            else {
                this.SATELLITE_NUMBER_TO_MAKE = 4;
            }
            this._TestSatelliteName = 'NONE';
            this._testAntennaId = 0;
            this._testAntennaSetJson = {
                searchType: 0,
                switchInput: 0,
                lnbFreq: 0,
                toneEnable: 0,
                diseqcVersion: 0
            };
            this._testtsIndex = 0;
            this._userTestlnbFreq = 0;
            this._userTestTPParam = {
                userFreq: '0',
                userPola: 'auto',
                userSymbol: '27500',
                userTrans: 'DVB',
                userFEC: 'auto'
            };
            this._userTempTPParam = {
                userFreq: '0',
                userPola: 'auto',
                userSymbol: '27500',
                userTrans: 'DVB',
                userFEC: 'auto'
            };
            this._searchSatelliteName = [];
            this._searchAntennaId = [];
            this._antennaSetJson = [];
            this._tsIndex = [];
            this._userlnbFreq = [];
            this._userTPParam = [];
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                this._searchSatelliteName.push('NONE');
                this._searchAntennaId.push(0);
                this._antennaSetJson.push({
                    searchType: 0,
                    switchInput: 0,
                    lnbFreq: 0,
                    toneEnable: 0,
                    diseqcVersion: 0
                });
                this._tsIndex.push(0);
                this._userlnbFreq.push(0);
                this._userTPParam.push({
                    userFreq: '0',
                    userPola: 'auto',
                    userSymbol: '27500',
                    userTrans: 'DVB',
                    userFEC: 'auto'
                });
            }
        };
        cDLCHANNEL_SEARCH.prototype._loadTpData = function (urlPrefix) {
            var _this = this;
            var prefix = urlPrefix || '';
            $.ajax({
                url: prefix + 'pages/util/satTpData.json',
                dataType: 'json',
                async: false,
                success: function (data) {
                    _this._transponderList = data;
                }
            });
        };
        cDLCHANNEL_SEARCH.prototype._loadSatPosData = function (urlPrefix) {
            var _this = this;
            var prefix = (!!urlPrefix) ? urlPrefix : '';
            $.ajax({
                url: prefix + 'pages/util/satPositionData.json',
                dataType: 'json',
                async: false,
                success: function (data) {
                    _this._satPosDataList = data;
                }
            });
        };
        cDLCHANNEL_SEARCH.prototype._remakeNotPreDefinedMotorAntennaPos = function () {
            var ucIncIdx = 0;
            var i;
            for (i = 0; i < this._satPosDataList.length; i += 1) {
                if (this._satPosDataList[i].posNum === 255) {
                    this._satPosDataList[i].posNum = this.INDEX_MOTOR_USERDEFINE + ucIncIdx;
                    ucIncIdx += 1;
                    if (this.INDEX_MOTOR_USERDEFINE + ucIncIdx > this.MAX_NUM_MOTOR_SATELLITE) {
                        break;
                    }
                }
            }
        };
        /////////////////////////// Initialize END! ///////////////////////////////////////
        //////////////////////  Save to WebStorage & get from WebStorage START ////////////////////////////
        cDLCHANNEL_SEARCH.prototype._saveAntIdToLocal = function (curUsingSatelliteIdx) {
            hx.svc.WEB_STORAGE.saveAntIdToLocal({
                antId: this._searchAntennaId[curUsingSatelliteIdx],
                index: curUsingSatelliteIdx
            });
        };
        cDLCHANNEL_SEARCH.prototype._saveSearchOptionsToLocal = function () {
            hx.svc.WEB_STORAGE.saveSearchOptionsToLocal({
                network: this._searchOptions.network,
                scramble: this._searchOptions.scramble,
                channel: this._searchOptions.channel
            });
        };
        cDLCHANNEL_SEARCH.prototype._getSearchOptionsFromLocal = function () {
            var searchOptions;
            searchOptions = hx.svc.WEB_STORAGE.getSearchOptionsFromLocal();
            if (searchOptions.network && searchOptions.network !== 'undefined') {
                this._searchOptions.network = searchOptions.network;
            }
            else {
                this._saveSearchOptionsToLocal();
            }
            if (searchOptions.scramble && searchOptions.scramble !== 'undefined') {
                this._searchOptions.scramble = searchOptions.scramble;
            }
            else {
                this._saveSearchOptionsToLocal();
            }
            if (searchOptions.channel && searchOptions.channel !== 'undefined') {
                this._searchOptions.channel = searchOptions.channel;
            }
            else {
                this._saveSearchOptionsToLocal();
            }
        };
        cDLCHANNEL_SEARCH.prototype._getTpIdxListFromLocal = function () {
            var i, tpIdxObj;
            tpIdxObj = hx.svc.WEB_STORAGE.getTpIdxListFromLocal();
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                var tpIdx = tpIdxObj['Idx' + (i + 1)];
                if (tpIdx !== null && tpIdx !== undefined) {
                    this._tsIndex[i] = parseInt(tpIdx, 10);
                }
            }
        };
        cDLCHANNEL_SEARCH.prototype._saveTpIdxToLocal = function (curSettingTpIdx) {
            hx.svc.WEB_STORAGE.saveTpIdxToLocal({
                position: curSettingTpIdx,
                Idx: this._tsIndex[curSettingTpIdx]
            });
        };
        cDLCHANNEL_SEARCH.prototype._getTpParamFromLocal = function () {
            var userTpParamObj, i;
            var fnName = '';
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                //get TpParam from WebStorage
                fnName = 'getUserTpParam' + (i + 1) + 'FromLocal';
                userTpParamObj = hx.svc.WEB_STORAGE[fnName]();
                //cache TpParam
                if (userTpParamObj.userFreq !== null && userTpParamObj.userFreq !== undefined) {
                    this._userTPParam[i].userFreq = userTpParamObj.userFreq;
                }
                if (userTpParamObj.userPola !== null && userTpParamObj.userPola !== undefined) {
                    this._userTPParam[i].userPola = userTpParamObj.userPola;
                }
                if (userTpParamObj.userSymbol !== null && userTpParamObj.userSymbol !== undefined) {
                    this._userTPParam[i].userSymbol = userTpParamObj.userSymbol;
                }
                if (userTpParamObj.userTrans !== null && userTpParamObj.userTrans !== undefined) {
                    this._userTPParam[i].userTrans = userTpParamObj.userTrans;
                }
                if (userTpParamObj.userFEC !== null && userTpParamObj.userFEC !== undefined) {
                    this._userTPParam[i].userFEC = userTpParamObj.userFEC;
                }
            }
        };
        cDLCHANNEL_SEARCH.prototype._saveTpParamToLocal = function (curSettingTpIdx) {
            hx.svc.WEB_STORAGE.saveUserTpParamToLocal({
                index: curSettingTpIdx,
                userFreq: this._userTPParam[curSettingTpIdx].userFreq,
                userPola: this._userTPParam[curSettingTpIdx].userPola,
                userSymbol: this._userTPParam[curSettingTpIdx].userSymbol,
                userTrans: this._userTPParam[curSettingTpIdx].userTrans,
                userFEC: this._userTPParam[curSettingTpIdx].userFEC
            });
        };
        //////////////////////  Save to WebStorage & get from WebStorage END! ////////////////////////////
        ////////////////////// Channel Scan Control START ////////////////////////////////////////////////
        cDLCHANNEL_SEARCH.prototype._startScan = function (param) {
            if (param.isStartScan) {
                this.oipfChannelConfigObj.stopScan();
                this._isStartScan = false;
            }
            var scanParamCollection = this._createChannelScanParamCollectionObject();
            var channelScanOptions = this._createChannelScanOptionsObject();
            var userParam = param.usrParam;
            var confParam = {};
            function _convertTpInfoOipf(itemInfo) {
                var modulation, oipfTpInfo;
                oipfTpInfo = {};
                oipfTpInfo.frequency = itemInfo[0];
                //	frequency
                oipfTpInfo.symbolrate = '"' + itemInfo[1] + '"';
                //	symbolrate
                oipfTpInfo.polarization = itemInfo[2];
                //	polarization
                oipfTpInfo.fec = itemInfo[3];
                //	fec
                oipfTpInfo.modulation = itemInfo[4];
                //	modulation mode
                modulation = itemInfo[5];
                //	DVB-S, S2
                if (modulation === 0) {
                    oipfTpInfo.modulation = 0;
                }
                return oipfTpInfo;
            }
            var tpList, j, tpInfo;
            for (j = 0; j < param.satName.length; j += 1) {
                var i;
                if (param.satName[j] === 'NONE') {
                    continue;
                }
                for (i = 0; i < this._transponderList.length; i += 1) {
                    if (this._transponderList[i].satName === param.satName[j]) {
                        tpList = this._transponderList[i].tpList;
                        break;
                    }
                }
                if (param.tsIndex[j] > 0) {
                    switch (param.tsIndex[j]) {
                        case 1:
                            break;
                        case 2:
                            //select User Define
                            //	Convert User Param to OIPF Param
                            confParam.a = 1;
                            confParam.frequency = userParam[j].userFreq * 1000;
                            confParam.symbolrate = (userParam[j].userSymbol * 1000).toString();
                            if (userParam[j].userPola === "Auto" || userParam[j].userPola === "auto") {
                                confParam.polarization = 0;
                            }
                            else if (userParam[j].userPola === "Horizontal" || userParam[j].userPola === "horizontal") {
                                confParam.polarization = 1;
                            }
                            else {
                                confParam.polarization = 2;
                            }
                            confParam.fec = userParam[j].userFEC;
                            switch (userParam[j].userTrans) {
                                case "DVB-S2 (QPSK)":
                                    confParam.modulation = 1;
                                    break;
                                case "DVB-S2 (8PSK)":
                                    confParam.modulation = 2;
                                    break;
                                default:
                                    confParam.modulation = 0;
                                    break;
                            }
                            this._addScanParameters(confParam.frequency, confParam.symbolrate, confParam.polarization, confParam.fec, confParam.modulation, param.antId[j], param.satName[j], scanParamCollection);
                            break;
                        default:
                            tpInfo = tpList[param.tsIndex[j] - 3];
                            confParam = _convertTpInfoOipf(tpInfo);
                            this._addScanParameters(confParam.frequency, confParam.symbolrate, confParam.polarization, confParam.fec, confParam.modulation, param.antId[j], param.satName[j], scanParamCollection);
                            break;
                    }
                }
                else {
                    for (i = 0; i < tpList.length; i += 1) {
                        tpInfo = tpList[i];
                        confParam = _convertTpInfoOipf(tpInfo);
                        this._addScanParameters(confParam.frequency, confParam.symbolrate, confParam.polarization, confParam.fec, confParam.modulation, param.antId[j], param.satName[j], scanParamCollection);
                    }
                }
            }
            switch (param.searchOptions.channel) {
                case 'All':
                    //	Type All
                    this._setScanOptionsChannelType(100, channelScanOptions);
                    break;
                case 'TV':
                    this._setScanOptionsChannelType(0, channelScanOptions);
                    break;
                case 'Radio':
                    this._setScanOptionsChannelType(1, channelScanOptions);
                    break;
                default:
                    this._setScanOptionsChannelType(2, channelScanOptions);
                    break;
            }
            switch (param.searchOptions.scramble) {
                case 'All':
                    this._setScanOptionsCasType(0, channelScanOptions);
                    break;
                case 'FTA':
                    this._setScanOptionsCasType(1, channelScanOptions);
                    break;
                case 'CAS':
                    this._setScanOptionsCasType(4096, channelScanOptions);
                    break;
                default:
                    this._setScanOptionsCasType(0, channelScanOptions);
                    break;
            }
            this._setScanOptionsNetwork((param.searchOptions.network === 'true' ? true : false), channelScanOptions);
            var antInfo = this._getAntennaInfoWithId(param.antId[0]);
            this.oipfChannelConfigObj.onChannelScan = param.fn;
            this.oipfChannelConfigObj.startScan(channelScanOptions, scanParamCollection);
            this._isStartScan = true;
        };
        cDLCHANNEL_SEARCH.prototype._startAntennaConnection = function (param) {
            if (param.isStartScan) {
                this.oipfChannelConfigObj.stopScan();
                this._isStartScan = false;
            }
            var scanParamCollection = this._createChannelScanParamCollectionObject();
            var confParam = {};
            function _convertTpInfoOipf(itemInfo) {
                var oipfTpInfo;
                oipfTpInfo = {};
                oipfTpInfo.frequency = itemInfo[0];
                //	frequency
                oipfTpInfo.symbolrate = '"' + itemInfo[1] + '"';
                //	symbolrate
                oipfTpInfo.polarization = itemInfo[2];
                //	polarization
                oipfTpInfo.fec = itemInfo[3];
                //	fec
                oipfTpInfo.modulation = itemInfo[4];
                //	modulation mode
                var modulation = itemInfo[5];
                //	DVB-S, S2
                if (modulation === 0) {
                    oipfTpInfo.modulation = 0;
                }
                return oipfTpInfo;
            }
            var tpList, tpInfo, i, j, lockingDataList, antInfo;
            lockingDataList = param.lockingDataList;
            for (i = 0; i < lockingDataList.length; i += 1) {
                tpInfo = lockingDataList[i].tpData;
                antInfo = this._getAntennaInfoFromId(lockingDataList[i].antId);
                confParam = _convertTpInfoOipf(tpInfo);
                this._addScanParameters(confParam.frequency, confParam.symbolrate, confParam.polarization, confParam.fec, confParam.modulation, lockingDataList[i].antId, antInfo.satelliteType, scanParamCollection);
            }
            this.oipfChannelConfigObj.onChannelScan = param.fn;
            this.oipfChannelConfigObj.startScan(null, scanParamCollection, 'antennaconnection');
            this._isStartScan = true;
        };
        cDLCHANNEL_SEARCH.prototype._startTune = function (param) {
            if (param.isStartScan) {
                this.oipfChannelConfigObj.stopScan();
                this._isStartScan = false;
            }
            var channelScanParams = this.oipfChannelConfigObj.createChannelScanParametersObject(11);
            var connectionType = this.oipfLocalSystemObj.antennaInfoLists.getConnectionType();
            function _convertTpInfoOipf(itemInfo) {
                var oipfTpInfo;
                oipfTpInfo = [];
                oipfTpInfo.frequency = itemInfo[0];
                //	frequency
                oipfTpInfo.symbolrate = itemInfo[1];
                //	symbolrate
                oipfTpInfo.polarization = itemInfo[2];
                //	polarization
                oipfTpInfo.fec = itemInfo[3];
                if (itemInfo[3] === 'auto' || itemInfo[3] === 'Auto') {
                    oipfTpInfo.fec = 'auto';
                }
                //	fec
                oipfTpInfo.modulation = itemInfo[4];
                //	modulation mode
                if (itemInfo[5] === 0) {
                    oipfTpInfo.modulation = 0;
                }
                // DVB-S, S2
                return oipfTpInfo;
            }
            var tpList, i;
            for (i = 0; i < this._transponderList.length; i += 1) {
                if (this._transponderList[i].satName === param.satName) {
                    tpList = this._transponderList[i].tpList;
                    break;
                }
            }
            var frequency = 0;
            var symbolrate = 0;
            var modulation = 0;
            var confParam = {};
            var fec, polarization;
            var userParam = param.usrParam;
            if (param.tsIndex === 0) {
                //	Convert User Param to OIPF Param
                frequency = userParam.userFreq * 1000;
                symbolrate = (userParam.userSymbol * 1000);
                fec = userParam.userFEC;
                if (userParam.userFEC === 'auto' || userParam.userFEC === 'Auto') {
                    fec = 'auto';
                }
                switch (userParam.userTrans) {
                    case "DVB-S2 (QPSK)":
                        modulation = 1;
                        break;
                    case "DVB-S2 (8PSK)":
                        modulation = 2;
                        break;
                    default:
                        modulation = 0;
                        break;
                }
                if (userParam.userPola === 'auto' || userParam.userPola === 'Auto') {
                    polarization = 0;
                }
                else if (userParam.userPola === 'horizontal' || userParam.userPola === 'Horizontal') {
                    polarization = 1;
                }
                else if (userParam.userPola === 'vertical' || userParam.userPola === 'Vertical') {
                    polarization = 2;
                }
                else {
                    polarization = 0;
                }
            }
            else {
                var tpInfo = tpList[param.tsIndex - 1];
                confParam = _convertTpInfoOipf(tpInfo);
                frequency = (!!confParam.frequency) ? confParam.frequency : 0;
                symbolrate = (!!confParam.symbolrate) ? confParam.symbolrate : '27500000';
                modulation = confParam.modulation;
                polarization = confParam.polarization;
                fec = confParam.fec;
                if (frequency < 100000) {
                    frequency *= 1000;
                }
                if (symbolrate < 100000) {
                    symbolrate *= 1000;
                }
            }
            var satName = param.satName;
            var antId = param.antId;
            this._settingChannelScanParameters(frequency, symbolrate.toString(), polarization, fec, modulation, antId, satName, channelScanParams);
            var antInfo = this._getAntennaInfoWithId(antId);
            //this.channelConfig.onChannelScan = param.fn;
            this.oipfChannelConfigObj.startScan(null, channelScanParams, 'tuneonly');
            this._isStartScan = true;
            //connectionType ? 'tunetwin' : 'tuneonly'
            //When search channels, do not consider connectionType because we use only first tuner
            return true;
        };
        cDLCHANNEL_SEARCH.prototype._disecScanCompleted = function () {
            this.oipfChannelConfigObj.controlScan('load');
            this.oipfChannelConfigObj.controlScan('save');
            this.oipfChannelConfigObj.stopScan();
            this._isStartScan = false;
            if (ENV.listModel.IR4000HD === ENV.model) {
                return this._getAllAntennaInfo();
            }
            return this._getAntennaInfo();
        };
        cDLCHANNEL_SEARCH.prototype._save_disecScanCompletedAntenna = function (scanData) {
            hx.log('dsrvChannelSearch', '_save_disecScanCompletedAntenna() called!!!');
            hx.log('warning', '');
            hx.log('warning', '');
            hx.log('warning', '######################## DiseqC Scan Result Start #######################');
            /*
             * antennaType defined in dlib_base_types.h
             *      eAntType_LNB : 0
             *      eAntType_Diseqc : 1
             *      eAntType_SCD : 2
             *      eAntType_SMATV : 3
             *      eAntType_MonoBlock : 4
             */
            hx.log('warning', 'Detected Antenna Type is : ' + ((scanData.antennaType === 0) ? 'LNB' : ((scanData.antennaType === 1) ? 'DISEQC' : 'OTHER')));
            hx.log('DiseqcScan', 'scanData.antennaType : ' + scanData.antennaType);
            hx.log('DiseqcScan', 'scanData.switchInput : ' + scanData.switchInput);
            hx.log('DiseqcScan', 'scanData.lnbFrequency : ' + scanData.lnbFrequency);
            hx.log('DiseqcScan', 'scanData.enable22KHzTone : ' + scanData.enable22KHzTone);
            hx.log('DiseqcScan', 'scanData.satelliteType : ' + scanData.satelliteType);
            hx.log('warning', '######################## DiseqC Scan Result End #######################');
            hx.log('warning', '');
            hx.log('warning', '');
            this._antennaSetJson[this._curUsingSatelliteIdx].searchType = scanData.antennaType;
            this._antennaSetJson[this._curUsingSatelliteIdx].switchInput = scanData.switchInput;
            this._antennaSetJson[this._curUsingSatelliteIdx].lnbFreq = scanData.lnbFrequency;
            this._antennaSetJson[this._curUsingSatelliteIdx].toneEnable = scanData.enable22KHzTone;
            this._searchSatelliteName[this._curUsingSatelliteIdx] = scanData.satelliteType;
            var antId;
            //consider only two Antenna type Diseqc Scan
            antId = this.setAntennaInfo({
                satName: this._searchSatelliteName[this._curUsingSatelliteIdx],
                antSet: this._antennaSetJson[this._curUsingSatelliteIdx],
                antId: this._searchAntennaId[this._curUsingSatelliteIdx],
                tuner1Param: this._tuner1Param,
                tuner2Param: this._tuner2Param,
                tuner3Param: this._tuner3Param,
                tuner4Param: this._tuner4Param
            });
            this._setAntennaId(antId, this._curUsingSatelliteIdx);
            this._saveAntIdToLocal(this._curUsingSatelliteIdx);
        };
        cDLCHANNEL_SEARCH.prototype._updateChannelList = function () {
            hx.log('updateLcnChannelList', 'call oipfHLcnUpdate.startLcnUpdate(lcnScanType.UPDATE)');
            this.oipfHLcnUpdate.startLcnUpdate(this.lcnScanType.UPDATE);
        };
        cDLCHANNEL_SEARCH.prototype._changeChannelList = function () {
            hx.log('changeLcnChannelList', 'call oipfHLcnUpdate.startLcnUpdate(lcnScanType.CHANGE)');
            this.oipfHLcnUpdate.startLcnUpdate(this.lcnScanType.CHANGE);
        };
        cDLCHANNEL_SEARCH.prototype._saveLcnUpdate = function (param) {
            hx.log('saveLcnUpdate', 'call oipfHLcnUpdate.saveLcnUpdate(param.searchMode, param.index), param.searchMode : ' + param.searchMode + ', param.index : ' + param.index);
            this.oipfHLcnUpdate.saveLcnUpdate(param.searchMode, param.index);
        };
        cDLCHANNEL_SEARCH.prototype._getLcnUpdateInfo = function () {
            this.oipfHLcnUpdate.getLcnUpdateInfo();
        };
        cDLCHANNEL_SEARCH.prototype._removeLcnUpdateInfo = function () {
            var sync = false;
            this.oipfHLcnUpdate.removeLcnUpdateInfo(sync);
        };
        //////////////////////////// Channel Scan Control END! //////////////////////////////////////////
        /////////////////////////// Manage Channel Scan Parameter START ///////////////////////////////////////
        cDLCHANNEL_SEARCH.prototype._createChannelScanParamCollectionObject = function () {
            var scanParamCollection = this.oipfChannelConfigObj.createChannelScanParametersObject();
            return scanParamCollection;
        };
        cDLCHANNEL_SEARCH.prototype._addScanParameters = function (freq, sr, polar, fec, modul, searchAntennaId, searchSatelliteName, scanParamCollection) {
            var channelParams = this.oipfChannelConfigObj.createChannelScanParametersObject(11);
            channelParams = this._settingChannelScanParameters(freq, sr, polar, fec, modul, searchAntennaId, searchSatelliteName, channelParams);
            scanParamCollection.addChannelScanParam(channelParams);
        };
        cDLCHANNEL_SEARCH.prototype._settingChannelScanParameters = function (freq, sr, polar, fec, modul, searchAntennaId, searchSatelliteName, channelParams) {
            channelParams.startFrequency = freq;
            channelParams.endFrequency = freq;
            channelParams.symbolRate = sr;
            channelParams.modulationModes = modul;
            channelParams.polarisation = polar;
            channelParams.codeRate = fec;
            channelParams.antennaId = searchAntennaId;
            channelParams.antennaName = searchSatelliteName;
            return channelParams;
        };
        cDLCHANNEL_SEARCH.prototype._createChannelScanOptionsObject = function () {
            var channelScanOptions = this.oipfChannelConfigObj.createChannelScanOptionsObject();
            return channelScanOptions;
        };
        cDLCHANNEL_SEARCH.prototype._setScanOptionsChannelType = function (channelType, channelScanOptions) {
            channelScanOptions.channelType = channelType;
        };
        cDLCHANNEL_SEARCH.prototype._setScanOptionsCasType = function (casType, channelScanOptions) {
            channelScanOptions.casType = casType;
        };
        cDLCHANNEL_SEARCH.prototype._setScanOptionsNetwork = function (network, channelScanOptions) {
            channelScanOptions.enableNetworkScan = network;
        };
        cDLCHANNEL_SEARCH.prototype._getTuningInfo = function (satName, type) {
            // type
            // 0 : unused
            // 1 : diseqc detection
            // 2 : antenna connection
            // 4 : both
            var a = this, tp = a._transponderList, data = [], i, j, tpListLength;
            for (i = 0, length = tp.length; i < length; i += 1) {
                if (tp[i].satName === satName) {
                    for (j = 0, tpListLength = tp[i].tpList.length; j < tpListLength; j += 1) {
                        if (tp[i].tpList[j][6] === type) {
                            data.push(tp[i].tpList[j]);
                        }
                    }
                }
            }
            return data;
        };
        /////////////////////////// Manage Channel Scan Parameter END! ///////////////////////////////////////
        /////////////////////////// Manage Satellite START //////////////////////////////////////////////
        cDLCHANNEL_SEARCH.prototype._antennaSetting = function () {
            var i;
            var antInfoList = [];
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                if (!!this._searchAntennaId[i] && this._searchAntennaId[i] !== '0') {
                    var antInfo = this._getAntennaInfoFromId(this._searchAntennaId[i]);
                    if (antInfo.satelliteName !== 'NONE') {
                        antInfoList.push(this._convertAntennaData(antInfo));
                    }
                    if (i === 0) {
                        if (antInfo.antennaType === 0) {
                            return antInfoList;
                        }
                    }
                }
            }
            return antInfoList;
        };
        cDLCHANNEL_SEARCH.prototype._getAllAntennaInfo = function () {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            return antInfoLists;
        };
        cDLCHANNEL_SEARCH.prototype._getAntennaInfo = function () {
            var antInfo = this._doGetCurrentAntInfo();
            if (antInfo === null) {
                return null;
            }
            var convAntInfo = this._convertAntennaData(antInfo);
            if (ENV.op === ENV.listOP.MIDDLE_EAST) {
                return {
                    antennaType: convAntInfo.antennaType,
                    lnbFrequency: convAntInfo.lnbFrequency,
                    switchInput: convAntInfo.switchInput,
                    enable22KHzTone: convAntInfo.enable22KHzTone,
                    satelliteName: this._MEdefaultAntName
                };
            }
            return {
                antennaType: convAntInfo.antennaType,
                lnbFrequency: convAntInfo.lnbFrequency,
                switchInput: convAntInfo.switchInput,
                enable22KHzTone: convAntInfo.enable22KHzTone,
                satelliteName: convAntInfo.satelliteName,
                satelliteType: convAntInfo.satelliteType
            };
        };
        cDLCHANNEL_SEARCH.prototype._doGetCurrentAntInfo = function () {
            var ret = null;
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            var curAntName = this._searchSatelliteName[this._curUsingSatelliteIdx];
            var i, len = antInfoLists.length;
            for (i = 0; i < len; i += 1) {
                var antInfo = antInfoLists.item(i);
                if (ENV.op === ENV.listOP.MIDDLE_EAST) {
                    if (antInfo.satelliteType.toLowerCase() === this._MEdefaultAntName.toLowerCase()) {
                        ret = antInfo;
                    }
                }
                else {
                    if (antInfo.satelliteType.toLowerCase() === curAntName.toLowerCase()) {
                        ret = antInfo;
                    }
                }
            }
            return ret;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaId = function (id, curUsingSatelliteIdx) {
            this._searchAntennaId[curUsingSatelliteIdx] = id;
        };
        cDLCHANNEL_SEARCH.prototype._setTestAntennaId = function (id) {
            this._testAntennaId = id;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfolnbFreq = function (antInfo, lnbFreq) {
            antInfo.lnbFrequency = lnbFreq;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoantennaType = function (antInfo, antennaType) {
            antInfo.antennaType = antennaType;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoswitchInput = function (antInfo, switchInput) {
            antInfo.switchInput = switchInput;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfodiseqcVersion = function (antInfo, diseqcVersion) {
            antInfo.diseqcVersion = diseqcVersion;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoenable22KHzTone = function (antInfo, enable22KHzTone) {
            antInfo.enable22KHzTone = enable22KHzTone;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoscdBandNo0 = function (antInfo, scdBandNo0) {
            antInfo.scdBandNo0 = scdBandNo0;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoscdBandNo1 = function (antInfo, scdBandNo1) {
            antInfo.scdBandNo1 = scdBandNo1;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoscdBandNo2 = function (antInfo, scdBandNo2) {
            antInfo.scdBandNo2 = scdBandNo2;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoscdBandNo3 = function (antInfo, scdBandNo3) {
            antInfo.scdBandNo3 = scdBandNo3;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoscdBandFreq0 = function (antInfo, scdBandFreq0) {
            antInfo.scdBandFreq0 = scdBandFreq0;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoscdBandFreq1 = function (antInfo, scdBandFreq1) {
            antInfo.scdBandFreq1 = scdBandFreq1;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoscdBandFreq2 = function (antInfo, scdBandFreq2) {
            antInfo.scdBandFreq2 = scdBandFreq2;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoscdBandFreq3 = function (antInfo, scdBandFreq3) {
            antInfo.scdBandFreq3 = scdBandFreq3;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfosatName = function (antInfo, satName) {
            antInfo.satelliteName = satName;
            antInfo.satelliteType = satName;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoposition = function (antInfo, position) {
            antInfo.position = position;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfolongitude = function (antInfo, longitude) {
            antInfo.longitude = longitude;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfolnbVoltage = function (antInfo, lnbVoltage) {
            antInfo.lnbVoltage = lnbVoltage;
        };
        cDLCHANNEL_SEARCH.prototype._antInfoCommit = function (antInfo) {
            //hx.logadd('sh');
            hx.log('sh', '############# AntennaInfo #################');
            hx.log('sh', '## antInfo.antennaType = ' + antInfo.antennaType);
            hx.log('sh', '## antInfo.satelliteType = ' + antInfo.satelliteType);
            hx.log('sh', '## antInfo.satelliteName = ' + antInfo.satelliteName);
            hx.log('sh', '## antInfo.lnbFrequency = ' + antInfo.lnbFrequency);
            hx.log('sh', '## antInfo.lnbVoltage = ' + antInfo.lnbVoltage);
            hx.log('sh', '## antInfo.diseqcVersion = ' + antInfo.diseqcVersion);
            hx.log('sh', '## antInfo.switchInput = ' + antInfo.switchInput);
            hx.log('sh', '## antInfo.enable22KHzTone = ' + antInfo.enable22KHzTone);
            hx.log('sh', '## antInfo.position = ' + antInfo.position);
            hx.log('sh', '## antInfo.longitude = ' + antInfo.longitude);
            hx.log('sh', '## antInfo.scdBandNo0 = ' + antInfo.scdBandNo0);
            hx.log('sh', '## antInfo.scdBandNo1 = ' + antInfo.scdBandNo1);
            hx.log('sh', '## antInfo.scdBandFreq0 = ' + antInfo.scdBandFreq0);
            hx.log('sh', '## antInfo.scdBandFreq1 = ' + antInfo.scdBandFreq1);
            antInfo.commit();
        };
        cDLCHANNEL_SEARCH.prototype._antInfoListsCommit = function () {
            var antInfoList = this.oipfLocalSystemObj.antennaInfoLists;
            antInfoList.commit();
        };
        cDLCHANNEL_SEARCH.prototype._removeAntennaList = function () {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            hx.log('dsrvChannelSearch', '_removeAntennaList() called!!!');
            antInfoLists.removeAll();
            antInfoLists.commit();
        };
        cDLCHANNEL_SEARCH.prototype._removeAnt = function (curUsingSatelliteIdx) {
            var i;
            this._setAntennaId(0, curUsingSatelliteIdx);
            this._saveAntIdToLocal(curUsingSatelliteIdx);
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                if (i > curUsingSatelliteIdx) {
                    if (this._searchAntennaId[i] !== 0) {
                        this._moveDataFromTo(i, i - 1);
                    }
                }
            }
        };
        cDLCHANNEL_SEARCH.prototype._moveDataFromTo = function (fromSatelliteIdx, toSatelliteIdx) {
            this._searchSatelliteName[toSatelliteIdx] = this._searchSatelliteName[fromSatelliteIdx];
            this._userlnbFreq[toSatelliteIdx] = this._userlnbFreq[fromSatelliteIdx];
            this._antennaSetJson[toSatelliteIdx].searchType = this._antennaSetJson[fromSatelliteIdx].searchType;
            this._antennaSetJson[toSatelliteIdx].switchInput = this._antennaSetJson[fromSatelliteIdx].switchInput;
            this._antennaSetJson[toSatelliteIdx].lnbFreq = this._antennaSetJson[fromSatelliteIdx].lnbFreq;
            this._antennaSetJson[toSatelliteIdx].toneEnable = this._antennaSetJson[fromSatelliteIdx].toneEnable;
            this._antennaSetJson[toSatelliteIdx].diseqcVersion = this._antennaSetJson[fromSatelliteIdx].diseqcVersion;
            this._searchAntennaId[toSatelliteIdx] = this._searchAntennaId[fromSatelliteIdx];
            this._tsIndex[toSatelliteIdx] = this._tsIndex[fromSatelliteIdx];
            this._saveTpIdxToLocal(toSatelliteIdx);
            this._saveAntIdToLocal(toSatelliteIdx);
            this._searchSatelliteName[fromSatelliteIdx] = 'NONE';
            this._searchAntennaId[fromSatelliteIdx] = 0;
            this._userlnbFreq[fromSatelliteIdx] = 0;
            this._saveAntIdToLocal(fromSatelliteIdx);
            this._antennaSetJson[fromSatelliteIdx] = {
                searchType: 0,
                switchInput: 0,
                lnbFreq: 0,
                toneEnable: 0,
                diseqcVersion: 0
            };
            this._tsIndex[fromSatelliteIdx] = 0;
            this._saveTpIdxToLocal(fromSatelliteIdx);
            this._userTPParam[toSatelliteIdx].userFreq = this._userTPParam[fromSatelliteIdx].userFreq;
            this._userTPParam[toSatelliteIdx].userPola = this._userTPParam[fromSatelliteIdx].userPola;
            this._userTPParam[toSatelliteIdx].userSymbol = this._userTPParam[fromSatelliteIdx].userSymbol;
            this._userTPParam[toSatelliteIdx].userTrans = this._userTPParam[fromSatelliteIdx].userTrans;
            this._userTPParam[toSatelliteIdx].userFEC = this._userTPParam[fromSatelliteIdx].userFEC;
            this._saveAntIdToLocal(toSatelliteIdx);
            this._saveTpParamToLocal(toSatelliteIdx);
            this._userTPParam[fromSatelliteIdx] = {
                userFreq: '0',
                userPola: 'auto',
                userSymbol: '27500',
                userTrans: 'DVB',
                userFEC: 'auto'
            };
            this._saveTpParamToLocal(fromSatelliteIdx);
        };
        cDLCHANNEL_SEARCH.prototype._calcAntlength = function () {
            var i;
            this._antlength = 0;
            for (i = 0; i < this.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                if ((this._searchSatelliteName[i] !== 'NONE') && (this._searchSatelliteName[i])) {
                    this._antlength += 1;
                }
            }
        };
        cDLCHANNEL_SEARCH.prototype._makeAntennaWithDefault = function (satName) {
            var antId = this._createAntennaInfo();
            var antInfo = this._getAntennaInfoWithId(antId);
            this._makeAntennaDefault(satName, antInfo);
            this._antInfoCommit(antInfo);
            return antId;
        };
        cDLCHANNEL_SEARCH.prototype._createAntennaInfo = function () {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            var antId = 0;
            antId = this.getAntennaIdByName(this._dfaultEmptyAntName);
            if (antId === 0) {
                antId = antInfoLists.createAntennaInfo();
            }
            return antId;
        };
        cDLCHANNEL_SEARCH.prototype._getAntennaInfoFromId = function (antId) {
            if (antId === 0) {
                return null;
            }
            return this._getAntennaInfoWithId(antId);
        };
        cDLCHANNEL_SEARCH.prototype._getAntennaInfoWithId = function (antId) {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            var antInfo;
            antInfo = antInfoLists.getAntennaInfo(antId);
            return antInfo;
        };
        cDLCHANNEL_SEARCH.prototype._makeAntennaDefault = function (satName, antInfo) {
            antInfo.satelliteType = satName;
            antInfo.satelliteName = satName;
            antInfo.antennaType = antInfo.TYPE_LNB;
            antInfo.lnbFrequency = antInfo.LNBFREQ_UNIVERSAL;
        };
        cDLCHANNEL_SEARCH.prototype._setAntennaInfoWithMenu = function (antInfo, antennaSetJson, satName, userlnbFreq, tuner1Param, tuner2Param, tuner3Param, tuner4Param, motorParam) {
            var dInput;
            this._setAntennaInfosatName(antInfo, satName);
            switch (antennaSetJson.lnbFreq) {
                case -1:
                case 0:
                    this._setAntennaInfolnbFreq(antInfo, antInfo.LNBFREQ_UNIVERSAL);
                    break;
                case 1:
                    this._setAntennaInfolnbFreq(antInfo, 9750);
                    break;
                case 2:
                    this._setAntennaInfolnbFreq(antInfo, 10600);
                    break;
                case 3:
                    this._setAntennaInfolnbFreq(antInfo, 5150);
                    break;
                case 4:
                    this._setAntennaInfolnbFreq(antInfo, 11475);
                    break;
                case 5:
                    this._setAntennaInfolnbFreq(antInfo, userlnbFreq & 0x7FFFFFFF);
                    break;
            }
            this._setAntennaInfoantennaType(antInfo, antInfo.TYPE_DISEQC);
            this._setAntennaInfoswitchInput(antInfo, antInfo.SW_N);
            if (antennaSetJson.searchType === 0) {
                this._setAntennaInfoantennaType(antInfo, antInfo.TYPE_LNB);
                this._setAntennaInfoswitchInput(antInfo, antInfo.SW_N);
            }
            else if (antennaSetJson.searchType === 1) {
                dInput = (antennaSetJson.switchInput) % 4;
                switch (dInput) {
                    case 0:
                        this._setAntennaInfoswitchInput(antInfo, antInfo.SW_A);
                        break;
                    case 1:
                        this._setAntennaInfoswitchInput(antInfo, antInfo.SW_B);
                        break;
                    case 2:
                        this._setAntennaInfoswitchInput(antInfo, antInfo.SW_C);
                        break;
                    case 3:
                        this._setAntennaInfoswitchInput(antInfo, antInfo.SW_D);
                        break;
                }
                if (antennaSetJson.switchInput < 4) {
                    this._setAntennaInfodiseqcVersion(antInfo, antInfo.DISEQC_1_0);
                }
                else {
                    this._setAntennaInfodiseqcVersion(antInfo, antInfo.DISEQC_2_0);
                }
            }
            else if (antennaSetJson.searchType === 2) {
                this._setAntennaInfoantennaType(antInfo, antInfo.TYPE_SCD);
                dInput = (antennaSetJson.switchInput) % 4;
                switch (dInput) {
                    case 0:
                        this._setAntennaInfoswitchInput(antInfo, antInfo.SW_A);
                        break;
                    case 1:
                        this._setAntennaInfoswitchInput(antInfo, antInfo.SW_B);
                        break;
                    case 2:
                        this._setAntennaInfoswitchInput(antInfo, antInfo.SW_C);
                        break;
                    case 3:
                        this._setAntennaInfoswitchInput(antInfo, antInfo.SW_D);
                        break;
                }
                function _convertScdBandNo(strscdBandNo) {
                    var intscdBandNo = 0;
                    switch (strscdBandNo) {
                        case 1:
                            intscdBandNo = 0;
                            break;
                        case 2:
                            intscdBandNo = 1;
                            break;
                        case 3:
                            intscdBandNo = 2;
                            break;
                        case 4:
                            intscdBandNo = 3;
                            break;
                        case 5:
                            intscdBandNo = 4;
                            break;
                        case 6:
                            intscdBandNo = 5;
                            break;
                        case 7:
                            intscdBandNo = 6;
                            break;
                        case 8:
                            intscdBandNo = 7;
                            break;
                    }
                    return intscdBandNo;
                }
                var scdBandNo0, scdBandNo1, scdBandNo2, scdBandNo3;
                scdBandNo0 = _convertScdBandNo(tuner1Param.scdBandNo);
                scdBandNo1 = _convertScdBandNo(tuner2Param.scdBandNo);
                scdBandNo2 = _convertScdBandNo(tuner3Param.scdBandNo);
                scdBandNo3 = _convertScdBandNo(tuner4Param.scdBandNo);
                this._setAntennaInfoscdBandNo0(antInfo, scdBandNo0);
                this._setAntennaInfoscdBandNo1(antInfo, scdBandNo1);
                this._setAntennaInfoscdBandNo2(antInfo, scdBandNo2);
                this._setAntennaInfoscdBandNo3(antInfo, scdBandNo3);
                this._setAntennaInfoscdBandFreq0(antInfo, parseInt(tuner1Param.scdBandFreq, 10));
                this._setAntennaInfoscdBandFreq1(antInfo, parseInt(tuner2Param.scdBandFreq, 10));
                this._setAntennaInfoscdBandFreq2(antInfo, parseInt(tuner3Param.scdBandFreq, 10));
                this._setAntennaInfoscdBandFreq3(antInfo, parseInt(tuner4Param.scdBandFreq, 10));
            }
            else if (antennaSetJson.searchType === 3) {
                this._setAntennaInfoantennaType(antInfo, antInfo.TYPE_DISEQC);
                this._setAntennaInfoswitchInput(antInfo, antInfo.SW_N);
                if (motorParam.motorType === 'DiSEqC 1.2') {
                    this._setAntennaInfodiseqcVersion(antInfo, antInfo.DISEQC_1_2);
                }
                else if (motorParam.motorType === 'USALS') {
                    this._setAntennaInfodiseqcVersion(antInfo, antInfo.DISEQC_1_3);
                }
            }
            if (antennaSetJson.toneEnable === 0) {
                this._setAntennaInfoenable22KHzTone(antInfo, true);
            }
            else {
                this._setAntennaInfoenable22KHzTone(antInfo, false);
            }
            for (var i = 0; i < this._satPosDataList.length; i += 1) {
                if (this._satPosDataList[i].satName === satName) {
                    var position = this._satPosDataList[i].posNum;
                    var longitude = this._satPosDataList[i].satDegree;
                    break;
                }
            }
            this._setAntennaInfoposition(antInfo, position);
            this._setAntennaInfolongitude(antInfo, longitude);
            this._setAntennaInfolnbVoltage(antInfo, 0);
            //0 : standard,  1: high
            this._antInfoCommit(antInfo);
            this._antInfoListsCommit();
        };
        cDLCHANNEL_SEARCH.prototype._convertAntennaData = function (antInfo) {
            var convAntInfo = {};
            convAntInfo.satelliteName = antInfo.satelliteName;
            convAntInfo.satelliteType = antInfo.satelliteType;
            switch (antInfo.lnbFrequency) {
                case -1:
                case 4294967295:
                    convAntInfo.lnbFrequency = 0;
                    break;
                case 9750:
                    convAntInfo.lnbFrequency = 1;
                    break;
                case 10600:
                    convAntInfo.lnbFrequency = 2;
                    break;
                case 5150:
                    convAntInfo.lnbFrequency = 3;
                    break;
                case 11475:
                    convAntInfo.lnbFrequency = 4;
                    break;
                default:
                    convAntInfo.lnbFrequency = 5;
                    convAntInfo.userlnbFreq = antInfo.lnbFrequency;
                    break;
            }
            if (antInfo.diseqcVersion === 0) {
                switch (antInfo.switchInput) {
                    case 1:
                        convAntInfo.switchInput = 0;
                        break;
                    case 2:
                        convAntInfo.switchInput = 1;
                        break;
                    case 3:
                        convAntInfo.switchInput = 2;
                        break;
                    case 4:
                        convAntInfo.switchInput = 3;
                        break;
                }
            }
            else {
                switch (antInfo.switchInput) {
                    case 1:
                        convAntInfo.switchInput = 4;
                        break;
                    case 2:
                        convAntInfo.switchInput = 5;
                        break;
                    case 3:
                        convAntInfo.switchInput = 6;
                        break;
                    case 4:
                        convAntInfo.switchInput = 7;
                        break;
                }
            }
            if (antInfo.enable22KHzTone) {
                convAntInfo.enable22KHzTone = 0;
            }
            else {
                convAntInfo.enable22KHzTone = 1;
            }
            convAntInfo.antennaType = antInfo.antennaType;
            function _convertScdBandNo(strscdBandNo) {
                var intscdBandNo = 0;
                switch (strscdBandNo) {
                    case 0:
                        intscdBandNo = 1;
                        break;
                    case 1:
                        intscdBandNo = 2;
                        break;
                    case 2:
                        intscdBandNo = 3;
                        break;
                    case 3:
                        intscdBandNo = 4;
                        break;
                    case 4:
                        intscdBandNo = 5;
                        break;
                    case 5:
                        intscdBandNo = 6;
                        break;
                    case 6:
                        intscdBandNo = 7;
                        break;
                    case 7:
                        intscdBandNo = 8;
                        break;
                }
                return intscdBandNo;
            }
            convAntInfo.scdBandNo1 = _convertScdBandNo(antInfo.scdBandNo0);
            convAntInfo.scdBandNo2 = _convertScdBandNo(antInfo.scdBandNo1);
            convAntInfo.scdBandNo3 = _convertScdBandNo(antInfo.scdBandNo2);
            convAntInfo.scdBandNo4 = _convertScdBandNo(antInfo.scdBandNo3);
            convAntInfo.scdBandFreq1 = antInfo.scdBandFreq0;
            convAntInfo.scdBandFreq2 = antInfo.scdBandFreq1;
            convAntInfo.scdBandFreq3 = antInfo.scdBandFreq2;
            convAntInfo.scdBandFreq4 = antInfo.scdBandFreq3;
            return convAntInfo;
        };
        cDLCHANNEL_SEARCH.prototype._SatNameConverter = function (name) {
            var convName = null;
            var satelNameIndex = 0;
            switch (name) {
                case 'NONE':
                    convName = hx.l('LOC_NONE_ID');
                    satelNameIndex = 0;
                    break;
                case hx.l('LOC_NONE_ID'):
                    convName = 'NONE';
                    satelNameIndex = 0;
                    break;
                case 'AMOS_2&3':
                    convName = 'AMOS 2&amp;3 (4.0W) ';
                    satelNameIndex = 1;
                    break;
                case 'AMOS 2&amp;3 (4.0W) ':
                    convName = 'AMOS_2&3';
                    satelNameIndex = 1;
                    break;
                case 'APSTAR_2R':
                    convName = 'APSTAR 2R (76.5E) ';
                    satelNameIndex = 2;
                    break;
                case 'APSTAR 2R (76.5E) ':
                    convName = 'APSTAR_2R';
                    satelNameIndex = 2;
                    break;
                case 'ARABSAT_5A':
                    convName = 'ARABSAT 5A (30.5E) ';
                    satelNameIndex = 3;
                    break;
                case 'ARABSAT 5A (30.5E) ':
                    convName = 'ARABSAT_5A';
                    satelNameIndex = 3;
                    break;
                case 'ASIASAT_3S':
                    convName = 'ASIASAT 3S (105.5E) ';
                    satelNameIndex = 4;
                    break;
                case 'ASIASAT 3S (105.5E) ':
                    convName = 'ASIASAT_3S';
                    satelNameIndex = 4;
                    break;
                case 'ASIASAT_5':
                    convName = 'ASIASAT 5 (100.5E) ';
                    satelNameIndex = 5;
                    break;
                case 'ASIASAT 5 (100.5E) ':
                    convName = 'ASIASAT_5';
                    satelNameIndex = 5;
                    break;
                case 'ASTRA_1':
                    convName = 'ASTRA 1 (19.2E) ';
                    satelNameIndex = 6;
                    break;
                case 'ASTRA 1 (19.2E) ':
                    convName = 'ASTRA_1';
                    satelNameIndex = 6;
                    break;
                case 'ASTRA_2':
                    convName = 'ASTRA 2 (28.2E) ';
                    satelNameIndex = 7;
                    break;
                case 'ASTRA 2 (28.2E) ':
                    convName = 'ASTRA_2';
                    satelNameIndex = 7;
                    break;
                case 'ASTRA_3A&3B':
                    convName = 'ASTRA 3A&amp;3B (23.5E) ';
                    satelNameIndex = 8;
                    break;
                case 'ASTRA 3A&amp;3B (23.5E) ':
                    convName = 'ASTRA_3A&3B';
                    satelNameIndex = 8;
                    break;
                case 'ASTRA_4A':
                    convName = 'ASTRA 4A (5.0E) ';
                    satelNameIndex = 9;
                    break;
                case 'ASTRA 4A (5.0E) ':
                    convName = 'ASTRA_4A';
                    satelNameIndex = 9;
                    break;
                case 'ATLANTIC_BIRD_1':
                    convName = 'ATLANTIC BIRD 1 (12.5W) ';
                    satelNameIndex = 10;
                    break;
                case 'ATLANTIC BIRD 1 (12.5W) ':
                    convName = 'ATLANTIC_BIRD_1';
                    satelNameIndex = 10;
                    break;
                case 'ATLANTIC_BIRD_2':
                    convName = 'ATLANTIC BIRD 2 (8.0W) ';
                    satelNameIndex = 11;
                    break;
                case 'ATLANTIC BIRD 2 (8.0W) ':
                    convName = 'ATLANTIC_BIRD_2';
                    satelNameIndex = 11;
                    break;
                case 'ATLANTIC_BIRD_3':
                    convName = 'ATLANTIC BIRD 3 (5.0W) ';
                    satelNameIndex = 12;
                    break;
                case 'ATLANTIC BIRD 3 (5.0W) ':
                    convName = 'ATLANTIC_BIRD_3';
                    satelNameIndex = 12;
                    break;
                case 'BADR_4&6':
                    convName = 'BADR 4&amp;6 (26.0E) ';
                    satelNameIndex = 13;
                    break;
                case 'BADR 4&amp;6 (26.0E) ':
                    convName = 'BADR_4&6';
                    satelNameIndex = 13;
                    break;
                case 'EUROBIRD_1':
                    convName = 'EUROBIRD 1 (28.2E) ';
                    satelNameIndex = 14;
                    break;
                case 'EUROBIRD 1 (28.2E) ':
                    convName = 'EUROBIRD_1';
                    satelNameIndex = 14;
                    break;
                case 'EUROBIRD_3':
                    convName = 'EUROBIRD 3 (33.0E) ';
                    satelNameIndex = 15;
                    break;
                case 'EUROBIRD 3 (33.0E) ':
                    convName = 'EUROBIRD_3';
                    satelNameIndex = 15;
                    break;
                case 'EUROBIRD_9A':
                    convName = 'EUROBIRD 9A (9.0E) ';
                    satelNameIndex = 16;
                    break;
                case 'EUROBIRD 9A (9.0E) ':
                    convName = 'EUROBIRD_9A';
                    satelNameIndex = 16;
                    break;
                case 'EUTELSAT_W2A':
                    convName = 'EUTELSAT W2A (10.0E) ';
                    satelNameIndex = 17;
                    break;
                case 'EUTELSAT W2A (10.0E) ':
                    convName = 'EUTELSAT_W2A';
                    satelNameIndex = 17;
                    break;
                case 'EUTELSAT_W3A':
                    convName = 'EUTELSAT W3A (7.0E) ';
                    satelNameIndex = 18;
                    break;
                case 'EUTELSAT W3A (7.0E) ':
                    convName = 'EUTELSAT_W3A';
                    satelNameIndex = 18;
                    break;
                case 'EUTELSAT_W4&W7':
                    convName = 'EUTELSAT W4&amp;W7 (36.0E) ';
                    satelNameIndex = 19;
                    break;
                case 'EUTELSAT W4&amp;W7 (36.0E) ':
                    convName = 'EUTELSAT_W4&W7';
                    satelNameIndex = 19;
                    break;
                case 'EUTELSAT_W6':
                    convName = 'EUTELSAT W6 (21.6E) ';
                    satelNameIndex = 20;
                    break;
                case 'EUTELSAT W6 (21.6E) ':
                    convName = 'EUTELSAT_W6';
                    satelNameIndex = 20;
                    break;
                case 'EXPRESS_AM1':
                    convName = 'EXPRESS AM1 (40.0E) ';
                    satelNameIndex = 21;
                    break;
                case 'EXPRESS AM1 (40.0E) ':
                    convName = 'EXPRESS_AM1';
                    satelNameIndex = 21;
                    break;
                case 'EXPRESS_AM22':
                    convName = 'EXPRESS AM22 (53.0E) ';
                    satelNameIndex = 22;
                    break;
                case 'EXPRESS AM22 (53.0E) ':
                    convName = 'EXPRESS_AM22';
                    satelNameIndex = 22;
                    break;
                case 'HELLAS_SAT2':
                    convName = 'HELLAS SAT2 (39.0E) ';
                    satelNameIndex = 23;
                    break;
                case 'HELLAS SAT2 (39.0E) ':
                    convName = 'HELLAS_SAT2';
                    satelNameIndex = 23;
                    break;
                case 'HISPASAT_1C&1D':
                    convName = 'HISPASAT 1C&amp;1D (30.0W) ';
                    satelNameIndex = 24;
                    break;
                case 'HISPASAT 1C&amp;1D (30.0W) ':
                    convName = 'HISPASAT_1C&1D';
                    satelNameIndex = 24;
                    break;
                case 'HOTBIRD':
                    convName = 'HOTBIRD (13.0E) ';
                    satelNameIndex = 25;
                    break;
                case 'HOTBIRD (13.0E) ':
                    convName = 'HOTBIRD';
                    satelNameIndex = 25;
                    break;
                case 'INSAT_3A&4B':
                    convName = 'INSAT 3A&amp;4B (93.5E) ';
                    satelNameIndex = 26;
                    break;
                case 'INSAT 3A&amp;4B (93.5E) ':
                    convName = 'INSAT_3A&4B';
                    satelNameIndex = 26;
                    break;
                case 'INTELSAT_10-02':
                    convName = 'INTELSAT 10-02 (1.0W) ';
                    satelNameIndex = 27;
                    break;
                case 'INTELSAT 10-02 (1.0W) ':
                    convName = 'INTELSAT_10-02';
                    satelNameIndex = 27;
                    break;
                case 'INTELSAT_12':
                    convName = 'INTELSAT 12 (45.0E) ';
                    satelNameIndex = 28;
                    break;
                case 'INTELSAT 12 (45.0E) ':
                    convName = 'INTELSAT_12';
                    satelNameIndex = 28;
                    break;
                case 'INTELSAT_7&10':
                    convName = 'INTELSAT 7&amp;10 (68.5E) ';
                    satelNameIndex = 29;
                    break;
                case 'INTELSAT 7&amp;10 (68.5E) ':
                    convName = 'INTELSAT_7&10';
                    satelNameIndex = 29;
                    break;
                case 'INTELSAT_901':
                    convName = 'INTELSAT 901 (18.0W) ';
                    satelNameIndex = 30;
                    break;
                case 'INTELSAT 901 (18.0W) ':
                    convName = 'INTELSAT_901';
                    satelNameIndex = 30;
                    break;
                case 'INTELSAT_902':
                    convName = 'INTELSAT 902 (62.0E) ';
                    satelNameIndex = 31;
                    break;
                case 'INTELSAT 902 (62.0E) ':
                    convName = 'INTELSAT_902';
                    satelNameIndex = 31;
                    break;
                case 'INTELSAT_904':
                    convName = 'INTELSAT 904 (60.0E) ';
                    satelNameIndex = 32;
                    break;
                case 'INTELSAT 904 (60.0E) ':
                    convName = 'INTELSAT_904';
                    satelNameIndex = 32;
                    break;
                case 'INTELSAT_906':
                    convName = 'INTELSAT 906 (64.2E) ';
                    satelNameIndex = 33;
                    break;
                case 'INTELSAT 906 (64.2E) ':
                    convName = 'INTELSAT_906';
                    satelNameIndex = 33;
                    break;
                case 'INTELSAT_907':
                    convName = 'INTELSAT 907 (27.5W) ';
                    satelNameIndex = 34;
                    break;
                case 'INTELSAT 907 (27.5W) ':
                    convName = 'INTELSAT_907';
                    satelNameIndex = 34;
                    break;
                case 'NILESAT_101&102':
                    convName = 'NILESAT 101&amp;102 (7.0W) ';
                    satelNameIndex = 35;
                    break;
                case 'NILESAT 101&amp;102 (7.0W) ':
                    convName = 'NILESAT_101&102';
                    satelNameIndex = 35;
                    break;
                case 'NSS_12':
                    convName = 'NSS 12 (57.0E) ';
                    satelNameIndex = 36;
                    break;
                case 'NSS 12 (57.0E) ':
                    convName = 'NSS_12';
                    satelNameIndex = 36;
                    break;
                case 'NSS_7':
                    convName = 'NSS 7 (22.0W) ';
                    satelNameIndex = 37;
                    break;
                case 'NSS 7 (22.0W) ':
                    convName = 'NSS_7';
                    satelNameIndex = 37;
                    break;
                case 'TELSTAR_12':
                    convName = 'TELSTAR 12 (15.0W) ';
                    satelNameIndex = 38;
                    break;
                case 'TELSTAR 12 (15.0W) ':
                    convName = 'TELSTAR_12';
                    satelNameIndex = 38;
                    break;
                case 'THOR_5&6':
                    convName = 'THOR 5&amp;6 (1.0W) ';
                    satelNameIndex = 39;
                    break;
                case 'THOR 5&amp;6 (1.0W) ':
                    convName = 'THOR_5&6';
                    satelNameIndex = 39;
                    break;
                case 'TURKSAT_2A&3A':
                    convName = 'TURKSAT 2A&amp;3A (42.0E) ';
                    satelNameIndex = 40;
                    break;
                case 'TURKSAT 2A&amp;3A (42.0E) ':
                    convName = 'TURKSAT_2A&3A';
                    satelNameIndex = 40;
                    break;
                case 'YAMAL_201':
                    convName = 'YAMAL 201 (90.0E) ';
                    satelNameIndex = 41;
                    break;
                case 'YAMAL 201 (90.0E) ':
                    convName = 'YAMAL_201';
                    satelNameIndex = 41;
                    break;
                case 'ARABSAT_2B':
                    convName = 'ARABSAT 2B (30.5E) ';
                    satelNameIndex = 42;
                    break;
                case 'ARABSAT 2B (30.5E) ':
                    convName = 'ARABSAT_2B';
                    satelNameIndex = 42;
                    break;
                case 'EUTELSAT_W1':
                    convName = 'EUTELSAT W1 (10.0E) ';
                    satelNameIndex = 43;
                    break;
                case 'EUTELSAT W1 (10.0E) ':
                    convName = 'EUTELSAT_W1';
                    satelNameIndex = 43;
                    break;
                case 'EUTELSAT_W2':
                    convName = 'EUTELSAT W2 (16.0E) ';
                    satelNameIndex = 44;
                    break;
                case 'EUTELSAT W2 (16.0E) ':
                    convName = 'EUTELSAT_W2';
                    satelNameIndex = 44;
                    break;
                case 'NSS_703':
                    convName = 'NSS 703 (57.0E) ';
                    satelNameIndex = 45;
                    break;
                case 'NSS 703 (57.0E) ':
                    convName = 'NSS_703';
                    satelNameIndex = 45;
                    break;
                case 'SIRIUS':
                    convName = 'SIRIUS (5.0E) ';
                    satelNameIndex = 46;
                    break;
                case 'SIRIUS (5.0E) ':
                    convName = 'SIRIUS';
                    satelNameIndex = 46;
                    break;
                case 'TELECOM_2D':
                    convName = 'TELECOM 2D (8.0W) ';
                    satelNameIndex = 47;
                    break;
                case 'TELECOM 2D (8.0W) ':
                    convName = 'TELECOM_2D';
                    satelNameIndex = 47;
                    break;
                case 'EUROBIRD_9':
                    convName = 'EUROBIRD 9 (9.0E) ';
                    satelNameIndex = 48;
                    break;
                case 'EUROBIRD 9 (9.0E) ':
                    convName = 'EUROBIRD_9';
                    satelNameIndex = 48;
                    break;
                case 'INTELSAT_802':
                    convName = 'INTELSAT 802 (33.0E) ';
                    satelNameIndex = 49;
                    break;
                case 'INTELSAT 802 (33.0E) ':
                    convName = 'INTELSAT_802';
                    satelNameIndex = 49;
                    break;
                case 'THOR_3_5':
                    convName = 'THOR 3 5 (1.0W) ';
                    satelNameIndex = 50;
                    break;
                case 'THOR 3 5 (1.0W) ':
                    convName = 'THOR_3_5';
                    satelNameIndex = 50;
                    break;
                case 'INSAT_2E4A':
                    convName = 'INSAT 2E4A (83.0E) ';
                    satelNameIndex = 51;
                    break;
                case 'INSAT 2E4A (83.0E) ':
                    convName = 'INSAT_2E4A';
                    satelNameIndex = 51;
                    break;
                case 'INTELSAT_17':
                    convName = 'INTELSAT 17 (66.0E) ';
                    satelNameIndex = 52;
                    break;
                case 'INTELSAT 17 (66.0E) ':
                    convName = 'INTELSAT_17';
                    satelNameIndex = 52;
                    break;
                case 'YAHLIVE':
                    convName = 'YAHLIVE (52.5E) ';
                    satelNameIndex = 53;
                    break;
                case 'YAHLIVE (52.5E) ':
                    convName = 'YAHLIVE';
                    satelNameIndex = 53;
                    break;
                case 'User Define':
                    convName = hx.l('LOC_USERDEFINE_ID') + ' ';
                    satelNameIndex = 54;
                    break;
                case hx.l('LOC_USERDEFINE_ID') + ' ':
                    convName = 'User Define';
                    satelNameIndex = 54;
                    break;
                case 'User Define 1':
                    convName = hx.l('LOC_USERDEFINE_1_ID') + ' ';
                    satelNameIndex = 54;
                    break;
                case hx.l('LOC_USERDEFINE_1_ID') + ' ':
                    convName = 'User Define 1';
                    satelNameIndex = 54;
                    break;
                case 'User Define 2':
                    convName = hx.l('LOC_USERDEFINE_2_ID') + ' ';
                    satelNameIndex = 55;
                    break;
                case hx.l('LOC_USERDEFINE_2_ID') + ' ':
                    convName = 'User Define 2';
                    satelNameIndex = 55;
                    break;
                case 'User Define 3':
                    convName = hx.l('LOC_USERDEFINE_3_ID') + ' ';
                    satelNameIndex = 56;
                    break;
                case hx.l('LOC_USERDEFINE_3_ID') + ' ':
                    convName = 'User Define 3';
                    satelNameIndex = 56;
                    break;
                case 'User Define 4':
                    convName = hx.l('LOC_USERDEFINE_4_ID') + ' ';
                    satelNameIndex = 57;
                    break;
                case hx.l('LOC_USERDEFINE_4_ID') + ' ':
                    convName = 'User Define 4';
                    satelNameIndex = 57;
                    break;
                case 'User Define 5':
                    convName = hx.l('LOC_USERDEFINE_5_ID') + ' ';
                    satelNameIndex = 58;
                    break;
                case hx.l('LOC_USERDEFINE_5_ID') + ' ':
                    convName = 'User Define 5';
                    satelNameIndex = 58;
                    break;
            }
            return {
                convName: convName,
                index: satelNameIndex
            };
        };
        cDLCHANNEL_SEARCH.prototype._initAllAntennaInfo = function () {
            var len = 0, i, antInfoLists = this.oipfLocalSystemObj.antennaInfoLists, antInfo;
            antInfoLists.refresh();
            len = antInfoLists.length;
            for (i = 0; i < len; i += 1) {
                antInfo = antInfoLists.item(i);
                this._setAntennaInfoWithMenu(antInfo, this._antennaSetJson[0], this._searchSatelliteName[0], this._userlnbFreq[0], this._tuner1Param, this._tuner2Param, this._tuner3Param, this._tuner4Param, this._motorSetJson);
            }
        };
        /////////////////////////// Manage Satellite END! //////////////////////////////////////////////
        ////////////////////////// Motor Channel Search START! ////////////////////////////////////////
        cDLCHANNEL_SEARCH.prototype._motorDisableLimit = function () {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            antInfoLists.disableLimit();
            alert('disableLimit');
        };
        cDLCHANNEL_SEARCH.prototype._motorEnableLimit = function () {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            antInfoLists.enableLimit();
            alert('enableLimit');
        };
        cDLCHANNEL_SEARCH.prototype._motorDrive = function (param) {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            var direction, mode, step;
            switch (param.direction) {
                case 'East':
                    direction = antInfoLists.EAST;
                    break;
                case 'West':
                    direction = antInfoLists.WEST;
                    break;
            }
            switch (param.mode) {
                case 'Continuous':
                    mode = antInfoLists.CONTINUOUS;
                    break;
                case 'Step':
                    mode = antInfoLists.STEP;
                    break;
                case 'Time':
                    mode = antInfoLists.TIMEOUT;
                    break;
            }
            step = param.step;
            antInfoLists.drive(direction, mode, step);
            alert('drive - direction : ' + direction + ', mode : ' + mode + ', step : ' + step);
        };
        cDLCHANNEL_SEARCH.prototype._motorStop = function () {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            antInfoLists.stop();
            alert('motorstop');
        };
        cDLCHANNEL_SEARCH.prototype._motorGotoPosition = function (param) {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            var antInfo = antInfoLists.getAntennaInfo(param.antId);
            var position = antInfo.position;
            antInfoLists.gotoPosition(position);
            alert('gotoPosition - position : ' + position);
        };
        cDLCHANNEL_SEARCH.prototype._motorStorePosition = function (param) {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            var antInfo = antInfoLists.getAntennaInfo(param.antId);
            var position = antInfo.position;
            antInfoLists.storePosition(position);
            alert('storePosition');
        };
        cDLCHANNEL_SEARCH.prototype._motorSetLimit = function (param) {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            antInfoLists.setLimit(param.direction);
            alert('setLimit : ' + param.direction);
        };
        cDLCHANNEL_SEARCH.prototype._motorGotoSatellite = function (param) {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            var antInfo = antInfoLists.getAntennaInfo(param.antId);
            var longitude = antInfo.longitude;
            antInfoLists.gotoSatellite(longitude);
            alert('gotoSatellite - longitude :  ' + longitude);
        };
        cDLCHANNEL_SEARCH.prototype._motorRecalculate = function (param) {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            var antInfo = antInfoLists.getAntennaInfo(param.antId);
            var position = antInfo.position;
            antInfoLists.recalculate(position);
            alert('recalculate : ' + position);
        };
        cDLCHANNEL_SEARCH.prototype._motorGetMyPosition = function () {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            var myLongitude = antInfoLists.getMyLongitude();
            var myLatitude = antInfoLists.getMyLatitude();
            return {
                myLongitude: myLongitude,
                myLatitude: myLatitude
            };
        };
        cDLCHANNEL_SEARCH.prototype._motorSetMyPosition = function (param) {
            var antInfoLists = this.oipfLocalSystemObj.antennaInfoLists;
            var myLatitude = param.myLatitude * 10;
            var myLongitude = param.myLongitude * 10;
            if (param.ewDirection === 'West') {
                myLongitude *= -1;
            }
            if (param.nsDirection === 'South') {
                myLongitude *= -1;
            }
            antInfoLists.setMyLatitude(myLatitude);
            antInfoLists.setMyLongitude(myLongitude);
            ////////////////////////// Motor Channel Search END ////////////////////////////////////////
        };
        return cDLCHANNEL_SEARCH;
    })(__KERNEL__);
    return cDLCHANNEL_SEARCH;
});
