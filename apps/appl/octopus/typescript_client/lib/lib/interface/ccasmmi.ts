/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CCasMmi extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Cas.Mmi', '/Octopus/Appkit/Cas/Mmi');
    }

    StartEvent(aCb: (param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(param1: number, param2: number, param3: number) {
                aCb(param1, param2, param3);
            });
        });
    }

    GetEvtInfo(aCb: (aEventInfo: tdata.TCASMMIEvent) => void) {
        this._call( function (iface){
            iface.GetEvtInfo['finish'] = function(data) {
                aCb(tdata.convert_casmmievent(data));
            };
            iface.GetEvtInfo();
        });
    }

    GetUiEvent(aSessionType: number, aHandleSession: number, aCb: (aEventInfo: tdata.TCASMMIEvent) => void) {
        this._call( function (iface){
            iface.GetUiEvent['finish'] = function(data) {
                aCb(tdata.convert_casmmievent(data));
            };
            iface.GetUiEvent(aSessionType, aHandleSession);
        });
    }

    SendUiResponse(aGroupId: number, aSlot: number, aSessionType: number, aHandleSession: number, aInputData: number[], aCb: () => void) {
        this._call( function (iface){
            iface.SendUiResponse['finish'] = function() {
                aCb();
            };
            iface.SendUiResponse(aGroupId, aSlot, aSessionType, aHandleSession, aInputData);
        });
    }

    CloseSession(aGroupId: number, aSlot: number, aSessionType: number, aHandleSession: number, aCb: () => void) {
        this._call( function (iface){
            iface.CloseSession['finish'] = function() {
                aCb();
            };
            iface.CloseSession(aGroupId, aSlot, aSessionType, aHandleSession);
        });
    }

    OpenSession(aGroupId: number, aSlot: number, aSessionType: number, aParamData: number[], aCb: (aHandleSession: number) => void) {
        this._call( function (iface){
            iface.OpenSession['finish'] = function(aHandleSession: number) {
                aCb(aHandleSession);
            };
            iface.OpenSession(aGroupId, aSlot, aSessionType, aParamData);
        });
    }
}

export = CCasMmi