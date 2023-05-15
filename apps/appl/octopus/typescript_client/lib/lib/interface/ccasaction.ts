/**
 * Created by hsseo on 14. 5. 12.
 */




import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CCasAction extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Cas.Action', '/Octopus/Appkit/Cas/Action');
    }

    StartEvent(aCb: (param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(param1: number, param2: number, param3: number) {
                aCb(param1, param2, param3);
            });
        });
    }

    GetSlotId(aCb: (aSlotId: number) => void) {
        this._call( function (iface){
            iface.GetSlotId['finish'] = function(aSlotId: number) {
                aCb(aSlotId);
            };
            iface.GetSlotId();
        });
    }

    GetCasId(aCb: (aCasId: number) => void) {
        this._call( function (iface){
            iface.GetCasId['finish'] = function(aCasId: number) {
                aCb(aCasId);
            };
            iface.GetCasId();
        });
    }

    GetData(aCb: (aData: number[]) => void) {
        this._call( function (iface){
            iface.GetData['finish'] = function(data) {
                aCb(data);
            };
            iface.GetData();
        });
    }
}

export = CCasAction