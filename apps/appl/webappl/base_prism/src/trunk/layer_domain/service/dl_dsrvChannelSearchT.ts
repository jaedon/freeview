/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvChannelSearch.js
/*jslint sloppy:true, nomen:true, vars:true */
//    CHANNEL_SEARCH_T: ['layer_domain/service/dl_dsrvChannelSearchT.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

class cDLCHANNEL_SEARCH_T extends __KERNEL__ implements ifSVCChannelSearchT {
    _transponderList : any = {};
    _isStartScan : boolean = false;
    _searchOptions : any = {
        network : 'true',
        scramble : 'All',
        channel : 'All'
    };

    _userTPParamT_AUSDTT : any = {
        userChannel : '6',
        userFreq : 177500,
        userTransmission : 'DVB-T',
        userBand : '7MHz',
        userNetworkSearch : 'Off'
    };
    _userTestTPParamT_AUSDTT : any = {
        userChannel : '6',
        userFreq : 177500,
        userTransmission : 'DVB-T',
        userBand : '7MHz',
        userNetworkSearch : 'Off'
    };
    _userTempTPParamT_AUSDTT : any = {
        userChannel : '6',
        userFreq : 177500,
        userTransmission : 'DVB-T',
        userBand : '7MHz',
        userNetworkSearch : 'Off'
    };
    _userTPParamT_UKDTT : any = {
        userChannel : '21',
        userFreq : 474000,
        userTransmission : 'DVB-T',
        userBand : '8MHz',
        userNetworkSearch : 'Off'
    };
    _userTestTPParamT_UKDTT : any = {
        userChannel : '21',
        userFreq : 474000,
        userTransmission : 'DVB-T',
        userBand : '8MHz',
        userNetworkSearch : 'Off'
    };
    _userTempTPParamT_UKDTT : any = {
        userChannel : '21',
        userFreq : 474000,
        userTransmission : 'DVB-T',
        userBand : '8MHz',
        userNetworkSearch : 'Off'
    };
    oipfChannelConfigObj : any;
    oipfLocalSystemObj : any;
    oipfMastConfigurationObj : any;
    oipfTrdConflict: any;

    ////////////////////// srv Data END /////////////////////

    constructor(aServiceName: string) {
        super(aServiceName);
        this.defineEvents(['channelScan']);
        this.init();
    }

    init () {
        this.oipfChannelConfigObj = hx.stbObject.channelConfig;
        this.oipfMastConfigurationObj = hx.stbObject.configuration;
        this.oipfLocalSystemObj = this.oipfMastConfigurationObj.localSystem;
        this.oipfTrdConflict = this.oipfLocalSystemObj.trdconflict;
        this._loadTpDataT();
        this._getTpParamFromLocalT();

        //hx.logadd('dsrvChannelSearch');
        hx.log('dsrvChannelSearch', 'create');
    }

    // ####################################################################################################
    // Private Methods
    /////////////////////////// Initialize START ///////////////////////////////////
    _loadTpDataT (aCallback?, aUrlPrefix?) {
        this._transponderList = hx.config.settings.channelgroupT;
    }
    /////////////////////////// Initialize END! ///////////////////////////////////////

