/// <reference path="../../def/sugar.d.ts" />
// file name : layer_domain/service/dl_dsrvChannelSearchS.js
/*jslint sloppy:true, nomen:true, vars:true */
//    CHANNEL_SEARCH_S: ['layer_domain/service/dl_dsrvChannelSearchS.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

class cDLCHANNEL_SEARCH_S extends __KERNEL__ implements ifSVCChannelSearchS {
    //**************** Public Method ***********************//
    /* Get & Set SearchOption */
    /* Channel Scan Control */
    /* Manage Satellite */
    /* Manage Transponder List */
    /* SCD Channel Search */

    //***************** Private Method **********************//
    /* Initialize */
    /* Save to WebStorage & get from WebStorage */
    /* Channel Scan Control */
    /* Manage Channel Scan Parameter */
    /* Manage Satellite */

    private lcnScanType:any = {
        UPDATE: 0,
        CHANGE: 1
    };
    ////////////// Defined Data ///////////////////
    private _MEdefaultAntName:string = 'HOTBIRD';
    _defaultAntName:string = 'ASTRA_1';
    _dfaultEmptyAntName:string = 'NONE';
    SATELLITE_NUMBER_TO_MAKE:number = 4;

    ////////////////////// srv cashed Data /////////////////////
    private _transponderList:any = {};
    private _satPosDataList:any = {};
    private _searchOptions:any = {
        network: 'true',
        scramble: 'All',
        channel: 'All'
    };
    private _curUsingSatelliteIdx:number = 0;
    private _curSettingTpIdx:number = 0;
    private _antlength:number = 0;
    private _isStartScan:boolean = false;
    private curLcnScanType:number = 0;
    private _TestSatelliteName:string = 'NONE';
    private _testAntennaId:any = 0;
    private _testAntennaSetJson:any = {
        searchType: 0,
        switchInput: 0,
        lnbFreq: 0,
        toneEnable: 0,
        diseqcVersion: 0
    };
    private _testtsIndex:number = 0;
    private _userTestlnbFreq:any = 0;
    private _userTestTPParam:any = {
        userFreq: '0',
        userPola: 'auto',
        userSymbol: '27500',
        userTrans: 'DVB',
        userFEC: 'auto'
    };
    private _userTempTPParam:any = {
        userFreq: '0',
        userPola: 'auto',
        userSymbol: '27500',
        userTrans: 'DVB',
        userFEC: 'auto'
    };
    private _searchSatelliteName:any = [];
    private _searchAntennaId:any = [];
    private _antennaSetJson:any = [];
    private _tsIndex:any = [];
    private _userlnbFreq:any = [];
    private _userTPParam:any = [];

    //scd tuner param
    private _lastSettingTuner:number = 1;
    private _tuner1Param:any = {
        scdBandNo: 1,
        scdBandFreq: 1210
    };
    private _tuner2Param:any = {
        scdBandNo: 2,
        scdBandFreq: 1400
    };
    private _tuner3Param:any = {
        scdBandNo: 3,
        scdBandFreq: 1516
    };
    private _tuner4Param:any = {
        scdBandNo: 4,
        scdBandFreq: 1632
    };
    ////////////////////// srv Data END /////////////////////

    private oipfChannelConfigObj : any;
    private oipfMastConfigurationObj : any;
    private oipfLocalSystemObj : any;
    private oipfHLcnUpdate : any;

    HXAntennaType : any = {
        ETypeLnb : 0,
        ETypeDiseqc : 1,
        ETypeScd : 2,
        ETypeMotor : 3
    };

    constructor(aServiceName: string) {
        super(aServiceName);
        this.defineEvents(['channelScan', 'diseqcScanResult', 'antennaConnection', 'startLcnListUpdate', 'startLcnListScan', 'saveLcnUpdate', 'saveLcnChannels', 'getLcnInfo']);
        this.init();
    }

    init() {
        this.oipfChannelConfigObj = hx.stbObject.channelConfig;
        this.oipfMastConfigurationObj = hx.stbObject.configuration;
        this.oipfLocalSystemObj = this.oipfMastConfigurationObj.localSystem;
        if (hx.config.useLcnUpdate) {
            this.oipfHLcnUpdate = this.oipfLocalSystemObj.lcnupdate;
        }
        this._initializeAntData();
        this._checkAntennaInformationWithSystemAntennaInformation(); // Used this function for channel import/export model
        this._loadTpData();
        this._loadSatPosData();
        this._getSearchOptionsFromLocal();
        this._getTpIdxListFromLocal();
        this._getTpParamFromLocal();
        this.getAntIdListFromLocal();
        this.antennaSetting();
        //hx.logadd('dsrvChannelSearch');
        hx.log('dsrvChannelSearch', 'create');
    }

    // ####################################################################################################
    // Public Methods
    //////////////////////////*  Get & Set SearchOption START *//////////////////////////
    setSearchOptionParam(param) {
        this._searchOptions.network = param.network;
        this._searchOptions.scramble = param.scramble;
        this._searchOptions.channel = param.channel;
        this._saveSearchOptionsToLocal();
    }

    getSearchOptionParam() {
        return {
            network: this._searchOptions.network,
            scramble: this._searchOptions.scramble,
            channel: this._searchOptions.channel
        };
    }

    //////////////////////////*  Get & Set SearchOption END *//////////////////////////

    /////////////////////////*   Channel Scan Control START *//////////////////////////////////////
    StartChannelScan() {
        var that = this;
        var fn = function (type, progress, frequency, signalStrength, channelNumber, channelType, channelCount, transponderCount, owSearchedInfos, tpInfo) {
            that.fireEvent('channelScan', {
                progress: progress,
                channelNumber: channelNumber,
                channelCount: channelCount,
                transponderCount: transponderCount,
                signalStrength: signalStrength,
                tpInfo: tpInfo,
                owSearchedInfos: owSearchedInfos
            });
        };

        var _antId = [parseInt(that._searchAntennaId[0], 10), parseInt(that._searchAntennaId[1], 10), parseInt(that._searchAntennaId[2], 10), parseInt(that._searchAntennaId[3], 10), parseInt(that._searchAntennaId[4], 10), parseInt(that._searchAntennaId[5], 10), parseInt(that._searchAntennaId[6], 10), parseInt(that._searchAntennaId[7], 10)];
        that._startScan({
            satName: that._searchSatelliteName,
            antId: _antId,
            tsIndex: that._tsIndex,
            searchOptions: that._searchOptions,
            fn: fn,
            usrParam: that._userTPParam,
            isStartScan: that._isStartScan
        });
    }

    startDiseqcScan() {
        var that = this;
        var callBack = function (type, progress, frequency, signalStrength, channelNumber, channelType, channelCount, transponderCount, owSearchedInfos, tpInfo) {
            hx.logadd('DiseqcScan');
            hx.log('DiseqcScan', 'type : ' + type);
            hx.log('DiseqcScan', 'progress : ' + progress);
            hx.log('DiseqcScan', 'channelCount : ' + channelCount);
            if (progress === 100) {
                var scanData = that._disecScanCompleted();
                if (channelCount === 1) {//scan success
                    hx.log('DiseqcScan', 'scanData - true ');
                    if (ENV.op === ENV.listOP.MIDDLE_EAST) {
                        var noWork;
                    } else {
                        that._save_disecScanCompletedAntenna(scanData);
                    }
                    that.fireEvent('diseqcScanResult', {
                        'scanSuccess': true,
                        'scanData': scanData
                    });
                } else {
                    hx.log('DiseqcScan', 'scanData - false ');
                    that.fireEvent('diseqcScanResult', {
                        'scanSuccess': false,
                        'scanData': scanData
                    });
                }
            }
        };
        hx.log('dsrvChannelSearch', 'startDiseqcScan() called!!!');

        var scanParamCollection = that._createChannelScanParamCollectionObject();
        var scanAntName = '';
        var _set_channel_param = function (sr, freq, coderate, polar, modul, trans) {
            var channelScanParams = that.oipfChannelConfigObj.createChannelScanParametersObject(11);
            hx.log('dsrvChannelSearch', 'scanParamCollection() called!!!');
            channelScanParams.symbolRate = sr;
            channelScanParams.startFrequency = freq;
            channelScanParams.endFrequency = freq;
            if (ENV.op === ENV.listOP.MIDDLE_EAST) {
                channelScanParams.antennaId = that.getAntennaIdByName(that._MEdefaultAntName);
                channelScanParams.antennaName = that._MEdefaultAntName;
            } else {
                scanAntName = that._searchSatelliteName[that._curUsingSatelliteIdx];
                channelScanParams.antennaId = that.getAntennaIdByName(scanAntName);
                channelScanParams.antennaName = scanAntName;
            }
            channelScanParams.codeRate = coderate;
            channelScanParams.polarisation = polar;
            channelScanParams.modulationModes = modul;
            if (trans === 0) {
                channelScanParams.modulationModes = 0;
            } else {
                channelScanParams.modulationModes = modul;
            }
            scanParamCollection.addChannelScanParam(channelScanParams);
        };

        var data;
        if (ENV.listModel.IR4000HD === ENV.model) {
            var szAntenaName = '', antenaInfo;
            szAntenaName = that._MEdefaultAntName;
            data = that._getTuningInfo(szAntenaName, 1);
        } else {
            var curAntName = '';
            curAntName = that._searchSatelliteName[that._curUsingSatelliteIdx];
            data = that._getTuningInfo(curAntName, 1);
        }

        var i, length = data.length;
        for (i = 0; i < length; i += 1) {
            _set_channel_param(data[i][1].toString(), data[i][0], data[i][3], data[i][2], data[i][4], data[i][5]);
        }
        that.oipfChannelConfigObj.onChannelScan = callBack;
        that.oipfChannelConfigObj.startScan(null, scanParamCollection, 'diseqcdetection');
        that._isStartScan = true;
    }

