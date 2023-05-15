/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CSysinfoStb extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Sysinfo.Stb', '/Octopus/Appkit/Sysinfo/Stb');
    }

    GetModelName(aCb: (aModelName: string) => void) {
        this._call( function (iface){
            iface.GetModelName['finish'] = function(aModelName: string) {
                aCb(aModelName);
            };
            iface.GetModelName();
        });
    }

    GetUpdatedList(aCb: (aUpdateList: string) => void) {
        this._call( function (iface){
            iface.GetUpdatedList['finish'] = function(aUpdateList: string) {
                aCb(aUpdateList);
            };
            iface.GetUpdatedList();
        });
    }

    GetSoftwareVersion(aCb: (aSoftwareVersion: string) => void) {
        this._call( function (iface){
            iface.GetSoftwareVersion['finish'] = function(aSoftwareVersion: string) {
                aCb(aSoftwareVersion);
            };
            iface.GetSoftwareVersion();
        });
    }

    GetLoaderName(aCb: (aLoaderName: string) => void) {
        this._call( function (iface){
            iface.GetLoaderName['finish'] = function(aLoaderName: string) {
                aCb(aLoaderName);
            };
            iface.GetLoaderName();
        });
    }

    GetLoaderVersion(aCb: (aLoaderVersion: string) => void) {
        this._call( function (iface){
            iface.GetLoaderVersion['finish'] = function(aLoaderVersion: string) {
                aCb(aLoaderVersion);
            };
            iface.GetLoaderVersion();
        });
    }

    GetManufacturerName(aCb: (aManufacturerName: string) => void) {
        this._call( function (iface){
            iface.GetManufacturerName['finish'] = function(aManufacturerName: string) {
                aCb(aManufacturerName);
            };
            iface.GetManufacturerName();
        });
    }

    GetSystemId(aCb: (aSystemId: string) => void) {
        this._call( function (iface){
            iface.GetSystemId['finish'] = function(aSystemId: string) {
                aCb(aSystemId);
            };
            iface.GetSystemId();
        });
    }

    GetDeviceId(aCb: (aDeviceId: string) => void) {
        this._call( function (iface){
            iface.GetDeviceId['finish'] = function(aDeviceId: string) {
                aCb(aDeviceId);
            };
            iface.GetDeviceId();
        });
    }

    GetChipId(aCb: (aChipId: string) => void) {
        this._call( function (iface){
            iface.GetChipId['finish'] = function(aChipId: string) {
                aCb(aChipId);
            };
            iface.GetChipId();
        });
    }

    GetObamaVersion(aCb: (aObamaVersion: string) => void) {
        this._call( function (iface){
            iface.GetObamaVersion['finish'] = function(aObamaVersion: string) {
                aCb(aObamaVersion);
            };
            iface.GetObamaVersion();
        });
    }

    GetMicomVersion(aCb: (aMicomVersion: string) => void) {
        this._call( function (iface){
            iface.GetMicomVersion['finish'] = function(aMicomVersion: string) {
                aCb(aMicomVersion);
            };
            iface.GetMicomVersion();
        });
    }

    GetUpdateDate(aCb: (aUpdateDate: string) => void) {
        this._call( function (iface){
            iface.GetUpdateDate['finish'] = function(aUpdateDate: string) {
                aCb(aUpdateDate);
            };
            iface.GetUpdateDate();
        });
    }

    GetSerialNumber(aCb: (aSerialNo: string) => void) {
        this._call( function (iface){
            iface.GetSerialNumber['finish'] = function(aSerialNo: string) {
                aCb(aSerialNo);
            };
            iface.GetSerialNumber();
        });
    }

    GetCustomCode(aCb: (aCustomCode: number) => void) {
        this._call( function (iface){
            iface.GetCustomCode['finish'] = function(aCustomCode: number) {
                aCb(aCustomCode);
            };
            iface.GetCustomCode();
        });
    }

    GetColdBootDone(aCb: (aCheckColdBootDone: number) => void) {
        this._call( function (iface){
            iface.GetColdBootDone['finish'] = function(aCheckColdBootDone: number) {
                aCb(aCheckColdBootDone);
            };
            iface.GetColdBootDone();
        });
    }

    SetFirstBootOnProduction(aFirstBootOnProduction: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetFirstBootOnProduction['finish'] = function() {
                aCb();
            };
            iface.SetFirstBootOnProduction(aFirstBootOnProduction);
        });
    }

    GetFirstBootOnProduction(aCb: (aFirstBootOnProduction: number) => void) {
        this._call( function (iface){
            iface.GetFirstBootOnProduction['finish'] = function(aFirstBootOnProduction: number) {
                aCb(aFirstBootOnProduction);
            };
            iface.GetFirstBootOnProduction();
        });
    }

    SetAntennaPower(aAntPower: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetAntennaPower['finish'] = function() {
                aCb();
            };
            iface.SetAntennaPower(aAntPower);
        });
    }

    GetAntennaPower(aCb: (aAntPower: number) => void) {
        this._call( function (iface){
            iface.GetAntennaPower['finish'] = function(aAntPower: number) {
                aCb(aAntPower);
            };
            iface.GetAntennaPower();
        });
    }

    SetServiceUpdateFlag(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetServiceUpdateFlag['finish'] = function() {
                aCb();
            };
            iface.SetServiceUpdateFlag(aEnable);
        });
    }

    GetServiceUpdateFlag(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetServiceUpdateFlag['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetServiceUpdateFlag();
        });
    }

    GetNandBadBlocks(aCb: (aCntLdr: number, aCntUBI: number, aCntUBIFS: number) => void) {
        this._call( function (iface){
            iface.GetNandBadBlocks['finish'] = function(data) {
                aCb(data[0], data[1], data[2]);
            };
            iface.GetNandBadBlocks();
        });
    }

    GetFusingInfo(aCb: (aScs: number, aJtag: number, aOtp: number, aCwe: number) => void) {
        this._call( function (iface){
            iface.GetFusingInfo['finish'] = function(data) {
                aCb(data[0], data[1], data[2], data[3]);
            };
            iface.GetFusingInfo();
        });
    }
}

export = CSysinfoStb