    //////////////////////  Save to WebStorage & get from WebStorage START ////////////////////////////
    _getTpParamFromLocalT () {
        var that = this, userTpParamObj : any;

        if (ENV.op === ENV.listOP.AUSDTT){
            userTpParamObj = hx.svc.WEB_STORAGE.getUserTpParamFromLocalT();
            if (userTpParamObj.userChannel !== null && userTpParamObj.userChannel !== undefined) {
                that._userTPParamT_AUSDTT.userChannel = userTpParamObj.userChannel;
            }
            if (userTpParamObj.userFreq !== null && userTpParamObj.userFreq !== undefined) {
                that._userTPParamT_AUSDTT.userFreq = userTpParamObj.userFreq;
            }
            if (userTpParamObj.userBand !== null && userTpParamObj.userBand !== undefined) {
                that._userTPParamT_AUSDTT.userBand = userTpParamObj.userBand;
            }
            if (userTpParamObj.userNetworkSearch !== null && userTpParamObj.userNetworkSearch !== undefined) {
                that._userTPParamT_AUSDTT.userNetworkSearch = userTpParamObj.userNetworkSearch;
            }
        }
        else if (ENV.op === ENV.listOP.UKDTT){
            userTpParamObj = hx.svc.WEB_STORAGE.getUserTpParamFromLocalT2();
            if (userTpParamObj.userChannel !== null && userTpParamObj.userChannel !== undefined) {
                that._userTPParamT_UKDTT.userChannel = userTpParamObj.userChannel;
            }
            if (userTpParamObj.userFreq !== null && userTpParamObj.userFreq !== undefined) {
                that._userTPParamT_UKDTT.userFreq = userTpParamObj.userFreq;
            }
            if (userTpParamObj.userTransmission !== null && userTpParamObj.userTransmission !== undefined) {
                that._userTPParamT_UKDTT.userTransmission = userTpParamObj.userTransmission;
            }
            if (userTpParamObj.userBand !== null && userTpParamObj.userBand !== undefined) {
                that._userTPParamT_UKDTT.userBand = userTpParamObj.userBand;
            }
            if (userTpParamObj.userNetworkSearch !== null && userTpParamObj.userNetworkSearch !== undefined) {
                that._userTPParamT_UKDTT.userNetworkSearch = userTpParamObj.userNetworkSearch;
            }
        }
    }
    _saveTpParamToLocalT () {
        if (ENV.op === ENV.listOP.AUSDTT){
            hx.svc.WEB_STORAGE.saveUserTpParamToLocalT({
                userChannel : this._userTPParamT_AUSDTT.userChannel,
                userFreq : this._userTPParamT_AUSDTT.userFreq,
                userBand : this._userTPParamT_AUSDTT.userBand,
                userNetworkSearch : this._userTPParamT_AUSDTT.userNetworkSearch
            });
        }
        else if (ENV.op === ENV.listOP.UKDTT){
            hx.svc.WEB_STORAGE.saveUserTpParamToLocalT2({
                userChannel : this._userTPParamT_UKDTT.userChannel,
                userFreq : this._userTPParamT_UKDTT.userFreq,
                userTransmission : this._userTPParamT_UKDTT.userTransmission,
                userBand : this._userTPParamT_UKDTT.userBand,
                userNetworkSearch : this._userTPParamT_UKDTT.userNetworkSearch
            });
        }
    }
    //////////////////////  Save to WebStorage & get from WebStorage END! ////////////////////////////