    getstartDiseqcScanAntennaListCount() {
        var that = this, antInfolist, len = 0;

        antInfolist = that._getAllAntennaInfo();
        len = antInfolist.length;
        return len;
    }

    getstartDiseqcScanAntennaInfo(data) {
        var that = this;
        var index = data.index, antInfolist, antInfo, convAntInfo;

        antInfolist = that._getAllAntennaInfo();
        if (antInfolist.length > 0) {
            antInfo = antInfolist.item(index);
            convAntInfo = that._convertAntennaData(antInfo);

            return convAntInfo;
        }
        return null;
    }

    startAntennaConnection(data) {
        var that = this, i;
        hx.logadd('antCon');
        for (i = 0; i < data.lockingDataList.length; i += 1) {
            hx.log('antCon', 'lockingTPList - Index : ' + i + ' - antId : ' + data.lockingDataList[i].antId);
            hx.log('antCon', 'lockingTPList - Index : ' + i + ' - tpData : ' + data.lockingDataList[i].tpData[0] + ', ' + data.lockingDataList[i].tpData[1] + ', ' + data.lockingDataList[i].tpData[2] + ', ' + data.lockingDataList[i].tpData[3] + ', ' + data.lockingDataList[i].tpData[4] + ', ' + data.lockingDataList[i].tpData[5] + ', ' + data.lockingDataList[i].tpData[6]);
        }

        var fn = function (type, progress, frequency, signalStrength, channelNumber, channelType, channelCount, transponderCount, owSearchedInfos, tpInfo) {
            that.fireEvent('antennaConnection', {
                type: type,
                progress: progress,
                connectionType: channelCount
            });
        };
        var _antId = [parseInt(that._searchAntennaId[0], 10), parseInt(that._searchAntennaId[1], 10), parseInt(that._searchAntennaId[2], 10), parseInt(that._searchAntennaId[3], 10), parseInt(that._searchAntennaId[4], 10), parseInt(that._searchAntennaId[5], 10), parseInt(that._searchAntennaId[6], 10), parseInt(that._searchAntennaId[7], 10)];

        that._startAntennaConnection({
            satName: that._searchSatelliteName,
            antId: _antId,
            fn: fn,
            lockingDataList: data.lockingDataList,
            isStartScan: that._isStartScan
        });
    }

    antennaConnectionComplete(data) {
        var that = this;
        var antInfoLists = that.oipfLocalSystemObj.antennaInfoLists;

        that.oipfChannelConfigObj.stopScan();
        that._isStartScan = false;
        antInfoLists.setConnectionType(data.connectionType);
    }

    StartTune(data) {
        var that = this, tsIndexNum = 0, tpParam = {};
        if (that._TestSatelliteName === 'NONE') {
            return;
        }

        if (data) {
            if (data.isTpSetPage) {
                tsIndexNum = 0;
                tpParam = that._userTempTPParam;
            } else {
                tsIndexNum = that._testtsIndex;
                tpParam = that._userTestTPParam;
            }
        } else {
            tsIndexNum = that._testtsIndex;
            tpParam = that._userTestTPParam;
        }

        that._startTune({
            satName: that._TestSatelliteName,
            antId: parseInt(that._testAntennaId, 10),
            tsIndex: tsIndexNum,
            usrParam: tpParam,
            isStartScan: that._isStartScan
        });
    }

    StartTuneWithNotTestAntenna(data) {
        var that = this, satelliteName = '', antId = 0, tsIndexNum = 0, tpParam = {};

        satelliteName = that._searchSatelliteName[that._curSettingTpIdx];
        antId = that._searchAntennaId[that._curSettingTpIdx];
        if (satelliteName === 'NONE') {
            return;
        }

        if (data) {
            if (data.isTpSetPage) {
                tsIndexNum = 0;
                tpParam = that._userTempTPParam;
            } else {
                tsIndexNum = that._testtsIndex;
                tpParam = that._userTestTPParam;
            }
        } else {
            tsIndexNum = that._testtsIndex;
            tpParam = that._userTestTPParam;
        }

        that._startTune({
            satName: satelliteName,
            antId: parseInt(antId + "", 10),
            tsIndex: tsIndexNum,
            usrParam: tpParam,
            isStartScan: that._isStartScan
        });
    }

    loadChannelSearchResult() {
        var that = this;
        that.oipfChannelConfigObj.controlScan('load');
    }

    loadChannelSearchResultWithDefCh() {
        var that = this;
        that.oipfChannelConfigObj.controlScan('loadDefChannels');
        that.oipfChannelConfigObj.controlScan('load');
    }

    saveChannelSearchResult() {
        var that = this;
        that.oipfChannelConfigObj.controlScan('save');
        that.oipfChannelConfigObj.stopScan();
        that._isStartScan = false;
    }

    stopScan() {
        var that = this;
        that.oipfChannelConfigObj.stopScan();
        that._isStartScan = false;
    }

    pauseScan() {
        var that = this;
        that.oipfChannelConfigObj.controlScan('pause');
    }

    resumeScan() {
        var that = this;
        that.oipfChannelConfigObj.controlScan('resume');
    }

    updateLcnChannelList() {
        var that = this;
        hx.logadd('updateLcnChannelList');
        that.curLcnScanType = that.lcnScanType.UPDATE;
        that._updateChannelList();
    }

    changeLcnChannelList() {//temporary code
        var that = this;
        hx.logadd('changeLcnChannelList');
        that.curLcnScanType = that.lcnScanType.CHANGE;
        that._changeChannelList();
    }

    stopLcnUpdate() {
        var that = this;
        hx.logadd('stopLcnUpdate');
        hx.log('stopLcnUpdate', 'call oipfHLcnUpdate.stopLcnUpdate()');
        that.oipfHLcnUpdate.stopLcnUpdate();
    }

    saveLcnUpdate(param) {
        var that = this;
        hx.logadd('saveLcnUpdate');
        that._saveLcnUpdate({
            searchMode: param.searchMode,
            index: param.index
        });
    }

    getCurrentLcnListInfo() {//temporary code
        var that = this;
        that._getLcnUpdateInfo();
    }

    removeCurrentLcnListInfo() {
        var that = this;
        that._removeLcnUpdateInfo();
    }

    /////////////////////////*   Channel Scan Control END *//////////////////////////////////////

    ////////////////////////*  Manage Satellite START  */////////////////////////////////////////
    getSearchTypeFromAnt() {//
        var that = this;
        if ((that._searchAntennaId[0] === 0) || (that._searchAntennaId[0] === null)) {
            return 0;
        }
        var antInfo = that._getAntennaInfoFromId(that._searchAntennaId[0]);

        if ((that.oipfLocalSystemObj.antennaInfoLists.length === 0) || (antInfo.satelliteName === 'NONE')) {
            return 0;
        }

        return antInfo.antennaType;
    }

