/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CUtilBuilder extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Util.Builder', '/Octopus/Appkit/Util/Builder');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number, param4: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number, param4: number) {
                aCb(msgInt, msgStr, param1, param2, param3, param4);
            });
        });
    }

    OpenSession(aDeliveryType: number, aScanType: string, aActionInfo: number[],aCb: (aBuilderHandle: number) => void) {
        this._call( function (iface){
            iface.OpenSession['finish'] = function(aBuilderHandle: number) {
                aCb(aBuilderHandle);
            };
            iface.OpenSession(aDeliveryType, aScanType, aActionInfo);
        });
    }

    CloseSession(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.CloseSession['finish'] = function() {
                aCb();
            };
            iface.CloseSession(aBuilderHandle);
        });
    }

    ClearBuildInfo(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.ClearBuildInfo['finish'] = function() {
                aCb();
            };
            iface.ClearBuildInfo(aBuilderHandle);
        });
    }

    AddBuildInfo_Sat(aBuilderHandle: number, aTunerParam: tdata.TTunerParamSat, aCb: () => void) {
        this._call( function (iface){
            iface.AddBuildInfo_Sat['finish'] = function() {
                aCb();
            };
            iface.AddBuildInfo_Sat(aBuilderHandle, aTunerParam);
        });
    }

    AddBuildInfo_Ter(aBuilderHandle: number, aTunerParam: tdata.TTunerParamTer, aCb: () => void) {
        this._call( function (iface){
            iface.AddBuildInfo_Ter['finish'] = function() {
                aCb();
            };
            iface.AddBuildInfo_Ter(aBuilderHandle, aTunerParam);
        });
    }

    AddBuildInfo_Cab(aBuilderHandle: number, aTunerParam: tdata.TTunerParamCab, aCb: () => void) {
        this._call( function (iface){
            iface.AddBuildInfo_Cab['finish'] = function() {
                aCb();
            };
            iface.AddBuildInfo_Cab(aBuilderHandle, aTunerParam);
        });
    }

    RemoveBuildInfo(aBuilderHandle: number, aBuildInfo: number, aCb: () => void) {
        this._call( function (iface){
            iface.RemoveBuildInfo['finish'] = function() {
                aCb();
            };
            iface.RemoveBuildInfo(aBuilderHandle, aBuildInfo);
        });
    }

    GetOption(aBuilderHandle: number, aCb: (aBuildInfo: number[]) => void) {
        this._call( function (iface){
            iface.GetOption['finish'] = function(data) {
                var buildInfo = [];
                data.forEach((s) => {
                    buildInfo.push(s);
                });
                aCb(buildInfo);
            };
            iface.GetOption(aBuilderHandle);
        });
    }

    SetOption(aBuilderHandle: number, aBuildInfo: number[], aCb: () => void) {
        this._call( function (iface){
            iface.SetOption['finish'] = function() {
                aCb();
            };
            iface.SetOption(aBuilderHandle, aBuildInfo);
        });
    }

    GetBuilderStatus(aBuilderHandle: number, aCb: (aTotalTuningNum: number, aCompleteTuningNum: number, aSearchedTuningNum: number, aSearchState: number, aFrequency: number, aDeliveryType: number, aTunerParam: number) => void) {
        this._call( function (iface){
            iface.GetBuilderStatus['finish'] = function(data) {
                aCb(data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
            };
            iface.GetBuilderStatus(aBuilderHandle);
        });
    }

    GetBuilderSignalStatus(aBuilderHandle: number, aCb: (aSignalStrength: number, aSignalQuality: number) => void) {
        this._call( function (iface){
            iface.GetBuilderSignalStatus['finish'] = function(data) {
                aCb(data[0], data[1]);
            };
            iface.GetBuilderSignalStatus(aBuilderHandle);
        });
    }

    GetBuilderSvcInfo(aBuilderHandle: number, aStartIndex: number, aCount: number, aCb: (aSvcInfoList: ArrayBuffer) => void) {
        this._call( function (iface){
            iface.GetBuilderSvcInfo['finish'] = function(data) {
                //aCb();
            };
            iface.GetBuilderSvcInfo(aBuilderHandle, aStartIndex, aCount);
        });
    }

    GetScanType(aBuilderHandle: number, aCb: (aScanType: string) => void) {
        this._call( function (iface){
            iface.GetScanType['finish'] = function(aScanType: string) {
                aCb(aScanType);
            };
            iface.GetScanType(aBuilderHandle);
        });
    }

    StartScan(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.StartScan['finish'] = function() {
                aCb();
            };
            iface.StartScan(aBuilderHandle);
        });
    }

    StopScan(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.StopScan['finish'] = function() {
                aCb();
            };
            iface.StopScan(aBuilderHandle);
        });
    }

    PauseScan(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.PauseScan['finish'] = function() {
                aCb();
            };
            iface.PauseScan(aBuilderHandle);
        });
    }

    ResumeScan(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.ResumeScan['finish'] = function() {
                aCb();
            };
            iface.ResumeScan(aBuilderHandle);
        });
    }

    SetAdditionalNumbers(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetAdditionalNumbers['finish'] = function() {
                aCb();
            };
            iface.SetAdditionalNumbers(aBuilderHandle);
        });
    }

    MotorGotoPosition(aBuilderHandle: number, aPosition: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorGotoPosition['finish'] = function() {
                aCb();
            };
            iface.MotorGotoPosition(aBuilderHandle, aPosition);
        });
    }

    MotorStorePosition(aBuilderHandle: number, aPosition: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorStorePosition['finish'] = function() {
                aCb();
            };
            iface.MotorStorePosition(aBuilderHandle, aPosition);
        });
    }

    MotorResetPosition(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorResetPosition['finish'] = function() {
                aCb();
            };
            iface.MotorResetPosition(aBuilderHandle);
        });
    }

    MotorRecalcuate(aBuilderHandle: number, aPosition: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorRecalcuate['finish'] = function() {
                aCb();
            };
            iface.MotorRecalcuate(aBuilderHandle, aPosition);
        });
    }

    MotorMove(aBuilderHandle: number, aDirection: number, aMode: number, aStep: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorMove['finish'] = function() {
                aCb();
            };
            iface.MotorMove(aBuilderHandle, aDirection, aMode, aStep);
        });
    }

    MotorStop(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorStop['finish'] = function() {
                aCb();
            };
            iface.MotorStop(aBuilderHandle);
        });
    }

    MotorSetLimit(aBuilderHandle: number, aDirection: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorSetLimit['finish'] = function() {
                aCb();
            };
            iface.MotorSetLimit(aBuilderHandle, aDirection);
        });
    }

    MotorEnableLimit(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorEnableLimit['finish'] = function() {
                aCb();
            };
            iface.MotorEnableLimit(aBuilderHandle);
        });
    }

    MotorDisableLimit(aBuilderHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorDisableLimit['finish'] = function() {
                aCb();
            };
            iface.MotorDisableLimit(aBuilderHandle);
        });
    }

    MotorGotoDegree(aBuilderHandle: number, aDegree: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorGotoDegree['finish'] = function() {
                aCb();
            };
            iface.MotorGotoDegree(aBuilderHandle, aDegree);
        });
    }

    MotorGotoSatellite(aBuilderHandle: number, aSatellite: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorGotoSatellite['finish'] = function() {
                aCb();
            };
            iface.MotorGotoSatellite(aBuilderHandle, aSatellite);
        });
    }

    MotorSetMyLongitude(aNewLongitude: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorSetMyLongitude['finish'] = function() {
                aCb();
            };
            iface.MotorSetMyLongitude(aNewLongitude);
        });
    }

    MotorSetMyLatitude(aNewLatitude: number, aCb: () => void) {
        this._call( function (iface){
            iface.MotorSetMyLatitude['finish'] = function() {
                aCb();
            };
            iface.MotorSetMyLatitude(aNewLatitude);
        });
    }

    MotorGetMyLongitude(aCb: (aMyLongitude: number) => void) {
        this._call( function (iface){
            iface.MotorGetMyLongitude['finish'] = function(aMyLongitude: number) {
                aCb(aMyLongitude);
            };
            iface.MotorGetMyLongitude();
        });
    }

    MotorGetMyLatitude(aCb: (aMyLatitude: number) => void) {
        this._call( function (iface){
            iface.MotorGetMyLatitude['finish'] = function(aMyLatitude: number) {
                aCb(aMyLatitude);
            };
            iface.MotorGetMyLatitude();
        });
    }
}

export = CUtilBuilder