    ////////////////////// Channel Scan Control START ////////////////////////////////////////////////
    _startTerScan (aParam) {
        if (aParam.isStartScan) {
            this.oipfChannelConfigObj.stopScan();
            this._isStartScan = false;
        }
        var scanParamCollection : any = this._createChannelScanParamCollectionObject();
        var channelScanOptions : any = this._createChannelScanOptionsObject();
        var userParam : any = aParam.usrParam;
        var confParam : any = {};
        var isAuto : boolean = aParam.isAuto;
        var tpList : any = [], j : number, tpInfo : any;

        var _convertTpInfoOipf= (aItemInfo)=> {
            var modulation, oipfTpInfo;
            oipfTpInfo = {};
            //  startFrequency
            oipfTpInfo.startFrequency = aItemInfo.userFreq;
            //	endFrequency
            oipfTpInfo.endFrequency = aItemInfo.userFreq;
            oipfTpInfo.transmission = aItemInfo.userTransmission;
            //	raster
            oipfTpInfo.raster = 0;
            //	modulationModes
            oipfTpInfo.modulationModes = 0;
            //	ofdm
            oipfTpInfo.ofdm = 'auto';
            //	bandwidth
            if (aItemInfo.userBand === '7MHz') {
                oipfTpInfo.bandwidth = '7Mhz';
            } else if (aItemInfo.userBand === '8MHz') {
                oipfTpInfo.bandwidth = '8Mhz';
            }
            return oipfTpInfo;
        }

        var _convertAllTpInfoOipf = (aItemInfoFreq, aItemTransmission, aItemInfoBandwith)=> {
            var modulation, oipfTpInfo;
            oipfTpInfo = {};
            //  startFrequency
            oipfTpInfo.startFrequency = aItemInfoFreq;
            //	endFrequency
            oipfTpInfo.endFrequency = aItemInfoFreq;
            oipfTpInfo.transmission = aItemTransmission;
            //	raster
            oipfTpInfo.raster = 0;
            //	modulationModes
            oipfTpInfo.modulationModes = 0;
            //	ofdm
            oipfTpInfo.ofdm = 'auto';
            //	bandwidth
            if (aItemInfoBandwith === '7MHz') {
                oipfTpInfo.bandwidth = '7Mhz';
            } else if (aItemInfoBandwith === '8MHz') {
                oipfTpInfo.bandwidth = '8Mhz';
            }

            return oipfTpInfo;
        }

        if (isAuto) {//Automatic Search
            if (ENV.op === ENV.listOP.AUSDTT)  {
                tpList[0] = userParam.bandwidth7MHz;
                tpList[1] = userParam.bandwidth8MHz;

                for ( j = 0; j < userParam.bandwidth7MHz.centreFrequency.length; j+=1) {
                    tpInfo = tpList[0].centreFrequency[j];
                    confParam = _convertAllTpInfoOipf(tpInfo, 'DVB-T', tpList[0].bandwith);
                    this._addTerScanParameters(confParam.startFrequency, confParam.endFrequency, 'DVB-T', confParam.raster, confParam.modulationModes, confParam.ofdm, confParam.bandwidth, scanParamCollection);
                }

                if (!hx.config.isFieldSearch) {
                    for ( j = 0; j < userParam.bandwidth8MHz.centreFrequency.length; j+=1) {
                        tpInfo = tpList[1].centreFrequency[j];
                        confParam = _convertAllTpInfoOipf(tpInfo, userParam.userTransmission, tpList[0].bandwith);
                        this._addTerScanParameters(confParam.startFrequency, confParam.endFrequency, 'DVB-T', confParam.raster, confParam.modulationModes, confParam.ofdm, confParam.bandwidth, scanParamCollection);
                    }
                }
            }
            else if (ENV.op === ENV.listOP.UKDTT){
                tpList[0] = userParam.bandwidth8MHz;
                //tpList[1] = userParam.bandwidth8MHz;

                for ( j = 0; j < userParam.bandwidth8MHz.centreFrequency.length; j+=1) {
                    tpInfo = tpList[0].centreFrequency[j];
                    confParam = _convertAllTpInfoOipf(tpInfo, userParam.userTransmission, tpList[0].bandwith);
                    this._addTerScanParameters(confParam.startFrequency, confParam.endFrequency, 'DVB-T', confParam.raster, confParam.modulationModes, confParam.ofdm, confParam.bandwidth, scanParamCollection);
                }
                for ( j = 0; j < userParam.bandwidth8MHz.centreFrequency.length; j+=1) {
                    tpInfo = tpList[0].centreFrequency[j];
                    confParam = _convertAllTpInfoOipf(tpInfo, userParam.userTransmission, tpList[0].bandwith);
                    this._addTerScanParameters(confParam.startFrequency, confParam.endFrequency, 'DVB-T2', confParam.raster, confParam.modulationModes, confParam.ofdm, confParam.bandwidth, scanParamCollection);
                }
/*
                if (!hx.config.isFieldSearch) {
                    for ( j = 0; j < userParam.bandwidth8MHz.centreFrequency.length; j+=1) {
                        tpInfo = tpList[1].centreFrequency[j];
                        confParam = _convertAllTpInfoOipf(tpInfo, userParam.userTransmission, tpList[0].bandwith);
                        this._addTerScanParameters(confParam.startFrequency, confParam.endFrequency, 'DVB-T', confParam.raster, confParam.modulationModes, confParam.ofdm, confParam.bandwidth, scanParamCollection);
                    }
                    for ( j = 0; j < userParam.bandwidth8MHz.centreFrequency.length; j+=1) {
                        tpInfo = tpList[1].centreFrequency[j];
                        confParam = _convertAllTpInfoOipf(tpInfo, userParam.userTransmission, tpList[0].bandwith);
                        this._addTerScanParameters(confParam.startFrequency, confParam.endFrequency, 'DVB-T2', confParam.raster, confParam.modulationModes, confParam.ofdm, confParam.bandwidth, scanParamCollection);
                    }
                }
*/
            }
            this._setScanOptionsNetwork(true, channelScanOptions);
        } else {//Manual Search
            confParam = _convertTpInfoOipf(userParam);
            this._addTerScanParameters(confParam.startFrequency, confParam.endFrequency, confParam.transmission, confParam.raster, confParam.modulationModes, confParam.ofdm, confParam.bandwidth, scanParamCollection);
            this._setScanOptionsNetwork((userParam.userNetworkSearch === 'On' ? true : false), channelScanOptions);
        }

        switch (aParam.searchOptions.channel) {
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
                // ALL
                break;
        }

        this.oipfChannelConfigObj.onChannelScan = aParam.fn;
        this.oipfChannelConfigObj.startScan(channelScanOptions, scanParamCollection);
        this._isStartScan = true;
    }