    getAntIdListFromLocal() {
        var that = this, i;
        var antIdList;

        antIdList = hx.svc.WEB_STORAGE.getAntIdListFromLocal();
        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            var antId = antIdList['antId' + (i + 1)];
            if (antId && antId !== 'undefined') {
                that._searchAntennaId[i] = parseInt(antId, 10);
            }
        }
    }

    getSatelliteNameById(param) {
        var that = this, satName = '';
        var antInfo = that._getAntennaInfoFromId(param.antId);

        if (antInfo) {
            satName = antInfo.satelliteType;
            satName = that._SatNameConverter(satName).convName;
        } else {
            satName = 'Unknown';
        }
        return satName;
    }

    getAntennaIdByName(name) {
        var that = this, len, i, antInfoLists = that.oipfLocalSystemObj.antennaInfoLists, antInfo;

        antInfoLists.refresh();
        len = antInfoLists.length;
        for (i = 0; i < len; i += 1) {
            antInfo = antInfoLists.item(i);
            if (antInfo.satelliteType.toLowerCase() === name.toLowerCase()) {
                return antInfo.id;
            }
        }
        return 0;
    }

    setTestSatelliteName(param) {
        var that = this;
        var convName = that._SatNameConverter(param.satName).convName;
        that._TestSatelliteName = convName;
    }

    getTestSatelliteName() {
        var that = this;
        return {
            satName: that._SatNameConverter(that._TestSatelliteName).convName,
            satelNameIndex: that._SatNameConverter(that._TestSatelliteName).index
        };
    }

    setAntennaInfo(param) {
        var that = this;
        var antId = param.antId;
        var antInfo;
        var antLength = that.oipfLocalSystemObj.antennaInfoLists.length;
        hx.logadd('SetSatellite');
        hx.log('SetSatellite', '******subChannelSearch - setAntennaInfo before makedefault*******antennaInfoLists Length : ' + antLength);
        hx.log('SetSatellite', '******subChannelSearch - setAntennaInfo before makedefault*******antId : ' + antId);
        if ((param.antId === 0) || (param.antId === '0') || (param.antId === null) || (antLength === 0)) {
            antId = that._makeAntennaWithDefault(param.satName);
            hx.log('SetSatellite', '******subChannelSearch - _makeAntennaWithDefault *******antId : ' + antId);
        } else {
            hx.log('SetSatellite', '******subChannelSearch - Why??? *******antId : ' + antId);
        }
        antLength = that.oipfLocalSystemObj.antennaInfoLists.length;
        hx.log('SetSatellite', '******subChannelSearch - setAntennaInfo after makedefault*******antennaInfoLists Length : ' + antLength);
        hx.log('SetSatellite', '******subChannelSearch - setAntennaInfo after makedefault*******antId : ' + antId);
        antInfo = that._getAntennaInfoFromId(antId);

        that._setAntennaInfoWithMenu(antInfo, param.antSet, param.satName, param.userlnbFreq, param.tuner1Param, param.tuner2Param, param.tuner3Param, param.tuner4Param);
        return antId;
    }

    saveAntennaInfo(param) {
        var that = this, antId;
        hx.log('dsrvChannelSearch', 'saveAntennaInfo() called!!!');
        that._antennaSetJson[that._curUsingSatelliteIdx].searchType = param.searchType;
        if (param.searchType === 2) {//scd
            var scdPosition = 0;
            // positon A = 0, postion B = 1;
            if (that._curUsingSatelliteIdx > 0) {
                scdPosition = 1;
            }
            that._antennaSetJson[that._curUsingSatelliteIdx].switchInput = scdPosition;
        }
        antId = that.setAntennaInfo({
            satName: that._searchSatelliteName[that._curUsingSatelliteIdx],
            antSet: that._antennaSetJson[that._curUsingSatelliteIdx],
            antId: that._searchAntennaId[that._curUsingSatelliteIdx],
            userlnbFreq: that._userlnbFreq[that._curUsingSatelliteIdx],
            tuner1Param: that._tuner1Param,
            tuner2Param: that._tuner2Param,
            tuner3Param: that._tuner3Param,
            tuner4Param: that._tuner4Param
        });
        that._setAntennaId(antId, that._curUsingSatelliteIdx);
        that._saveAntIdToLocal(that._curUsingSatelliteIdx);
    }

    saveTestAntennaInfo(param) {
        var that = this;
        if (param.searchType !== undefined) {
            that._testAntennaSetJson.searchType = param.searchType;
        }
        if (param.switchInput !== undefined) {
            if (param.searchType === 2) { //scd
                var scdPosition = 0;
                // positon A = 0, postion B = 1;
                if (that._curUsingSatelliteIdx > 0) {
                    scdPosition = 1;
                }
                param.switchInput = scdPosition;
            }
            that._testAntennaSetJson.switchInput = param.switchInput;
        }
        if (param.lnbFreq !== undefined) {
            that._testAntennaSetJson.lnbFreq = param.lnbFreq;
        }
        if (param.toneEnable !== undefined) {
            that._testAntennaSetJson.toneEnable = param.toneEnable;
        }
        that._testAntennaId = that._searchAntennaId[that._curUsingSatelliteIdx];
        var antId = that.setAntennaInfo({
            satName: that._TestSatelliteName,
            antSet: that._testAntennaSetJson,
            antId: that._testAntennaId,
            userlnbFreq: that._userTestlnbFreq,
            tuner1Param: that._tuner1Param,
            tuner2Param: that._tuner2Param,
            tuner3Param: that._tuner3Param,
            tuner4Param: that._tuner4Param
        });
        that._setTestAntennaId(antId);
        that._searchAntennaId[that._curUsingSatelliteIdx] = antId;
        that._saveAntIdToLocal(that._curUsingSatelliteIdx);
    }

    removeAntennaList() {
        var that = this, i;
        hx.log('dsrvChannelSearch', 'removeAntennaList() called!!!');
        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            that._setAntennaId(0, i);
            that._saveAntIdToLocal(i);
            that._searchSatelliteName[i] = 'NONE';
            that._userlnbFreq[i] = 0;
            that._tsIndex[i] = 0;
            that._antennaSetJson[i] = {
                searchType: 0,
                switchInput: 0,
                lnbFreq: 0,
                toneEnable: 0,
                diseqcVersion: 0
            };
            that._userTPParam[i] = {
                userFreq: '0',
                userPola: 'auto',
                userSymbol: '27500',
                userTrans: 'DVB',
                userFEC: 'auto'
            };
            that._saveTpIdxToLocal(i);
            that._saveTpParamToLocal(i);
        }
        that._lastSettingTuner = 1;
        that._tuner1Param.scdBandNo = 1;
        that._tuner1Param.scdBandFreq = 1210;
        that._tuner2Param.scdBandNo = 2;
        that._tuner2Param.scdBandFreq = 1400;
        that._tuner3Param.scdBandNo = 3;
        that._tuner3Param.scdBandFreq = 1516;
        that._tuner4Param.scdBandNo = 4;
        that._tuner4Param.scdBandFreq = 1632;
        that._initAllAntennaInfo();
        that._removeAntennaList();
    }

    removeAnt() {
        var that = this;
        that._removeAnt(that._curUsingSatelliteIdx);
        that._calcAntlength();
    }

    updateNumSatelSetting(param) {
        var that = this;
        that._curUsingSatelliteIdx = param.numSat;
    }

    updateNumTpSetting(param) {
        var that = this;
        that._curSettingTpIdx = param.numTp;
    }

    setSatelliteSetting(param) {
        var that = this;

        hx.log('dsrvChannelSearch', 'setSatelliteSetting() called!!!');
        if (param.satName !== undefined) {
            that._searchSatelliteName[that._curUsingSatelliteIdx] = that._SatNameConverter(param.satName).convName;
        }
        if (param.switchInput !== undefined) {
            that._antennaSetJson[that._curUsingSatelliteIdx].switchInput = param.switchInput;
        }
        if (param.lnbFreq !== undefined) {
            that._antennaSetJson[that._curUsingSatelliteIdx].lnbFreq = param.lnbFreq;
        }
        if (param.toneEnable !== undefined) {
            that._antennaSetJson[that._curUsingSatelliteIdx].toneEnable = param.toneEnable;
        }
        that._calcAntlength();
    }

    getSatelliteSetting() {
        var that = this;
        var satName = that._SatNameConverter(that._searchSatelliteName[that._curUsingSatelliteIdx]);
        var antennaJson = null;
        antennaJson = that._antennaSetJson[that._curUsingSatelliteIdx];

        return {
            satName: satName.convName,
            satNameIndex: satName.index,
            switchInput: antennaJson.switchInput,
            lnbFreq: antennaJson.lnbFreq,
            toneEnable: antennaJson.toneEnable,
            diseqcVersion: antennaJson.diseqcVersion,
            userlnbFreq: that._userlnbFreq[that._curUsingSatelliteIdx]
        };
    }

    getAllSatelliteSetting() {
        var that = this, i;
        var searchSatelName = [];
        var antennaJson = [];
        var userlnbFreq = [];
        var antLength = that._antlength;

        for (i = 0; i < antLength; i += 1) {
            searchSatelName.push(that._SatNameConverter(that._searchSatelliteName[i]).convName);
            antennaJson.push(that._antennaSetJson[i]);
            userlnbFreq.push(that._userlnbFreq[i]);
        }
        return {
            satName: searchSatelName,
            antennaJson: antennaJson,
            userlnbFreq: userlnbFreq,
            antLength: antLength
        };
    }

    antennaSetting() {
        var that = this, i;
        var antInfoList = that._antennaSetting();
        hx.log('dsrvChannelSearch', 'antennaSetting() called!!!');
        /* initialize */
        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            that._searchSatelliteName[i] = 'NONE';
            that._antennaSetJson[i].searchType = 0;
            that._antennaSetJson[i].switchInput = 0;
            that._antennaSetJson[i].lnbFreq = 0;
            that._antennaSetJson[i].toneEnable = 0;
            that._antennaSetJson[i].diseqcVersion = 0;
        }

        that._antlength = antInfoList.length;
        /* setting */
        for (i = 0; i < that._antlength; i += 1) {
            that._searchSatelliteName[i] = antInfoList[i].satelliteName;
            that._antennaSetJson[i].lnbFreq = antInfoList[i].lnbFrequency;
            if (that._antennaSetJson[i].lnbFreq === 5) {
                that._userlnbFreq[i] = antInfoList[i].userlnbFreq;
            }
            that._antennaSetJson[i].switchInput = antInfoList[i].switchInput;
            that._antennaSetJson[i].toneEnable = antInfoList[i].enable22KHzTone;
            that._antennaSetJson[i].searchType = antInfoList[i].antennaType;

            if (i === 0) {
                that._tuner1Param.scdBandNo = antInfoList[i].scdBandNo1;
                that._tuner2Param.scdBandNo = antInfoList[i].scdBandNo2;
                that._tuner3Param.scdBandNo = antInfoList[i].scdBandNo3;
                that._tuner4Param.scdBandNo = antInfoList[i].scdBandNo4;
                that._tuner1Param.scdBandFreq = antInfoList[i].scdBandFreq1;
                that._tuner2Param.scdBandFreq = antInfoList[i].scdBandFreq2;
                that._tuner3Param.scdBandFreq = antInfoList[i].scdBandFreq3;
                that._tuner4Param.scdBandFreq = antInfoList[i].scdBandFreq4;
            }
        }
        return antInfoList.length;
    }

    ////////////////////////*  Manage Satellite END  */////////////////////////////////////////

    /////////////////////* Manage Transponder List START *////////////////////////////////////////////
    getTransponderSelectorAll(data) {// It is used in searchStart Page so I shoud check _tsIndex
        var that = this;
        var returnValue = [], index = 0, value, tpList, tpInfo, i;

        // _searchSatelliteName's TPList
        function _getSearchingTP(satName, tsIndex, userTPParam, returnValue) {
            if (tsIndex === 0) {
                if (satName !== null && satName !== 'NONE') {
                    var idx, idx2;
                    for (idx = 0; idx < that._transponderList.length; idx += 1) {
                        if (that._transponderList[idx].satName === satName) {
                            tpList = that._transponderList[idx].tpList;
                            for (idx2 = 0; idx2 < tpList.length; idx2 += 1) {
                                tpInfo = tpList[idx2];

                                value = that._SatNameConverter(satName).convName;
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
                                } else {
                                    value += tpInfo[3];
                                }
                                returnValue[index] = value;
                                index += 1;
                            }
                        }
                    }
                }
            } else if (tsIndex === 2) {// user define
                if (satName !== null && satName !== 'NONE') {
                    value = that._SatNameConverter(satName).convName;
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
                    } else {
                        value += userTPParam.userFEC;
                    }
                    returnValue[index] = value;
                    index += 1;
                }
            } else if (tsIndex > 2) {
                if (satName !== null && satName !== 'NONE') {
                    var i;
                    for (i = 0; i < that._transponderList.length; i += 1) {
                        if (that._transponderList[i].satName === satName) {
                            tpList = that._transponderList[i].tpList;

                            tpInfo = tpList[tsIndex - 3];

                            value = that._SatNameConverter(satName).convName;
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
                            } else {
                                value += tpInfo[3];
                            }
                            returnValue[index] = value;
                            index += 1;
                        }
                    }
                }
            }
        }

        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            _getSearchingTP(that._searchSatelliteName[i], that._tsIndex[i], that._userTPParam[i], returnValue);
        }
        return {
            transponderSelector: returnValue
        };
    }

    getTpUserSelector() {//It is used in lnbSearch, diseqcSearch page to show UserSettingTp
        var that = this, tpIndex = [], paramToGet = [], tpUserSelector = [], satNameList = [], i;

        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            paramToGet.push(that._userTPParam[i]);
            tpIndex.push(that._tsIndex[i]);
            satNameList.push(that._searchSatelliteName[i]);
        }

        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
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
            } else {
                value += paramToGet[i].userFEC;
            }
            tpUserSelector.push(value);
        }

        return {
            tpUserSelector: tpUserSelector,
            tpIndex: tpIndex,
            satNameList: satNameList
        };
    }

    getUserSettingTestTp() {//It is used in satelliteSetting page to show UserSettingTp
        var that = this;
        var i;
        var value = (that._userTestTPParam.userFreq) + ', ';
        switch (that._userTestTPParam.userPola) {
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
        value += (that._userTestTPParam.userSymbol) + ', ';
        if (that._userTestTPParam.userFEC === 'auto') {
            value += 'A';
        } else {
            value += that._userTestTPParam.userFEC;
        }

        return {
            value: value,
            userTestTp: that._userTestTPParam,
            testTpIndex: that._testtsIndex
        };
    }

    getTransponderSelector() {//It is used in lbnSearch, diseqcSearch page to show TpList
        var that = this, i;
        var allreturnValue = [];
        var returnValue = [];
        var index = 0;
        var tsIndex = [];

        for (i = 0; i < that._antlength; i += 1) {
            tsIndex.push(that._tsIndex[i]);
            returnValue = [];
            if (that._searchSatelliteName[i] === 'NONE') {
                returnValue.push(hx.l('LOC_NONE_ID'));
            } else {
                var idx, idx2;
                returnValue.push(hx.l('LOC_ALL_ID'));
                returnValue.push(hx.l('LOC_NONE_ID'));
                returnValue.push(hx.l('LOC_USERDEFINE_ID'));
                for (idx = 0; idx < that._transponderList.length; idx += 1) {
                    if (that._transponderList[idx].satName === that._searchSatelliteName[i]) {
                        var tpList = that._transponderList[idx].tpList;
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
                            } else {
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
    }

    getTestTransponderSelector() {//It is used in satelliteSetting page to show testTpList
        var that = this;
        var returnValue = [];
        var index = 0;
        // DBG('getValueList for transponderSelector');
        returnValue[index] = hx.l('LOC_USERDEFINE_ID') + ' ';
        index += 1;
        var i;
        for (i = 0; i < that._transponderList.length; i += 1) {
            if (that._transponderList[i].satName === that._TestSatelliteName) {
                var j, tpList = that._transponderList[i].tpList;
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
                    } else {
                        value += tpInfo[3];
                    }
                    returnValue[index] = value;
                    index += 1;
                }
            }
        }
        // return returnValue;
        if (returnValue.length <= 1) {
            that._testtsIndex = 0;
        } else if (returnValue.length > 1) {
            that._testtsIndex = 1;
        }

        return {
            transponderSelector: returnValue
        };
        //dlEvent : 'dlgetTestTpSelector'
    }

    setUserlnbFreq(param) {
        var that = this;
        that._userlnbFreq[that._curUsingSatelliteIdx] = param.userlnbFreq;
        that._userTestlnbFreq = param.userlnbFreq;
    }

    setUserTPParam(param) {
        var that = this;
        var paramToSet;
        paramToSet = that._userTPParam[that._curSettingTpIdx];
        paramToSet.userFreq = param.userFreq;
        paramToSet.userPola = param.userPola;
        paramToSet.userSymbol = param.userSymbol;
        paramToSet.userTrans = param.userTrans;
        paramToSet.userFEC = param.userFEC;
        that._saveTpParamToLocal(that._curSettingTpIdx);
    }

    setUserTestTPParam(param) {
        var that = this;
        that._userTestTPParam.userFreq = param.userFreq;
        that._userTestTPParam.userPola = param.userPola;
        that._userTestTPParam.userSymbol = param.userSymbol;
        that._userTestTPParam.userTrans = param.userTrans;
        that._userTestTPParam.userFEC = param.userFEC;
    }

    setUserTempTPParam(param) {
        var that = this;
        that._userTempTPParam.userFreq = param.userFreq;
        that._userTempTPParam.userPola = param.userPola;
        that._userTempTPParam.userSymbol = param.userSymbol;
        that._userTempTPParam.userTrans = param.userTrans;
        that._userTempTPParam.userFEC = param.userFEC;
    }

    getUserTPParam() {
        var that = this;
        return {
            tpParam: that._userTPParam[that._curSettingTpIdx]
        };
    }

    setTpIndex(param) {
        var that = this;
        that._tsIndex[that._curSettingTpIdx] = param.tsIndex;
        that._saveTpIdxToLocal(that._curSettingTpIdx);
    }

    setTestTpIndex(param) {
        var that = this;
        that._testtsIndex = param.testtsIndex;
    }

    /////////////////////* Manage Transponder List END *////////////////////////////////////////////

    ///////////////////////////*  SCD Channel Search START *///////////////////////////////
    setlastSettingTuner(param) {
        var that = this;
        that._lastSettingTuner = param.lastSettingTuner;
    }

    getlastSettingTuner() {
        var that = this;
        return that._lastSettingTuner;
    }

    setScdSetting(param) {
        var that = this;
        that._tuner1Param.scdBandNo = param.scdBandNo;
        that._tuner2Param.scdBandNo = param.scdBandNo2;
        that._tuner3Param.scdBandNo = param.scdBandNo3;
        that._tuner4Param.scdBandNo = param.scdBandNo4;
        that._tuner1Param.scdBandFreq = param.scdBandFreq;
        that._tuner2Param.scdBandFreq = param.scdBandFreq2;
        that._tuner3Param.scdBandFreq = param.scdBandFreq3;
        that._tuner4Param.scdBandFreq = param.scdBandFreq4;
    }

    getScdSetting() {
        var that = this;
        return {
            scdBandNo: that._tuner1Param.scdBandNo,
            scdBandNo2: that._tuner2Param.scdBandNo,
            scdBandNo3: that._tuner3Param.scdBandNo,
            scdBandNo4: that._tuner4Param.scdBandNo,
            scdBandFreq: that._tuner1Param.scdBandFreq,
            scdBandFreq2: that._tuner2Param.scdBandFreq,
            scdBandFreq3: that._tuner3Param.scdBandFreq,
            scdBandFreq4: that._tuner4Param.scdBandFreq
        };
    }

    ///////////////////////////*  SCD Channel Search END *////////////////////////

// ####################################################################################################
    // Private Methods
    /////////////////////////// Initialize START ///////////////////////////////////
    private _initializeAntData() {
        var that = this, i;
        that.SATELLITE_NUMBER_TO_MAKE = 4;
        that._TestSatelliteName = 'NONE';
        that._testAntennaId = 0;
        that._testAntennaSetJson = {
            searchType: 0,
            switchInput: 0,
            lnbFreq: 0,
            toneEnable: 0,
            diseqcVersion: 0
        };
        that._testtsIndex = 0;
        that._userTestlnbFreq = 0;
        that._userTestTPParam = {
            userFreq: '0',
            userPola: 'auto',
            userSymbol: '27500',
            userTrans: 'DVB',
            userFEC: 'auto'
        };
        that._userTempTPParam = {
            userFreq: '0',
            userPola: 'auto',
            userSymbol: '27500',
            userTrans: 'DVB',
            userFEC: 'auto'
        };

        that._searchSatelliteName = [];
        that._searchAntennaId = [];
        that._antennaSetJson = [];
        that._tsIndex = [];
        that._userlnbFreq = [];
        that._userTPParam = [];

        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            that._searchSatelliteName.push('NONE');
            that._searchAntennaId.push(0);
            that._antennaSetJson.push({
                searchType: 0,
                switchInput: 0,
                lnbFreq: 0,
                toneEnable: 0,
                diseqcVersion: 0
            });
            that._tsIndex.push(0);
            that._userlnbFreq.push(0);
            that._userTPParam.push({
                userFreq: '0',
                userPola: 'auto',
                userSymbol: '27500',
                userTrans: 'DVB',
                userFEC: 'auto'
            });
        }
    }

    private _loadTpData() {
        var that = this;
        $.ajax({
            url: 'pages/util/satTpData.json',
            dataType: 'json',
            async: false,
            success: function (data) {
                that._transponderList = data;
            }
        });
    }

    private _loadSatPosData() {
        var that = this;
        $.ajax({
            url: 'pages/util/satPositionData.json',
            dataType: 'json',
            async: false,
            success: function (data) {
                that._satPosDataList = data;
            }
        });
    }

    private _checkAntennaInformationWithSystemAntennaInformation() {
        var that = this;
        //hx.logadd('onCheckAntenna');

        if ((hx.config.useImportExportChannels) && (hx.config.useImportExportChannels === 1)) {
            var i = 0, antInfo, antInfoCount = 0;
            var stSysAntennaList = [], stSearchAntennaId = [];
            var stWebAntennaList;

            // Inner Function
            var fnGetAntIdListFromLocal = function() {
                var antIdList = null;

                antIdList = hx.svc.WEB_STORAGE.getAntIdListFromLocal();
                for ( i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                    var antId = antIdList['antId' + (i + 1)];
                    if (antId && antId !== 'undefined') {
                        stSearchAntennaId[i] = parseInt(antId, 10);
                    }
                }
            };

            var fnLoadSystemAntennaInformation = function() {
                antInfoCount = that.getstartDiseqcScanAntennaListCount();
                hx.log('onCheckAntenna', 'getstartDiseqcScanAntennaListCount : ' + antInfoCount);
                // Copy Antenna List
                for ( i = 0; i < antInfoCount; i += 1) {
                    antInfo = that.getstartDiseqcScanAntennaInfo({
                        index : i
                    });
                    hx.log('onCheckAntenna', 'getstartDiseqcScanAntennaInfo get Antenna name : ' + antInfo.satelliteName);
                    if (antInfo.satelliteName === 'NONE') {
                        continue;
                    }
                    stSysAntennaList.push(antInfo);
                    if (antInfo.antennaType === that.HXAntennaType.ETypeLnb) {
                        break;
                    }
                }
            };

            var fnGetWebAntennaSettingNum = function() {
                var ulCount = 0;

                for ( i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                    if (!!stSearchAntennaId[i] && stSearchAntennaId[i] !== '0') {
                        ulCount++;
                    }
                }
                return ulCount;
            };

            var fnReSetWebAntennaSetting = function(pNum) {
                var antId = 0;
                for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
                    if (i < pNum) {
                        antId = i+1;
                    } else {
                        antId = 0;
                    }
                    hx.svc.WEB_STORAGE.saveAntIdToLocal({
                        antId : antId,
                        index : i
                    });
                }
            };

            var fnSetSearchType = function() {
                var type = 'lnb', searchType = 0;
                var count = stSysAntennaList.length;

                for (i=0; i< count; i++) {
                    searchType = stSysAntennaList[i].antennaType;
                    break;
                }

                if ((searchType === that.HXAntennaType.ETypeLnb) || (searchType === null)) {//LNB Page
                    type = 'lnb';
                } else if (searchType === that.HXAntennaType.ETypeDiseqc) {//DiSEqC Page
                    type = 'diseqc';
                } else if (searchType === that.HXAntennaType.ETypeScd) {//SCD Page
                    type = 'scd';
                } else if (searchType === that.HXAntennaType.ETypeMotor) {//Motor Page
                    type = 'motor';
                }

                hx.log('onCheckAntenna', 'getstartDiseqcScanAntennaListCount : ' + type);
                hx.svc.WEB_STORAGE.setLastSearchType({
                    lastSearchType : type
                });
            };

            var ulLocAntNum = 0, ulSysAntNum = 0;
            // Loader Web-App Antenna Info.
            fnGetAntIdListFromLocal();
            ulLocAntNum = fnGetWebAntennaSettingNum();

            // Load MW System Antenna Info.
            fnLoadSystemAntennaInformation();
            ulSysAntNum = stSysAntennaList.length;

            if (ulSysAntNum !== ulLocAntNum) {
                hx.log("error", '#### _checkAntennaInformationWithSystemAntennaInformation : System & Web antenna config miss match!');
                fnReSetWebAntennaSetting(ulSysAntNum);
                fnSetSearchType();
            }
        }
    }

    private _initLcnEventHandler() {
        var that = this, i;
        var lcnEventType = hx.il.getEnum('LcnEventType');
        hx.logadd('onLcnUpdate');
        that.oipfHLcnUpdate.onLcnUpdate = function (eventType, numOfSvcList, svcList, numOfNewSvc, newSvc, numOfReplacedSvc, replacedSvc, coldbootResult) {
            hx.log('onLcnUpdate', 'eventType : ' + eventType + ', numOfSvcList : ' + numOfSvcList + ', numOfNewSvc : ' + numOfNewSvc + ', numOfReplacedSvc : ' + numOfReplacedSvc + ', coldbootResult : ' + coldbootResult);
            switch (eventType) {
                case lcnEventType.STARTOK:
                case lcnEventType.STARTFAIL:
                    if (that.curLcnScanType === that.lcnScanType.UPDATE) {
                        that.fireEvent('startLcnListUpdate', {
                            eventType: eventType
                        });
                    } else if (that.curLcnScanType === that.lcnScanType.CHANGE) {
                        var _svcInfos = [];
                        if (svcList) {
                            for (i = 0; i < numOfSvcList; i += 1) {
                                _svcInfos.push({
                                    'name': svcList.getInfo(i, 'name'),
                                    'index': svcList.getInfo(i, 'index')
                                });
                            }
                        }
                        that.fireEvent('startLcnListScan', {
                            eventType: eventType,
                            svcInfos: _svcInfos
                        });
                    }
                    break;
                case lcnEventType.SAVEOK:
                case lcnEventType.SAVEFAIL:
                    // Make new, replaced svcInfos
                    var _newSvcInfos = [], _replacedSvcInfos = [];
                    if (newSvc) {
                        for (i = 0; i < numOfNewSvc; i += 1) {
                            _newSvcInfos.push({
                                'name': newSvc.getInfo(i, 'name'),
                                'lcn': newSvc.getInfo(i, 'lcn'),
                                'ccid': newSvc.getInfo(i, 'ccid')
                            });
                        }
                    }
                    if (replacedSvc) {
                        for (i = 0; i < numOfReplacedSvc; i += 1) {
                            _replacedSvcInfos.push({
                                'name': replacedSvc.getInfo(i, 'name'),
                                'lcn': replacedSvc.getInfo(i, 'lcn'),
                                'ccid': replacedSvc.getInfo(i, 'ccid')
                            });
                        }
                    }

                    // Send Event
                    if (that.curLcnScanType === that.lcnScanType.UPDATE) {
                        that.fireEvent('saveLcnUpdate', {
                            eventType: eventType,
                            newSvcInfos: _newSvcInfos,
                            replacedSvcInfos: _replacedSvcInfos
                        });
                    } else if (that.curLcnScanType === that.lcnScanType.CHANGE) {
                        if ((_newSvcInfos.length === 0) && (_replacedSvcInfos.length === 0)) {
                            that.fireEvent('saveLcnChannels', {
                                eventType: eventType
                            });
                        } else {
                            that.fireEvent('saveLcnUpdate', {
                                eventType: eventType,
                                newSvcInfos: _newSvcInfos,
                                replacedSvcInfos: _replacedSvcInfos
                            });
                        }
                    }
                    break;
                case lcnEventType.STOPOK:
                case lcnEventType.STOPFAIL:
                    break;
                case lcnEventType.GETINFO:
                    that.fireEvent('getLcnInfo', {//overloading Event Parameter
                        eventType: eventType,
                        svcListName: numOfSvcList,
                        tvNum: svcList,
                        radioNum: numOfNewSvc,
                        lastUpdatedTime: newSvc,
                        lastCheckedTime: numOfReplacedSvc,
                        displayInfo: replacedSvc,
                        coldbootResult: coldbootResult
                    });
                    break;
            }
        };
    }

    /////////////////////////// Initialize END! ///////////////////////////////////////

    //////////////////////  Save to WebStorage & get from WebStorage START ////////////////////////////
    private _saveAntIdToLocal(curUsingSatelliteIdx) {
        var that = this;
        hx.svc.WEB_STORAGE.saveAntIdToLocal({
            antId: that._searchAntennaId[curUsingSatelliteIdx],
            index: curUsingSatelliteIdx
        });
    }

    private _saveSearchOptionsToLocal() {
        var that = this;

        hx.svc.WEB_STORAGE.saveSearchOptionsToLocal({
            network: that._searchOptions.network,
            scramble: that._searchOptions.scramble,
            channel: that._searchOptions.channel
        });
    }

    private _getSearchOptionsFromLocal() {
        var that = this;
        var searchOptions;

        searchOptions = hx.svc.WEB_STORAGE.getSearchOptionsFromLocal();

        if (searchOptions.network && searchOptions.network !== 'undefined') {
            that._searchOptions.network = searchOptions.network;
        } else {
            that._saveSearchOptionsToLocal();
        }

        if (searchOptions.scramble && searchOptions.scramble !== 'undefined') {
            that._searchOptions.scramble = searchOptions.scramble;
        } else {
            that._saveSearchOptionsToLocal();
        }

        if (searchOptions.channel && searchOptions.channel !== 'undefined') {
            that._searchOptions.channel = searchOptions.channel;
        } else {
            that._saveSearchOptionsToLocal();
        }
    }

    private _getTpIdxListFromLocal() {
        var that = this, i, tpIdxObj;

        tpIdxObj = hx.svc.WEB_STORAGE.getTpIdxListFromLocal();
        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            var tpIdx = tpIdxObj['Idx' + (i + 1)];
            if (tpIdx !== null && tpIdx !== undefined) {
                that._tsIndex[i] = parseInt(tpIdx, 10);
            }
        }
    }

    private _saveTpIdxToLocal(curSettingTpIdx) {
        var that = this;

        hx.svc.WEB_STORAGE.saveTpIdxToLocal({
            position: curSettingTpIdx,
            Idx: that._tsIndex[curSettingTpIdx]
        });
    }

    private _getTpParamFromLocal() {
        var that = this, userTpParamObj, i;

        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            //get TpParam from WebStorage
            userTpParamObj = hx.svc.WEB_STORAGE['getUserTpParam' + (i + 1) + 'FromLocal'];
            //cache TpParam
            if (userTpParamObj.userFreq !== null && userTpParamObj.userFreq !== undefined) {
                that._userTPParam[i].userFreq = userTpParamObj.userFreq;
            }
            if (userTpParamObj.userPola !== null && userTpParamObj.userPola !== undefined) {
                that._userTPParam[i].userPola = userTpParamObj.userPola;
            }
            if (userTpParamObj.userSymbol !== null && userTpParamObj.userSymbol !== undefined) {
                that._userTPParam[i].userSymbol = userTpParamObj.userSymbol;
            }
            if (userTpParamObj.userTrans !== null && userTpParamObj.userTrans !== undefined) {
                that._userTPParam[i].userTrans = userTpParamObj.userTrans;
            }
            if (userTpParamObj.userFEC !== null && userTpParamObj.userFEC !== undefined) {
                that._userTPParam[i].userFEC = userTpParamObj.userFEC;
            }
        }
    }

    private _saveTpParamToLocal(curSettingTpIdx) {
        var that = this;

        hx.svc.WEB_STORAGE.saveUserTpParamToLocal({
            index: curSettingTpIdx,
            userFreq: that._userTPParam[curSettingTpIdx].userFreq,
            userPola: that._userTPParam[curSettingTpIdx].userPola,
            userSymbol: that._userTPParam[curSettingTpIdx].userSymbol,
            userTrans: that._userTPParam[curSettingTpIdx].userTrans,
            userFEC: that._userTPParam[curSettingTpIdx].userFEC
        });
    }
    //////////////////////  Save to WebStorage & get from WebStorage END! ////////////////////////////

    ////////////////////// Channel Scan Control START ////////////////////////////////////////////////
    private _startScan(param) {
        var that = this;
        if (param.isStartScan) {
            that.oipfChannelConfigObj.stopScan();
        }
        var scanParamCollection = that._createChannelScanParamCollectionObject();
        var channelScanOptions = that._createChannelScanOptionsObject();
        var userParam = param.usrParam;
        var confParam = {
            frequency : 0,
            symbolrate : '',
            polarization: 0,
            fec : '',
            modulation : 0
        };
        var _convertTpInfoOipf = function (itemInfo) {
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
        };

        var tpList, j, tpInfo;

        for (j = 0; j < param.satName.length; j += 1) {
            var i;
            if (param.satName[j] === 'NONE') {
                continue;
            }
            for (i = 0; i < that._transponderList.length; i += 1) {
                if (that._transponderList[i].satName === param.satName[j]) {
                    tpList = that._transponderList[i].tpList;
                    break;
                }
            }

            if (param.tsIndex[j] > 0) {
                switch (param.tsIndex[j]) {
                    case    1:
                        //select NONE
                        break;
                    case    2:
                        //select User Define
                        //	Convert User Param to OIPF Param
                        confParam.frequency = userParam[j].userFreq * 1000;
                        confParam.symbolrate = (userParam[j].userSymbol * 1000).toString();
                        if (userParam[j].userPola === "Auto" || userParam[j].userPola === "auto") {
                            confParam.polarization = 0;
                        } else if (userParam[j].userPola === "Horizontal" || userParam[j].userPola === "horizontal") {
                            confParam.polarization = 1;
                        } else {
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
                        that._addScanParameters(confParam.frequency, confParam.symbolrate, confParam.polarization, confParam.fec, confParam.modulation, param.antId[j], param.satName[j], scanParamCollection);
                        break;
                    default:
                        tpInfo = tpList[param.tsIndex[j] - 3];
                        confParam = _convertTpInfoOipf(tpInfo);
                        that._addScanParameters(confParam.frequency, confParam.symbolrate, confParam.polarization, confParam.fec, confParam.modulation, param.antId[j], param.satName[j], scanParamCollection);
                        break;
                }
            } else {
                //	Scan All Tp in Satellite...
                for (i = 0; i < tpList.length; i += 1) {
                    tpInfo = tpList[i];
                    confParam = _convertTpInfoOipf(tpInfo);
                    that._addScanParameters(confParam.frequency, confParam.symbolrate, confParam.polarization, confParam.fec, confParam.modulation, param.antId[j], param.satName[j], scanParamCollection);
                }
            }

        }

        switch (param.searchOptions.channel) {
            case 'All':
                //	Type All
                that._setScanOptionsChannelType(100, channelScanOptions);
                break;
            case 'TV':
                that._setScanOptionsChannelType(0, channelScanOptions);
                break;
            case 'Radio':
                that._setScanOptionsChannelType(1, channelScanOptions);
                break;
            default:
                that._setScanOptionsChannelType(2, channelScanOptions);
                // ALL
                break;
        }
        switch (param.searchOptions.scramble) {
            case 'All':
                that._setScanOptionsCasType(0, channelScanOptions);
                break;
            case 'FTA':
                that._setScanOptionsCasType(1, channelScanOptions);
                break;
            case 'CAS':
                that._setScanOptionsCasType(4096, channelScanOptions);
                // ALL CAS
                break;
            default:
                that._setScanOptionsCasType(0, channelScanOptions);
                // ALL
                break;
        }
        that._setScanOptionsNetwork((param.searchOptions.network === 'true' ? true : false), channelScanOptions);

        var antInfo = that._getAntennaInfoWithId(param.antId[0]);
        that.oipfChannelConfigObj.onChannelScan = param.fn;
        that.oipfChannelConfigObj.startScan(channelScanOptions, scanParamCollection);
    }

    private _startAntennaConnection(param) {
        var that = this;
        if (param.isStartScan) {
            that.oipfChannelConfigObj.stopScan();
        }
        var scanParamCollection = that._createChannelScanParamCollectionObject();
        var confParam = {
            frequency : 0,
            symbolrate : '',
            polarization: 0,
            fec : '',
            modulation : 0
        };

        var _convertTpInfoOipf = function (itemInfo) {
            var oipfTpInfo, modulation;
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
        };

        var tpList, tpInfo, i, j, lockingDataList, antInfo;

        lockingDataList = param.lockingDataList;
        for (i = 0; i < lockingDataList.length; i += 1) {
            tpInfo = lockingDataList[i].tpData;
            antInfo = that._getAntennaInfoFromId(lockingDataList[i].antId);
            confParam = _convertTpInfoOipf(tpInfo);
            that._addScanParameters(confParam.frequency, confParam.symbolrate, confParam.polarization, confParam.fec, confParam.modulation, lockingDataList[i].antId, antInfo.satelliteType, scanParamCollection);
        }
        that.oipfChannelConfigObj.onChannelScan = param.fn;
        that.oipfChannelConfigObj.startScan(null, scanParamCollection, 'antennaconnection');
    }

    private _startTune(param) {
        var that = this;
        if (param.isStartScan) {
            that.oipfChannelConfigObj.stopScan();
        }
        var channelScanParams = that.oipfChannelConfigObj.createChannelScanParametersObject(11);
        var connectionType = that.oipfLocalSystemObj.antennaInfoLists.getConnectionType();
        var _convertTpInfoOipf = function (itemInfo) {
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
        };

        var tpList, i;
        for (i = 0; i < that._transponderList.length; i += 1) {
            if (that._transponderList[i].satName === param.satName) {
                tpList = that._transponderList[i].tpList;
                break;
            }
        }
        var frequency = 0;
        var symbolrate;
        var modulation = 0;
        var confParam = {
            frequency : 0,
            symbolrate : 0,
            polarization: 0,
            fec : '',
            modulation : 0
        };
        var fec, polarization;
        var userParam = param.usrParam;
        if (param.tsIndex === 0)//User Define
        {
            //	Convert User Param to OIPF Param
            frequency = userParam.userFreq * 1000;
            symbolrate = (userParam.userSymbol * 1000).toString();
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
            } else if (userParam.userPola === 'horizontal' || userParam.userPola === 'Horizontal') {
                polarization = 1;
            } else if (userParam.userPola === 'vertical' || userParam.userPola === 'Vertical') {
                polarization = 2;
            } else {
                polarization = 0;
            }
        } else {
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
            symbolrate = symbolrate.toString();
        }

        var satName = param.satName;
        var antId = param.antId;
        that._settingChannelScanParameters(frequency, symbolrate, polarization, fec, modulation, antId, satName, channelScanParams);
        var antInfo = that._getAntennaInfoWithId(antId);
        //that.channelConfig.onChannelScan = param.fn;
        that.oipfChannelConfigObj.startScan(null, channelScanParams, 'tuneonly');
        that._isStartScan = true;
        //connectionType ? 'tunetwin' : 'tuneonly'
        //When search channels, do not consider connectionType because we use only first tuner
        return true;
    }

    private _disecScanCompleted() {
        var that = this;
        that.oipfChannelConfigObj.controlScan('load');
        that.oipfChannelConfigObj.controlScan('save');
        that.oipfChannelConfigObj.stopScan();
        return that._getAntennaInfo();
    }

    private _save_disecScanCompletedAntenna(scanData) {
        var that = this;
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
        that._antennaSetJson[that._curUsingSatelliteIdx].searchType = scanData.antennaType;
        that._antennaSetJson[that._curUsingSatelliteIdx].switchInput = scanData.switchInput;
        that._antennaSetJson[that._curUsingSatelliteIdx].lnbFreq = scanData.lnbFrequency;
        that._antennaSetJson[that._curUsingSatelliteIdx].toneEnable = scanData.enable22KHzTone;
        that._searchSatelliteName[that._curUsingSatelliteIdx] = scanData.satelliteType;

        var antId;
        //consider only two Antenna type Diseqc Scan
        antId = that.setAntennaInfo({
            satName: that._searchSatelliteName[that._curUsingSatelliteIdx],
            antSet: that._antennaSetJson[that._curUsingSatelliteIdx],
            antId: that._searchAntennaId[that._curUsingSatelliteIdx],
            tuner1Param: that._tuner1Param,
            tuner2Param: that._tuner2Param,
            tuner3Param: that._tuner3Param,
            tuner4Param: that._tuner4Param
        });
        that._setAntennaId(antId, that._curUsingSatelliteIdx);
        that._saveAntIdToLocal(that._curUsingSatelliteIdx);
    }

    private _updateChannelList() {
        var that = this;
        hx.log('updateLcnChannelList', 'call oipfHLcnUpdate.startLcnUpdate(lcnScanType.UPDATE)');
        that.oipfHLcnUpdate.startLcnUpdate(that.lcnScanType.UPDATE);
    }

    private _changeChannelList() {
        var that = this;
        hx.log('changeLcnChannelList', 'call oipfHLcnUpdate.startLcnUpdate(lcnScanType.CHANGE)');
        that.oipfHLcnUpdate.startLcnUpdate(that.lcnScanType.CHANGE);
    }

    private _saveLcnUpdate(param) {
        var that = this;
        hx.log('saveLcnUpdate', 'call oipfHLcnUpdate.saveLcnUpdate(param.searchMode, param.index), param.searchMode : ' + param.searchMode + ', param.index : ' + param.index);
        that.oipfHLcnUpdate.saveLcnUpdate(param.searchMode, param.index);
    }

    private _getLcnUpdateInfo() {
        var that = this;
        that.oipfHLcnUpdate.getLcnUpdateInfo();
    }

    private _removeLcnUpdateInfo() {
        var that = this;
        var sync = false;
        that.oipfHLcnUpdate.removeLcnUpdateInfo(sync);
    }

    //////////////////////////// Channel Scan Control END! //////////////////////////////////////////

    /////////////////////////// Manage Channel Scan Parameter START ///////////////////////////////////////
    private _createChannelScanParamCollectionObject() {
        var that = this;
        var scanParamCollection = that.oipfChannelConfigObj.createChannelScanParametersObject();
        return scanParamCollection;
    }

    private _addScanParameters(freq, sr, polar, fec, modul, searchAntennaId, searchSatelliteName, scanParamCollection) {
        var that = this;
        var channelParams = that.oipfChannelConfigObj.createChannelScanParametersObject(11);
        channelParams = that._settingChannelScanParameters(freq, sr, polar, fec, modul, searchAntennaId, searchSatelliteName, channelParams);
        scanParamCollection.addChannelScanParam(channelParams);
    }

    private _settingChannelScanParameters(freq, sr, polar, fec, modul, searchAntennaId, searchSatelliteName, channelParams) {
        channelParams.startFrequency = freq;
        channelParams.endFrequency = freq;
        channelParams.symbolRate = sr;
        channelParams.modulationModes = modul;
        channelParams.polarisation = polar;
        channelParams.codeRate = fec;
        channelParams.antennaId = searchAntennaId;
        channelParams.antennaName = searchSatelliteName;
        return channelParams;
    }

    private _createChannelScanOptionsObject() {
        var that = this;
        var channelScanOptions = that.oipfChannelConfigObj.createChannelScanOptionsObject();
        return channelScanOptions;
    }

    private _setScanOptionsChannelType(channelType, channelScanOptions) {
        var that = this;
        channelScanOptions.channelType = channelType;
    }

    private _setScanOptionsCasType(casType, channelScanOptions) {
        var that = this;
        channelScanOptions.casType = casType;
    }

    private _setScanOptionsNetwork(network, channelScanOptions) {
        var that = this;
        channelScanOptions.enableNetworkScan = network;
    }

    private _getTuningInfo(satName, type) {
        // type
        // 0 : unused
        // 1 : diseqc detection
        // 2 : antenna connection
        // 4 : both
        var a = this, tp = a._transponderList, data = [], i, j;
        var length, tpListLength;
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
    }

    /////////////////////////// Manage Channel Scan Parameter END! ///////////////////////////////////////

    /////////////////////////// Manage Satellite START //////////////////////////////////////////////
    private _antennaSetting() {
        var that = this, i;
        var antInfoList = [];

        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            if (!!that._searchAntennaId[i] && that._searchAntennaId[i] !== '0') {
                var antInfo = that._getAntennaInfoFromId(that._searchAntennaId[i]);
                if (antInfo.satelliteName !== 'NONE') {
                    antInfoList.push(that._convertAntennaData(antInfo));
                }
                if (i === 0) {
                    if (antInfo.antennaType === 0) {//LNB just need one ant
                        return antInfoList;
                    }
                }
            }
        }
        return antInfoList;
    }

    private _getAllAntennaInfo() {
        var that = this;
        var antInfoLists = that.oipfLocalSystemObj.antennaInfoLists;

        return antInfoLists;
    }

    private _getAntennaInfo() {
        var that = this;
        var antInfo = that._doGetCurrentAntInfo();
        if (antInfo === null) {
            return null;
        }
        var convAntInfo = that._convertAntennaData(antInfo);

        return {
            antennaType: convAntInfo['antennaType'],
            lnbFrequency: convAntInfo['lnbFrequency'],
            switchInput: convAntInfo['switchInput'],
            enable22KHzTone: convAntInfo['enable22KHzTone'],
            satelliteName: convAntInfo['satelliteName'],
            satelliteType: convAntInfo['satelliteType']
        };
    }

    private _doGetCurrentAntInfo() {
        var that = this;
        var ret = null;
        var antInfoLists = that.oipfLocalSystemObj.antennaInfoLists;
        var curAntName = that._searchSatelliteName[that._curUsingSatelliteIdx];

        var i, len = antInfoLists.length;
        for (i = 0; i < len; i += 1) {
            var antInfo = antInfoLists.item(i);
            if (antInfo.satelliteType.toLowerCase() === curAntName.toLowerCase()) {
                ret = antInfo;
            }
        }
        return ret;
    }

    private _setAntennaId(id, curUsingSatelliteIdx) {
        var that = this;
        that._searchAntennaId[curUsingSatelliteIdx] = id;
    }

    private _setTestAntennaId(id) {
        var that = this;
        that._testAntennaId = id;
    }

    private _setAntennaInfolnbFreq(antInfo, lnbFreq) {
        antInfo.lnbFrequency = lnbFreq;
    }

    private _setAntennaInfoantennaType(antInfo, antennaType) {
        antInfo.antennaType = antennaType;
    }

    private _setAntennaInfoswitchInput(antInfo, switchInput) {
        antInfo.switchInput = switchInput;
    }

    private _setAntennaInfodiseqcVersion(antInfo, diseqcVersion) {
        antInfo.diseqcVersion = diseqcVersion;
    }

    private _setAntennaInfoenable22KHzTone(antInfo, enable22KHzTone) {
        antInfo.enable22KHzTone = enable22KHzTone;
    }

    private _setAntennaInfoscdBandNo0(antInfo, scdBandNo0) {
        antInfo.scdBandNo0 = scdBandNo0;
    }

    private _setAntennaInfoscdBandNo1(antInfo, scdBandNo1) {
        antInfo.scdBandNo1 = scdBandNo1;
    }

    private _setAntennaInfoscdBandNo2(antInfo, scdBandNo2) {
        antInfo.scdBandNo2 = scdBandNo2;
    }

    private _setAntennaInfoscdBandNo3(antInfo, scdBandNo3) {
        antInfo.scdBandNo3 = scdBandNo3;
    }

    private _setAntennaInfoscdBandFreq0(antInfo, scdBandFreq0) {
        antInfo.scdBandFreq0 = scdBandFreq0;
    }

    private _setAntennaInfoscdBandFreq1(antInfo, scdBandFreq1) {
        antInfo.scdBandFreq1 = scdBandFreq1;
    }

    private _setAntennaInfoscdBandFreq2(antInfo, scdBandFreq2) {
        antInfo.scdBandFreq2 = scdBandFreq2;
    }

    private _setAntennaInfoscdBandFreq3(antInfo, scdBandFreq3) {
        antInfo.scdBandFreq3 = scdBandFreq3;
    }

    private _setAntennaInfosatName(antInfo, satName) {
        antInfo.satelliteName = satName;
        antInfo.satelliteType = satName;
    }

    private _setAntennaInfoposition(antInfo, position) {
        antInfo.position = position;
    }

    private _setAntennaInfolongitude(antInfo, longitude) {
        antInfo.longitude = longitude;
    }

    private _setAntennaInfolnbVoltage(antInfo, lnbVoltage) {
        antInfo.lnbVoltage = lnbVoltage;
    }

    private _antInfoCommit(antInfo) {
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
    }

    private _antInfoListsCommit() {
        var that = this;
        var antInfoList = that.oipfLocalSystemObj.antennaInfoLists;
        antInfoList.commit();
    }

    private _removeAntennaList() {
        var that = this;
        var antInfoLists = that.oipfLocalSystemObj.antennaInfoLists;
        hx.log('dsrvChannelSearch', '_removeAntennaList() called!!!');
        antInfoLists.removeAll();
        antInfoLists.commit();
    }

    private _removeAnt(curUsingSatelliteIdx) {
        var that = this, i;
        that._setAntennaId(0, curUsingSatelliteIdx);
        that._saveAntIdToLocal(curUsingSatelliteIdx);

        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            if (i > curUsingSatelliteIdx) {
                if (that._searchAntennaId[i] !== 0) {
                    that._moveDataFromTo(i, i - 1);
                }
            }
        }
    }

    private _moveDataFromTo(fromSatelliteIdx, toSatelliteIdx) {
        var that = this;
        that._searchSatelliteName[toSatelliteIdx] = that._searchSatelliteName[fromSatelliteIdx];
        that._userlnbFreq[toSatelliteIdx] = that._userlnbFreq[fromSatelliteIdx];
        that._antennaSetJson[toSatelliteIdx].searchType = that._antennaSetJson[fromSatelliteIdx].searchType;
        that._antennaSetJson[toSatelliteIdx].switchInput = that._antennaSetJson[fromSatelliteIdx].switchInput;
        that._antennaSetJson[toSatelliteIdx].lnbFreq = that._antennaSetJson[fromSatelliteIdx].lnbFreq;
        that._antennaSetJson[toSatelliteIdx].toneEnable = that._antennaSetJson[fromSatelliteIdx].toneEnable;
        that._antennaSetJson[toSatelliteIdx].diseqcVersion = that._antennaSetJson[fromSatelliteIdx].diseqcVersion;
        that._searchAntennaId[toSatelliteIdx] = that._searchAntennaId[fromSatelliteIdx];
        that._tsIndex[toSatelliteIdx] = that._tsIndex[fromSatelliteIdx];
        that._saveTpIdxToLocal(toSatelliteIdx);
        that._saveAntIdToLocal(toSatelliteIdx);

        that._searchSatelliteName[fromSatelliteIdx] = 'NONE';
        that._searchAntennaId[fromSatelliteIdx] = 0;
        that._userlnbFreq[fromSatelliteIdx] = 0;
        that._saveAntIdToLocal(fromSatelliteIdx);
        that._antennaSetJson[fromSatelliteIdx] = {
            searchType: 0,
            switchInput: 0,
            lnbFreq: 0,
            toneEnable: 0,
            diseqcVersion: 0
        };
        that._tsIndex[fromSatelliteIdx] = 0;
        that._saveTpIdxToLocal(fromSatelliteIdx);

        that._userTPParam[toSatelliteIdx].userFreq = that._userTPParam[fromSatelliteIdx].userFreq;
        that._userTPParam[toSatelliteIdx].userPola = that._userTPParam[fromSatelliteIdx].userPola;
        that._userTPParam[toSatelliteIdx].userSymbol = that._userTPParam[fromSatelliteIdx].userSymbol;
        that._userTPParam[toSatelliteIdx].userTrans = that._userTPParam[fromSatelliteIdx].userTrans;
        that._userTPParam[toSatelliteIdx].userFEC = that._userTPParam[fromSatelliteIdx].userFEC;
        that._saveAntIdToLocal(toSatelliteIdx);
        that._saveTpParamToLocal(toSatelliteIdx);
        that._userTPParam[fromSatelliteIdx] = {
            userFreq: '0',
            userPola: 'auto',
            userSymbol: '27500',
            userTrans: 'DVB',
            userFEC: 'auto'
        };
        that._saveTpParamToLocal(fromSatelliteIdx);
    }

    private _calcAntlength() {
        var that = this, i;
        that._antlength = 0;
        for (i = 0; i < that.SATELLITE_NUMBER_TO_MAKE; i += 1) {
            if ((that._searchSatelliteName[i] !== 'NONE') && (that._searchSatelliteName[i])) {
                that._antlength += 1;
            }
        }
    }

    private _makeAntennaWithDefault(satName) {
        var that = this;
        var antId = that._createAntennaInfo();
        var antInfo = that._getAntennaInfoWithId(antId);

        that._makeAntennaDefault(satName, antInfo);
        that._antInfoCommit(antInfo);

        return antId;
    }

    private _createAntennaInfo() {
        var that = this;
        var antInfoLists = that.oipfLocalSystemObj.antennaInfoLists;
        var antId = 0;
        antId = that.getAntennaIdByName(that._dfaultEmptyAntName);
        if (antId === 0) {
            antId = antInfoLists.createAntennaInfo();
        }
        return antId;
    }

    private _getAntennaInfoFromId(antId) {
        var that = this;
        if (antId === 0) {
            return null;
        }
        return that._getAntennaInfoWithId(antId);
    }

    private _getAntennaInfoWithId(antId) {
        var that = this;
        var antInfoLists = that.oipfLocalSystemObj.antennaInfoLists;
        var antInfo;

        antInfo = antInfoLists.getAntennaInfo(antId);

        return antInfo;
    }

    private _makeAntennaDefault(satName, antInfo) {
        antInfo.satelliteType = satName;
        antInfo.satelliteName = satName;
        antInfo.antennaType = antInfo.TYPE_LNB;
        antInfo.lnbFrequency = antInfo.LNBFREQ_UNIVERSAL;
    }

    private _setAntennaInfoWithMenu(antInfo, antennaSetJson, satName, userlnbFreq, tuner1Param, tuner2Param, tuner3Param, tuner4Param) {
        var that = this;
        var dInput, i;
        that._setAntennaInfosatName(antInfo, satName);
        switch (antennaSetJson.lnbFreq) {
            case -1:
            case 0:
                that._setAntennaInfolnbFreq(antInfo, antInfo.LNBFREQ_UNIVERSAL);
                break;
            case 1:
                that._setAntennaInfolnbFreq(antInfo, 9750);
                break;
            case 2:
                that._setAntennaInfolnbFreq(antInfo, 10600);
                break;
            case 3:
                that._setAntennaInfolnbFreq(antInfo, 5150);
                break;
            case 4:
                that._setAntennaInfolnbFreq(antInfo, 11475);
                break;
            case 5:
                that._setAntennaInfolnbFreq(antInfo, userlnbFreq & 0x7FFFFFFF);
                break;
            //default:	antInfo.lnbFrequency = antennaSetJson.lnbFreq & 0x7FFFFFFF;		break;
        }
        that._setAntennaInfoantennaType(antInfo, antInfo.TYPE_DISEQC);
        that._setAntennaInfoswitchInput(antInfo, antInfo.SW_N);
        if (antennaSetJson.searchType === 0) {
            that._setAntennaInfoantennaType(antInfo, antInfo.TYPE_LNB);
            that._setAntennaInfoswitchInput(antInfo, antInfo.SW_N);
        } else if (antennaSetJson.searchType === 1) {
            dInput = (antennaSetJson.switchInput) % 4;
            switch (dInput) {
                case 0:
                    that._setAntennaInfoswitchInput(antInfo, antInfo.SW_A);
                    break;
                case 1:
                    that._setAntennaInfoswitchInput(antInfo, antInfo.SW_B);
                    break;
                case 2:
                    that._setAntennaInfoswitchInput(antInfo, antInfo.SW_C);
                    break;
                case 3:
                    that._setAntennaInfoswitchInput(antInfo, antInfo.SW_D);
                    break;
            }
            if (antennaSetJson.switchInput < 4) {
                that._setAntennaInfodiseqcVersion(antInfo, antInfo.DISEQC_1_0);
            } else {
                that._setAntennaInfodiseqcVersion(antInfo, antInfo.DISEQC_2_0);
            }
        } else if (antennaSetJson.searchType === 2) {
            that._setAntennaInfoantennaType(antInfo, antInfo.TYPE_SCD);
            dInput = (antennaSetJson.switchInput) % 4;
            switch (dInput) {
                case 0:
                    that._setAntennaInfoswitchInput(antInfo, antInfo.SW_A);
                    break;
                case 1:
                    that._setAntennaInfoswitchInput(antInfo, antInfo.SW_B);
                    break;
                case 2:
                    that._setAntennaInfoswitchInput(antInfo, antInfo.SW_C);
                    break;
                case 3:
                    that._setAntennaInfoswitchInput(antInfo, antInfo.SW_D);
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
            that._setAntennaInfoscdBandNo0(antInfo, scdBandNo0);
            that._setAntennaInfoscdBandNo1(antInfo, scdBandNo1);
            that._setAntennaInfoscdBandNo2(antInfo, scdBandNo2);
            that._setAntennaInfoscdBandNo3(antInfo, scdBandNo3);
            that._setAntennaInfoscdBandFreq0(antInfo, parseInt(tuner1Param.scdBandFreq, 10));
            that._setAntennaInfoscdBandFreq1(antInfo, parseInt(tuner2Param.scdBandFreq, 10));
            that._setAntennaInfoscdBandFreq2(antInfo, parseInt(tuner3Param.scdBandFreq, 10));
            that._setAntennaInfoscdBandFreq3(antInfo, parseInt(tuner4Param.scdBandFreq, 10));
        }

        if (antennaSetJson.toneEnable === 0) {
            that._setAntennaInfoenable22KHzTone(antInfo, true);
        } else {
            that._setAntennaInfoenable22KHzTone(antInfo, false);
        }

        /* save sate posNum, satDegree */
        for (i = 0; i < that._satPosDataList.length; i += 1) {
            if (that._satPosDataList[i].satName === satName) {
                var position = that._satPosDataList[i].posNum;
                var longitude = that._satPosDataList[i].satDegree;
                break;
            }
        }
        that._setAntennaInfoposition(antInfo, position);
        that._setAntennaInfolongitude(antInfo, longitude);
        that._setAntennaInfolnbVoltage(antInfo, 0);
        //0 : standard,  1: high
        that._antInfoCommit(antInfo);
        that._antInfoListsCommit();
    }

    private _convertAntennaData(antInfo) {
        var convAntInfo = {
            satelliteName : null,
            satelliteType : null,
            lnbFrequency : null,
            userlnbFreq : null,
            switchInput : null,
            enable22KHzTone : null,
            antennaType : null,
            scdBandNo1 : null,
            scdBandNo2 : null,
            scdBandNo3 : null,
            scdBandNo4 : null,
            scdBandFreq1 : null,
            scdBandFreq2 : null,
            scdBandFreq3 : null,
            scdBandFreq4 : null
        };

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
        } else {
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
        } else {
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
    }

    private _SatNameConverter(name) {
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
    }

    private _initAllAntennaInfo() {
        var that = this;
        var len = 0, i, antInfoLists = that.oipfLocalSystemObj.antennaInfoLists, antInfo;

        antInfoLists.refresh();
        len = antInfoLists.length;
        for (i = 0; i < len; i += 1) {
            antInfo = antInfoLists.item(i);
            that._setAntennaInfoWithMenu(antInfo, that._antennaSetJson[0], that._searchSatelliteName[0], that._userlnbFreq[0], that._tuner1Param, that._tuner2Param, that._tuner3Param, that._tuner4Param);
        }
    }
    /////////////////////////// Manage Satellite END! //////////////////////////////////////////////
}

export = cDLCHANNEL_SEARCH_S;