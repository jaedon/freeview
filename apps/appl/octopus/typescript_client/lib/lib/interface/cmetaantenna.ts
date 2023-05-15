/**
 * Created by hsseo on 14. 5. 15.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CMetaAntenna extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Meta.Antenna', '/Octopus/Appkit/Meta/Antenna');
    }

    Add(aAntInfo: tdata.TAntennaInfo, aCb: (aUid: number) => void) {
        this._call( function (iface){
            iface.Add['finish'] = function(aUid: number) {
                aCb(aUid);
            };
            iface.Add(aAntInfo);
        });
    }

    RemoveAll(aCb: () => void) {
        this._call( function (iface){
            iface.RemoveAll['finish'] = function() {
                aCb();
            };
            iface.RemoveAll();
        });
    }

    Count(aCb: (aCount: number) => void) {
        this._call( function (iface){
            iface.Count['finish'] = function(aCount: number) {
                aCb(aCount);
            };
            iface.Count();
        });
    }

    GetUidAt(aIndex: number, aCb: (aUid: number) => void) {
        this._call( function (iface){
            iface.GetUidAt['finish'] = function(aUid: number) {
                aCb(aUid);
            };
            iface.GetUidAt(aIndex);
        });
    }

    GetInfo(aUid: number, aCb: (aAntInfo: tdata.TAntennaInfo) => void) {
        this._call( function (iface){
            iface.GetInfo['finish'] = function(data) {
                aCb(tdata.convert_antennainfo(data));
            };
            iface.GetInfo(aUid);
        });
    }

    SetInfo(aUid: number, aAntInfo: tdata.TAntennaInfo, aCb: () => void) {
        this._call( function (iface){
            iface.SetInfo['finish'] = function() {
                aCb();
            };
            iface.SetInfo(aUid, aAntInfo);
        });
    }

    Load(aCb: () => void) {
        this._call( function (iface){
            iface.Load['finish'] = function() {
                aCb();
            };
            iface.Load();
        });
    }

    Save(aCb: () => void) {
        this._call( function (iface){
            iface.Save['finish'] = function() {
                aCb();
            };
            iface.Save();
        });
    }
}

export = CMetaAntenna