/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CUtilDb extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Util.Db', '/Octopus/Appkit/Util/Db');
    }

    Remove(aKey: string, aCb:() => void) {
        this._call(function (iface) {
            iface.Remove['finish'] = function () {
                aCb();
            };
            iface.Remove(aKey);
        });
    }

    SetInt(aKey: string, aValue: number, aCb:() => void) {
        this._call(function (iface) {
            iface.SetInt['finish'] = function () {
                aCb();
            };
            iface.SetInt(aKey, aValue);
        });
    }

    GetInt(aKey: string, aCb:(aValue: number) => void) {
        this._call(function (iface) {
            iface.GetInt['finish'] = function (aValue: number) {
                aCb(aValue);
            };
            iface.GetInt(aKey);
        });
    }

    SetStr(aKey: string, aValue: string, aCb:() => void) {
        this._call(function (iface) {
            iface.SetStr['finish'] = function () {
                aCb();
            };
            iface.SetStr(aKey, aValue);
        });
    }

    GetStr(aKey: string, aCb:(aValue: string) => void) {
        this._call(function (iface) {
            iface.GetStr['finish'] = function (aValue: string) {
                aCb(aValue);
            };
            iface.GetStr(aKey);
        });
    }
}

export = CUtilDb