    _addTerScanParameters (aStartFrequency, aEndFrequency, aTransmission, aRaster, aModulationModes, aOfdm, aBandwidth, aScanParamCollection) {
        var channelParams = this.oipfChannelConfigObj.createChannelScanParametersObject(12);
        channelParams = this._settingTerChannelScanParameters(aStartFrequency, aEndFrequency, aTransmission, aRaster, aModulationModes, aOfdm, aBandwidth, channelParams);
        //hx.logadd('sh');
        // hx.log('sh','############ ChannelScanParam #############');
        // hx.log('sh','## channelParams.startFrequency = ['+channelParams.startFrequency+']');
        // hx.log('sh','## channelParams.endFrequency = ['+channelParams.endFrequency+']');
        // hx.log('sh','## channelParams.raster = ['+channelParams.raster+']');
        // hx.log('sh','## channelParams.modulationModes = ['+channelParams.modulationModes+']');
        // hx.log('sh','## channelParams.ofdm = ['+channelParams.ofdm+']');
        // hx.log('sh','## channelParams.bandwidth = ['+channelParams.bandwidth+']');
        // hx.log('sh','## channelParams.antennaId = ['+channelParams.antennaId+']');
        // hx.log('sh','## channelParams.antennaName = ['+channelParams.antennaName+']');
        // hx.log('sh','############################################');
        aScanParamCollection.addChannelScanParam(channelParams);
    }

    _settingTerChannelScanParameters (aStartFrequency, aEndFrequency, aTransmission, aRaster, aModulationModes, aOfdm, aBandwidth, aChannelParams) {
        aChannelParams.startFrequency = aStartFrequency;
        aChannelParams.endFrequency = aEndFrequency;
        aChannelParams.transmission = aTransmission;
        aChannelParams.raster = aRaster;
        aChannelParams.modulationModes = aModulationModes;
        aChannelParams.ofdm = aOfdm;
        aChannelParams.bandwidth = aBandwidth;
        return aChannelParams;
    }

