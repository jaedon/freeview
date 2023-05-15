/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CFilemgrFtpserver extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Filemgr.Ftpserver', '/Octopus/Appkit/Filemgr/Ftpserver');
    }

    StartEvent(aCb: (handleId: number, result: number) => void) {
        this._call( function (iface){
            iface.on('event', function(handleId: number, result: number) {
                aCb(handleId, result);
            });
        });
    }

    Start(aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.Start['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.Start();
        });
    }

    Stop(aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.Stop['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.Stop();
        });
    }

    SetConfig(aServerName: string, aPort: number, aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.SetConfig['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.SetConfig((aServerName, aPort));
        });
    }

    GetServerInfo(aCb: (aFtpInfo: tdata.TFTPServerInfo) => void) {
        this._call( function (iface){
            iface.GetServerInfo['finish'] = function(data) {
                aCb(tdata.convert_ftpserverinfo(data));
            };
            iface.GetServerInfo();
        });
    }

    SetPassword(aUserName: string, aCurrentPassword: string, aNewPassword: string, aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.SetPassword['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.SetPassword((aUserName, aCurrentPassword, aNewPassword));
        });
    }

    CheckPassword(aUserName: string, aPassword: string, aCb: (aIsVaild: number) => void) {
        this._call( function (iface){
            iface.CheckPassword['finish'] = function(aIsVaild: number) {
                aCb(aIsVaild);
            };
            iface.CheckPassword((aUserName, aPassword));
        });
    }

    SetPermission(aUserName: string, aName: string, aPath: string, aState: number, aWritable: number, aMounted: number, aCb: (aIsVaild: number) => void) {
        this._call( function (iface){
            iface.SetPermission['finish'] = function(aIsVaild: number) {
                aCb(aIsVaild);
            };
            iface.SetPermission(aUserName, (aName,aPath,aState,aWritable,aMounted));
        });
    }

    GetSharedList(aUserName: string, aCb: (aSharedInfoList: tdata.TFTPShared []) => void) {
        this._call( function (iface){
            iface.GetSharedList['finish'] = function(data) {
                var sharedInfoList = [];
                data.forEach((s) => {
                    sharedInfoList.push(tdata.convert_ftpshared(s));
                });
                aCb(sharedInfoList);
            };
            iface.GetSharedList(aUserName);
        });
    }

    SetPublicAccess(aUserName: string, aAccess: number, aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.SetPublicAccess['finish'] = function(aReval: number) {
                aCb(aReval);
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

export = CFilemgrFtpserver