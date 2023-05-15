/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CFilemgrGroup extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Filemgr.Group', '/Octopus/Appkit/Filemgr/Group');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    Create(aExtFilter: string, aCb: (aHandle: number) => void) {
        this._call( function (iface){
            iface.Create['finish'] = function(aHandle: number) {
                aCb(aHandle);
            };
            iface.Create(aExtFilter);
        });
    }

    Release(aCb: (aHandle: number) => void) {
        this._call( function (iface){
            iface.Release['finish'] = function(aHandle: number) {
                aCb(aHandle);
            };
            iface.Release();
        });
    }

    AddPath(aHandle: number, aPath: string, aCb: () => void) {
        this._call( function (iface){
            iface.AddPath['finish'] = function() {
                aCb();
            };
            iface.AddPath(aHandle, aPath);
        });
    }

    RemovePath(aHandle: number, aPath: string, aCb: () => void) {
        this._call( function (iface){
            iface.RemovePath['finish'] = function() {
                aCb();
            };
            iface.RemovePath(aHandle, aPath);
        });
    }

    ExcludePath(aHandle: number, aPath: string, aCb: () => void) {
        this._call( function (iface){
            iface.ExcludePath['finish'] = function() {
                aCb();
            };
            iface.ExcludePath(aHandle, aPath);
        });
    }

    Copy(aHandle: number, aPath: string, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.Copy['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.Copy(aHandle, aPath);
        });
    }

    Move(aHandle: number, aPath: string, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.Move['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.Move(aHandle, aPath);
        });
    }

    Remove(aHandle: number, aPath: string, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.Remove['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.Remove(aHandle, aPath);
        });
    }

    Cancel(aHandle: number, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.Cancel['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.Cancel(aHandle);
        });
    }
}

export = CFilemgrGroup