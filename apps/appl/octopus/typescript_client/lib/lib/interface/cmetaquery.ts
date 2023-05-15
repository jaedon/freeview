/**
 * Created by hsseo on 14. 5. 15.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CMetaQuery extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Meta.Query', '/Octopus/Appkit/Meta/Query');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    NewQuery(aField: string, aComparision: string, aValue: string, aCb: (aQueryHandle: number) => void) {
        this._call( function (iface){
            iface.NewQuery['finish'] = function(aQueryHandle: number) {
                aCb(aQueryHandle);
            };
            iface.NewQuery(aField, aComparision, aValue);
        });
    }

    DeleteQuery(aQueryHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.DeleteQuery['finish'] = function() {
                aCb();
            };
            iface.DeleteQuery(aQueryHandle);
        });
    }

    CloneQuery(aQueryHandle: number, aCb: (aNewHandle: number) => void) {
        this._call( function (iface){
            iface.CloneQuery['finish'] = function(aNewHandle: number) {
                aCb(aNewHandle);
            };
            iface.CloneQuery(aQueryHandle);
        });
    }

    And(aQueryHandle1: number, aQueryHandle2: number, aCb: (aNewHandle: number) => void) {
        this._call( function (iface){
            iface.And['finish'] = function(aNewHandle: number) {
                aCb(aNewHandle);
            };
            iface.And(aQueryHandle1, aQueryHandle2);
        });
    }

    Or(aQueryHandle1: number, aQueryHandle2: number, aCb: (aNewHandle: number) => void) {
        this._call( function (iface){
            iface.Or['finish'] = function(aNewHandle: number) {
                aCb(aNewHandle);
            };
            iface.Or(aQueryHandle1, aQueryHandle2);
        });
    }

    Not(aQueryHandle: number, aCb: (aNewHandle: number) => void) {
        this._call( function (iface){
            iface.Not['finish'] = function(aNewHandle: number) {
                aCb(aNewHandle);
            };
            iface.Not(aQueryHandle);
        });
    }

    New(aTarget: number, aCb: (aMetaQHandle: number) => void) {
        this._call( function (iface){
            iface.New['finish'] = function(aMetaQHandle: number) {
                aCb(aMetaQHandle);
            };
            iface.New(aTarget);
        });
    }

    Delete(aMetaQHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.Delete['finish'] = function() {
                aCb();
            };
            iface.Delete(aMetaQHandle);
        });
    }

    SetQuery(aMetaQHandle: number, aQueryHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetQuery['finish'] = function() {
                aCb();
            };
            iface.SetQuery(aMetaQHandle, aQueryHandle);
        });
    }

    GetQuery(aMetaQHandle: number, aCb: (aQueryHandle: number) => void) {
        this._call( function (iface){
            iface.GetQuery['finish'] = function(aQueryHandle: number) {
                aCb(aQueryHandle);
            };
            iface.GetQuery(aMetaQHandle);
        });
    }

    AddSvcConstraint_uid(aMetaQHandle: number, aSvcUid: number, aCb: () => void) {
        this._call( function (iface){
            iface.AddSvcConstraint_uid['finish'] = function() {
                aCb();
            };
            iface.AddSvcConstraint_uid(aMetaQHandle);
        });
    }

    AddSvcConstraint_tripleid(aMetaQHandle: number, aOnid: number, aTsid: number, aSvcid: number, aCb: () => void) {
        this._call( function (iface){
            iface.AddSvcConstraint_tripleid['finish'] = function() {
                aCb();
            };
            iface.AddSvcConstraint_tripleid(aMetaQHandle, aOnid, aTsid, aSvcid);
        });
    }

    ClearSvcConstraint(aMetaQHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.ClearSvcConstraint['finish'] = function() {
                aCb();
            };
            iface.ClearSvcConstraint(aMetaQHandle);
        });
    }

    OrderBy(aMetaQHandle: number, aField: string, aAscending: number, aCb: () => void) {
        this._call( function (iface){
            iface.OrderBy['finish'] = function() {
                aCb();
            };
            iface.OrderBy(aMetaQHandle, aField, aAscending);
        });
    }

    SetFilter(aMetaQHandle: number, aFilter: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetFilter['finish'] = function() {
                aCb();
            };
            iface.SetFilter(aMetaQHandle, aFilter);
        });
    }

    Search(aMetaQHandle: number, aDirect: number, aCb: () => void) {
        this._call( function (iface){
            iface.Search['finish'] = function() {
                aCb();
            };
            iface.Search(aMetaQHandle, aDirect);
        });
    }

    Abort(aMetaQHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.Abort['finish'] = function() {
                aCb();
            };
            iface.Abort(aMetaQHandle);
        });
    }

    Result(aMetaQHandle: number, aCb: (aResultList : tdata.TEvent[]) => void) {
        this._call( function (iface){
            iface.Result['finish'] = function(data) {
                var eventList = [];
                data.forEach((e) => {
                    eventList.push(tdata.convert_event(e));
                });
                aCb(eventList);
            };
            iface.Result(aMetaQHandle);
        });
    }

    GetEvent_uid(aType: number, aSvcUid: number, aCb: (aEventInfo: tdata.TEvent) => void) {
        this._call( function (iface){
            iface.GetEvent_uid['finish'] = function(data) {
                aCb(tdata.convert_event(data));
            };
            iface.GetEvent_uid(aType, aSvcUid);
        });
    }

    GetEvent_tripleid(aType: number, aOnid: number, aTsid: number, aSvcid: number, aCb: (aEventInfo: tdata.TEvent) => void) {
        this._call( function (iface){
            iface.GetEvent_tripleid['finish'] = function(data) {
                aCb(tdata.convert_event(data));
            };
            iface.GetEvent_tripleid(aType, aOnid, aTsid, aSvcid);
        });
    }

    GetEventWithIds_uid(aSvcUid: number, aEventid: number, aCb: (aEventInfo: tdata.TEvent) => void) {
        this._call( function (iface){
            iface.GetEventWithIds_uid['finish'] = function(data) {
                aCb(tdata.convert_event(data));
            };
            iface.GetEventWithIds_uid(aSvcUid, aEventid);
        });
    }

    GetEventWithIds_tripleid(aOnid: number, aTsid: number, aSvcid: number, aEventid: number, aCb: (aEventInfo: tdata.TEvent) => void) {
        this._call( function (iface){
            iface.GetEventWithIds_tripleid['finish'] = function(data) {
                aCb(tdata.convert_event(data));
            };
            iface.GetEventWithIds_tripleid(aOnid, aTsid, aSvcid, aEventid);
        });
    }

    GetIpEvent_uid(aSvcUid: number, aEventid: number, aCb: (aEventInfo: tdata.TEvent) => void) {
        this._call( function (iface){
            iface.GetIpEvent_uid['finish'] = function(data) {
                aCb(tdata.convert_event(data));
            };
            iface.GetIpEvent_uid(aSvcUid, aEventid);
        });
    }

    GetIpEvent_tripleid(aOnid: number, aTsid: number, aSvcid: number, aEventid: number, aCb: (aEventInfo: tdata.TEvent) => void) {
        this._call( function (iface){
            iface.GetIpEvent_tripleid['finish'] = function(data) {
                aCb(tdata.convert_event(data));
            };
            iface.GetIpEvent_tripleid(aOnid, aTsid, aSvcid, aEventid);
        });
    }

    GetPF_uid(aSvcUid: number, aCb: (aEventInfo: tdata.TEvent[]) => void) {
        this._call( function (iface){
            iface.GetPF_uid['finish'] = function(data) {
                var eventList = [];
                data.forEach((e) => {
                    eventList.push(tdata.convert_event(e));
                });
                aCb(eventList);
            };
            iface.GetPF_uid(aSvcUid);
        });
    }

    GetPF_tripleid(aOnid: number, aTsid: number, aSvcid: number, aCb: (aEventInfo: tdata.TEvent[]) => void) {
        this._call( function (iface){
            iface.GetPF_tripleid['finish'] = function(data) {
                var eventList = [];
                data.forEach((e) => {
                    eventList.push(tdata.convert_event(e));
                });
                aCb(eventList);
            };
            iface.GetPF_tripleid(aOnid, aTsid, aSvcid);
        });
    }

    SetIpEvent(aUid: number, aData: ArrayBuffer, aCb: () => void) {
        this._call( function (iface){
            iface.SetIpEvent['finish'] = function() {
                aCb();
            };
            iface.SetIpEvent(aUid, aData);
        });
    }

    ResetIpEvent(aUid: number, aCb: () => void) {
        this._call( function (iface){
            iface.ResetIpEvent['finish'] = function() {
                aCb();
            };
            iface.ResetIpEvent(aUid);
        });
    }

    ResetIpEventTable(aCb: () => void) {
        this._call( function (iface){
            iface.ResetIpEventTable['finish'] = function() {
                aCb();
            };
            iface.ResetIpEventTable();
        });
    }

    DeletIpEventByEndTime(aEndtime: number, aCb: () => void) {
        this._call( function (iface){
            iface.DeletIpEventByEndTime['finish'] = function() {
                aCb();
            };
            iface.DeletIpEventByEndTime(aEndtime);
        });
    }

    SetIpSvc(aUid: number, aData: ArrayBuffer, aCb: () => void) {
        this._call( function (iface){
            iface.SetIpSvc['finish'] = function() {
                aCb();
            };
            iface.SetIpSvc(aUid, aData);
        });
    }

    ResetIpSvc(aUid: number, aCb: () => void) {
        this._call( function (iface){
            iface.ResetIpSvc['finish'] = function() {
                aCb();
            };
            iface.ResetIpSvc(aUid);
        });
    }

    ResetIpSvcTable(aCb: () => void) {
        this._call( function (iface){
            iface.ResetIpSvcTable['finish'] = function() {
                aCb();
            };
            iface.ResetIpSvcTable();
        });
    }

    SetIpLogo(aUid: number, aData: ArrayBuffer, aCb: () => void) {
        this._call( function (iface){
            iface.SetIpLogo['finish'] = function() {
                aCb();
            };
            iface.SetIpLogo(aUid, aData);
        });
    }


    ResetIpLogo(aUid: number, aCb: () => void) {
        this._call( function (iface){
            iface.ResetIpLogo['finish'] = function() {
                aCb();
            };
            iface.ResetIpLogo(aUid);
        });
    }

    ResetIpLogoTable(aCb: () => void) {
        this._call( function (iface){
            iface.ResetIpLogoTable['finish'] = function() {
                aCb();
            };
            iface.ResetIpLogoTable();
        });
    }

    SyncIpEventTable(aCb: () => void) {
        this._call( function (iface){
            iface.SyncIpEventTable['finish'] = function() {
                aCb();
            };
            iface.SyncIpEventTable();
        });
    }

    SyncIpSvcTable(aCb: () => void) {
        this._call( function (iface){
            iface.SyncIpSvcTable['finish'] = function() {
                aCb();
            };
            iface.SyncIpSvcTable();
        });
    }

    SyncIpLogoTable(aCb: () => void) {
        this._call( function (iface){
            iface.SyncIpLogoTable['finish'] = function() {
                aCb();
            };
            iface.SyncIpLogoTable();
        });
    }

    ResetTVTVEventTable(aCb: () => void) {
        this._call( function (iface){
            iface.ResetTVTVEventTable['finish'] = function() {
                aCb();
            };
            iface.ResetTVTVEventTable();
        });
    }

    ResetTVTVSvcTable(aCb: () => void) {
        this._call( function (iface){
            iface.ResetTVTVSvcTable['finish'] = function() {
                aCb();
            };
            iface.ResetTVTVSvcTable();
        });
    }

    SyncTVTVSvcTable(aCb: () => void) {
        this._call( function (iface){
            iface.SyncTVTVSvcTable['finish'] = function() {
                aCb();
            };
            iface.SyncTVTVSvcTable();
        });
    }
}

export = CMetaQuery