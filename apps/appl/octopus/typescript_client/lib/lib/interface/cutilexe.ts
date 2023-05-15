/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CUtilExe extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Util.Exe', '/Octopus/Appkit/Util/Exe');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    BATCH_Stop(aCb: () => void) {
        this._call( function (iface){
            iface.BATCH_Stop['finish'] = function() {
                aCb();
            };
            iface.BATCH_Stop();
        });
    }

    BGEIT_Start_FreeSat(aCb: () => void) {
        this._call( function (iface){
            iface.BGEIT_Start_FreeSat['finish'] = function() {
                aCb();
            };
            iface.BGEIT_Start_FreeSat();
        });
    }

    BGEIT_Stop_FreeSat(aCb: () => void) {
        this._call( function (iface){
            iface.BGEIT_Stop_FreeSat['finish'] = function() {
                aCb();
            };
            iface.BGEIT_Stop_FreeSat();
        });
    }

    FTP_Start(aCb: () => void) {
        this._call( function (iface){
            iface.FTP_Start['finish'] = function() {
                aCb();
            };
            iface.FTP_Start();
        });
    }

    FTP_Stop(aCb: () => void) {
        this._call( function (iface){
            iface.FTP_Stop['finish'] = function() {
                aCb();
            };
            iface.FTP_Stop();
        });
    }

    FTP_GetStatus(aCb: (aFtpRun: number) => void) {
        this._call( function (iface){
            iface.FTP_GetStatus['finish'] = function(aFtpRun: number) {
                aCb(aFtpRun);
            };
            iface.FTP_GetStatus();
        });
    }

    StartFactoryDefault(aFactoryDefaultMode: number, aCb: () => void) {
        this._call( function (iface){
            iface.StartFactoryDefault['finish'] = function() {
                aCb();
            };
            iface.StartFactoryDefault(aFactoryDefaultMode);
        });
    }

    RemoveLcnUpdateInfo(aSyncMode: number, aCb: () => void) {
        this._call( function (iface){
            iface.RemoveLcnUpdateInfo['finish'] = function() {
                aCb();
            };
            iface.RemoveLcnUpdateInfo(aSyncMode);
        });
    }

    BML_Set(aBMLKeyControlMode: number, aCb: () => void) {
        this._call( function (iface){
            iface.BML_Set['finish'] = function() {
                aCb();
            };
            iface.BML_Set(aBMLKeyControlMode);
        });
    }

    RFVOD_Start(aVodMode: number, aUrl: string, aCb: () => void) {
        this._call( function (iface){
            iface.RFVOD_Start['finish'] = function() {
                aCb();
            };
            iface.RFVOD_Start(aVodMode, aUrl);
        });
    }

    RFVOD_Stop(aCb: () => void) {
        this._call( function (iface){
            iface.RFVOD_Stop['finish'] = function() {
                aCb();
            };
            iface.RFVOD_Stop();
        });
    }

    MHEG_Start(aWndId: number, aStillWndId: number, aCb: () => void) {
        this._call( function (iface){
            iface.MHEG_Start['finish'] = function() {
                aCb();
            };
            iface.MHEG_Start(aWndId, aStillWndId);
        });
    }

    MHEG_SetKey(aKey: number, aCb: () => void) {
        this._call( function (iface){
            iface.MHEG_SetKey['finish'] = function() {
                aCb();
            };
            iface.MHEG_SetKey(aKey);
        });
    }

    MHEG_DeepLink(aUrl: string, aPrevLcn: number, aCb: () => void) {
        this._call( function (iface){
            iface.MHEG_DeepLink['finish'] = function() {
                aCb();
            };
            iface.MHEG_DeepLink(aUrl, aPrevLcn);
        });
    }

    MHEG_ControlOP(aControl: number, aCb: () => void) {
        this._call( function (iface){
            iface.MHEG_ControlOP['finish'] = function() {
                aCb();
            };
            iface.MHEG_ControlOP(aControl);
        });
    }

    MHEGIB_RequestFile(aSessionId: number, aRequestNameLen: number, aRequestName: string, aCb: () => void) {
        this._call( function (iface){
            iface.MHEGIB_RequestFile['finish'] = function() {
                aCb();
            };
            iface.MHEGIB_RequestFile(aSessionId, aRequestNameLen, aRequestName);
        });
    }

    MHEGIB_RequestData(aSessionId: number, aRequestNameLen: number, aRequestName: string, aCb: () => void) {
        this._call( function (iface){
            iface.MHEGIB_RequestData['finish'] = function() {
                aCb();
            };
            iface.MHEGIB_RequestData(aSessionId, aRequestNameLen, aRequestName);
        });
    }

    MHEGIB_AbortByUser(aSessionId: number, aCb: () => void) {
        this._call( function (iface){
            iface.MHEGIB_AbortByUser['finish'] = function() {
                aCb();
            };
            iface.MHEGIB_AbortByUser(aSessionId);
        });
    }

    MHEGIB_AbortBySystem(aSessionId: number, aCb: () => void) {
        this._call( function (iface){
            iface.MHEGIB_AbortBySystem['finish'] = function() {
                aCb();
            };
            iface.MHEGIB_AbortBySystem(aSessionId);
        });
    }

    StartSatRecord(aCb: () => void) {
        this._call( function (iface){
            iface.StartSatRecord['finish'] = function() {
                aCb();
            };
            iface.StartSatRecord();
        });
    }

    TVTVEPG_Initdirectory(aCb: () => void) {
        this._call( function (iface){
            iface.TVTVEPG_Initdirectory['finish'] = function() {
                aCb();
            };
            iface.TVTVEPG_Initdirectory();
        });
    }

    Load_BaseEPG(aCb: () => void) {
        this._call( function (iface){
            iface.Load_BaseEPG['finish'] = function() {
                aCb();
            };
            iface.Load_BaseEPG();
        });
    }

    Load_TVTVEPG(aCb: () => void) {
        this._call( function (iface){
            iface.Load_TVTVEPG['finish'] = function() {
                aCb();
            };
            iface.Load_TVTVEPG();
        });
    }
}

export = CUtilExe