    _startTerTune (aParam) {
        var userParam : any = aParam.usrParam;
        if (aParam.isStartScan) {
            this.oipfChannelConfigObj.stopScan();
            this._isStartScan = false;
        }
        var channelScanParams : any = this.oipfChannelConfigObj.createChannelScanParametersObject(12);

        var  _convertTpInfoOipf = (itemInfo) => {
            var modulation : any, oipfTpInfo : any;
            oipfTpInfo = {};
            //  startFrequency
            oipfTpInfo.startFrequency = itemInfo.userFreq;
            //	endFrequency
            oipfTpInfo.endFrequency = itemInfo.userFreq;
            oipfTpInfo.transmission = itemInfo.userTransmission;
            //	raster
            oipfTpInfo.raster = 0;
            //	modulationModes
            oipfTpInfo.modulationModes = 0;
            //	ofdm
            oipfTpInfo.ofdm = 'auto';
            //	bandwidth
            if (itemInfo.userBand === '7MHz') {
                oipfTpInfo.bandwidth = '7Mhz';
            } else if (itemInfo.userBand === '8MHz') {
                oipfTpInfo.bandwidth = '8Mhz';
            }
            return oipfTpInfo;
        }

        var confParam = _convertTpInfoOipf(userParam);
        this._settingTerChannelScanParameters(confParam.startFrequency, confParam.endFrequency, confParam.transmission, confParam.raster, confParam.modulationModes, confParam.ofdm, confParam.bandwidth, channelScanParams);
        this.oipfChannelConfigObj.startScan(null, channelScanParams, 'tuneonly');
        this._isStartScan = true;
        return true;
    }

    _createChannelScanParamCollectionObject () {
        var scanParamCollection : any = this.oipfChannelConfigObj.createChannelScanParametersObject();
        return scanParamCollection;
    }
    _createChannelScanOptionsObject () {
        var channelScanOptions : any = this.oipfChannelConfigObj.createChannelScanOptionsObject();
        return channelScanOptions;
    }
    _setScanOptionsChannelType (channelType, channelScanOptions) {
        channelScanOptions.channelType = channelType;
    }
    _setScanOptionsNetwork (network, channelScanOptions) {
        channelScanOptions.enableNetworkScan = network;
    }

    /////////////////////////*   Channel Scan Control START *//////////////////////////////////////

    StartTerChannelScan (aParam) {
        var fn = (type, progress, frequency, signalStrength, channelNumber, channelType, channelCount, transponderCount, owSearchedInfos, tpInfo) => {
            this.fireEvent('channelScan', {
                progress : progress,
                channelNumber : channelNumber,
                channelCount : channelCount,
                transponderCount : transponderCount,
                signalStrength : signalStrength,
                tpInfo : tpInfo,
                owSearchedInfos : owSearchedInfos
            });
        };

        if (aParam.isAuto) {
            this._startTerScan({
                searchOptions : this._searchOptions,
                fn : fn,
                usrParam : this._transponderList,
                isAuto : aParam.isAuto,
                isStartScan : this._isStartScan
            });
        } else {
            if (ENV.op === ENV.listOP.AUSDTT){
                this._startTerScan({
                    searchOptions : this._searchOptions,
                    fn : fn,
                    usrParam : this._userTPParamT_AUSDTT,
                    isAuto : aParam.isAuto,
                    isStartScan : this._isStartScan
                });
            }
            else if (ENV.op === ENV.listOP.UKDTT)
            {
                this._startTerScan({
                    searchOptions : this._searchOptions,
                    fn : fn,
                    usrParam : this._userTPParamT_UKDTT,
                    isAuto : aParam.isAuto,
                    isStartScan : this._isStartScan
                });
            }
        }
    }

    StartTerTune () {
        if (ENV.op === ENV.listOP.AUSDTT){
            this._startTerTune({
                usrParam : this._userTempTPParamT_AUSDTT,
                isStartScan : this._isStartScan
            });
        }
        else if (ENV.op === ENV.listOP.UKDTT) {
            this._startTerTune({
                usrParam : this._userTempTPParamT_UKDTT,
                isStartScan : this._isStartScan
            });
        }
    }

    loadChannelSearchResult () {
        this.oipfChannelConfigObj.controlScan('load');
    }

    saveChannelSearchResult () {
        this.oipfChannelConfigObj.controlScan('save');
        this.oipfChannelConfigObj.stopScan();
        this._isStartScan = false;
    }
    stopScan () {
        this.oipfChannelConfigObj.stopScan();
        this._isStartScan = false;
    }
    pauseScan () {
        this.oipfChannelConfigObj.controlScan('pause');
    }
    resumeScan () {
        this.oipfChannelConfigObj.controlScan('resume');
    }
    isStartScan() {
        return this._isStartScan;
    }

