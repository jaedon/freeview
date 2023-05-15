/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CFilemgrSambaserver extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Filemgr.Sambaserver', '/Octopus/Appkit/Filemgr/Sambaserver');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    Start(aCb: () => void) {
        this._call( function (iface){
            iface.Start['finish'] = function() {
                aCb();
            };
            iface.Start();
        });
    }

    Stop(aCb: () => void) {
        this._call( function (iface){
            iface.Stop['finish'] = function() {
                aCb();
            };
            iface.Stop();
        });
    }

    SetConfig(aConfigInfo: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetConfig['finish'] = function() {
                aCb();
            };
            iface.SetConfig(aConfigInfo);
        });
    }

    GetServerInfo(aCb: (aConfigInfo: number) => void) {
        this._call( function (iface){
            iface.GetServerInfo['finish'] = function(aConfigInfo: number) {
                aCb(aConfigInfo);
            };
            iface.GetServerInfo();
        });
    }

    SetPassword(aUserName: string, aCurrentPassword: string, aNewPassword: string, aCb: () => void) {
        this._call( function (iface){
            iface.SetPassword['finish'] = function() {
                aCb();
            };
            iface.SetPassword(aUserName, aCurrentPassword, aNewPassword);
        });
    }

    CheckPassword(aUserName: string, aPassword: string, aCb: (aIsVaild: number) => void) {
        this._call( function (iface){
            iface.CheckPassword['finish'] = function(aIsVaild: number) {
                aCb(aIsVaild);
            };
            iface.CheckPassword(aUserName, aPassword);
        });
    }

    SetPermission(aUserName: string, aSharedInfo: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPermission['finish'] = function() {
                aCb();
            };
            iface.SetPermission(aUserName, aSharedInfo);
        });
    }

    GetSharedList(aUserName: string, aCb: (aSharedInfo: number) => void) {
        this._call( function (iface){
            iface.GetSharedList['finish'] = function(aSharedInfo: number) {
                aCb(aSharedInfo);
            };
            iface.GetSharedList(aUserName);
        });
    }

    SetPublicAccess(aUserName: string, aAccess: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPublicAccess['finish'] = function() {
                aCb();
            };
            iface.SetPublicAccess(aUserName, aAccess);
        });
    }

    GetPublicAccess(aUserName: string, aCb: (aAccess: number) => void) {
        this._call( function (iface){
            iface.GetPublicAccess['finish'] = function(aAccess: number) {
                aCb(aAccess);
            };
            iface.GetPublicAccess(aUserName);
        });
    }
}

export = CFilemgrSambaserver