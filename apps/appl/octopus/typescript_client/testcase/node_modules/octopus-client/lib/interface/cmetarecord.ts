/**
 * Created by hsseo on 14. 5. 15.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CMetaRecord extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Meta.Record', '/Octopus/Appkit/Meta/Record');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    GetCID(aUrl: string, aCb: (aCid: number) => void) {
        this._call( function (iface){
            iface.GetCID['finish'] = function(aCid: number) {
                aCb(aCid);
            };
            iface.GetCID(aUrl);
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

    GetByIdx(aIndex: number, aCb: (aRecData: tdata.TRecListData) => void) {
        this._call( function (iface){
            iface.GetByIdx['finish'] = function(data) {
                aCb(tdata.convert_reclistdata(data));
            };
            iface.GetByIdx(aIndex);
        });
    }

    GetByCID(aCid: number, aCb: (aRecData: tdata.TRecListData) => void) {
        this._call( function (iface){
            iface.GetByCID['finish'] = function(data) {
                aCb(tdata.convert_reclistdata(data));
            };
            iface.GetByCID(aCid);
        });
    }

    GetContentsIdByUrl(aUrl: string, aCb: (aCid: number) => void) {
        this._call( function (iface){
            iface.GetContentsIdByUrl['finish'] = function(aCid: number) {
                aCb(aCid);
            };
            iface.GetContentsIdByUrl(aUrl);
        });
    }

    Release(aRecData: tdata.TRecListData, aCb: () => void) {
        this._call( function (iface){
            iface.Release['finish'] = function() {
                aCb();
            };
            iface.Release(aRecData);
        });
    }

    GetItems(aDirect: string, aCb: () => void) {
        this._call( function (iface){
            iface.GetItems['finish'] = function() {
                aCb();
            };
            iface.GetItems(aDirect);
        });
    }

    Move(aCid: number, aPath: string, aIsDecrypt: number, aCb: (aToken: number) => void) {
        this._call( function (iface){
            iface.Move['finish'] = function(aToken: number) {
                aCb(aToken);
            };
            iface.Move(aCid, aPath, aIsDecrypt);
        });
    }

    CancelFileOperation(aCid: number, aCb: () => void) {
        this._call( function (iface){
            iface.CancelFileOperation['finish'] = function() {
                aCb();
            };
            iface.CancelFileOperation(aCid);
        });
    }

    Scan(aUrl: string, aCb: () => void) {
        this._call( function (iface){
            iface.Scan['finish'] = function() {
                aCb();
            };
            iface.Scan(aUrl);
        });
    }

    Copy(aCid: number, aPath: string, bDecrypt: number, aCb: () => void) {
        this._call( function (iface){
            iface.Copy['finish'] = function() {
                aCb();
            };
            iface.Copy(aCid, aPath, bDecrypt);
        });
    }

    Cancel(aCb: () => void) {
        this._call( function (iface){
            iface.Cancel['finish'] = function() {
                aCb();
            };
            iface.Cancel();
        });
    }

    Clear(aCb: () => void) {
        this._call( function (iface){
            iface.Clear['finish'] = function() {
                aCb();
            };
            iface.Clear();
        });
    }
}

export = CMetaRecord