    /////////////////////////*   Channel Scan Control END *//////////////////////////////////////

    ////////////////////////*  Manage Satellite END  */////////////////////////////////////////

    setUserTPParam (aParam) {
        var paramToSet : any;

        if (ENV.op === ENV.listOP.AUSDTT){
            paramToSet = this._userTPParamT_AUSDTT;
        }
        else if (ENV.op === ENV.listOP.UKDTT){
            paramToSet = this._userTPParamT_UKDTT;
            paramToSet.userTransmission = aParam.userTransmission;
        }

        paramToSet.userChannel = aParam.userChannel;
        paramToSet.userFreq = aParam.userFreq;
        paramToSet.userBand = aParam.userBand;
        paramToSet.userNetworkSearch = aParam.userNetworkSearch;

        this._saveTpParamToLocalT();
    }

    setUserTestTPParam (aParam) {
        if (ENV.op === ENV.listOP.AUSDTT){
            this._userTestTPParamT_AUSDTT.userChannel = aParam.userChannel;
            this._userTestTPParamT_AUSDTT.userFreq = aParam.userFreq;
            this._userTestTPParamT_AUSDTT.userBand = aParam.userBand;
            this._userTestTPParamT_AUSDTT.userNetworkSearch = aParam.userNetworkSearch;
        }
        else if (ENV.op === ENV.listOP.UKDTT){
            this._userTestTPParamT_UKDTT.userChannel = aParam.userChannel;
            this._userTestTPParamT_UKDTT.userFreq = aParam.userFreq;
            this._userTestTPParamT_UKDTT.userTransmission = aParam.userTransmission;
            this._userTestTPParamT_UKDTT.userBand = aParam.userBand;
            this._userTestTPParamT_UKDTT.userNetworkSearch = aParam.userNetworkSearch;
        }
    }

    setUserTempTPParam (aParam) {
        if (ENV.op === ENV.listOP.AUSDTT){
            this._userTempTPParamT_AUSDTT.userChannel = aParam.userChannel;
            this._userTempTPParamT_AUSDTT.userFreq = aParam.userFreq;
            this._userTempTPParamT_AUSDTT.userBand = aParam.userBand;
            this._userTempTPParamT_AUSDTT.userNetworkSearch = aParam.userNetworkSearch;
        }
        else if (ENV.op === ENV.listOP.UKDTT){
            this._userTempTPParamT_UKDTT.userChannel = aParam.userChannel;
            this._userTempTPParamT_UKDTT.userFreq = aParam.userFreq;
            this._userTempTPParamT_UKDTT.userTransmission = aParam.userTransmission;
            this._userTempTPParamT_UKDTT.userBand = aParam.userBand;
            this._userTempTPParamT_UKDTT.userNetworkSearch = aParam.userNetworkSearch;
        }
    }

    getUserTPParam () {
        var tpParam : any = {};

        if (ENV.op === ENV.listOP.AUSDTT){
            tpParam = this._userTPParamT_AUSDTT;
        }
        else if (ENV.op === ENV.listOP.UKDTT){
            tpParam = this._userTPParamT_UKDTT;
        }

        return {
            tpParam : tpParam
        };
    }
    /////////////////////* Manage Transponder List END *//////////////////////////////////
    getTrdConflict(aDepth: number, aCallback: (aCount, aList) => void): void {
        var trdConflict = this.oipfTrdConflict;
        trdConflict.onTrdConflict = (aEvent: number, aCount: number, aString: string) => {
            var list: string[] = [];
            var devider = ",";
            if (aCount && aString) {
                list = aString.split(devider);
                list.length = aCount;
            }
            aCallback(aCount, list);
        };
        trdConflict.getTrdConflict(aDepth);
    }
    setTrdConflict(aDepth: number, aIndex: number): void {
        var localSystem = this.getPluginInstance("LocalSystem");
        var trdConflict = localSystem.trdconflict;
        trdConflict.setTrdConflict(aDepth, aIndex);
    }
}
export = cDLCHANNEL_SEARCH_T;

