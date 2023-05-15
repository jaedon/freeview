/**
 * Created by hsseo on 14. 5. 12.
 */



import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CFilemgrBase extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Filemgr.Base', '/Octopus/Appkit/Filemgr/Base');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    BrowserFiles(aPath: string, aFilter: string, aKeyword: string, aBrowsingMode: number, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.BrowserFiles['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.BrowserFiles(aPath, aFilter, aKeyword, aBrowsingMode);
        });
    }

    ScanDir(aSource: string, aExtFilter: string, aRecursive: number, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.ScanDir['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.ScanDir(aSource, aExtFilter, aRecursive);
        });
    }

    Remove(aSource: string, aExtFilter: string, aRecursive: number, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.Remove['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.Remove(aSource, aExtFilter, aRecursive);
        });
    }

    Copy(aSource: string, aDest: string, aExtFilter: string, aDecryption: number, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.Copy['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.Copy(aSource, aDest, aExtFilter, aDecryption);
        });
    }

    Move(aSource: string, aDest: string, aExtFilter: string, aDecryption: number, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.Move['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.Move(aSource, aDest, aExtFilter, aDecryption);
        });
    }

    MakeDir(aSource: string, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.MakeDir['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.MakeDir(aSource);
        });
    }

    RemoveDir(aSource: string, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.RemoveDir['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.RemoveDir(aSource);
        });
    }

    Cancel(aCancleRequestId: number, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.Cancel['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.Cancel(aCancleRequestId);
        });
    }

    GetSizeOfPath(aPath: string, aExtFilter: string, aCb: (aSize: number) => void) {
        this._call( function (iface){
            iface.GetSizeOfPath['finish'] = function(aSize: number) {
                aCb(aSize);
            };
            iface.GetSizeOfPath(aPath, aExtFilter);
        });
    }
}

export = CFilemgrBase