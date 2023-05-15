/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CMetaService extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Meta.Service', '/Octopus/Appkit/Meta/Service');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }
    
    GetService(aUid: number, aCb: (service: tdata.TService) => void) {
        this._call( function (iface){
            iface.GetService['finish'] = function(data) {
                aCb(tdata.convert_service(data));
            };
            iface.GetService(aUid);
        });
    }

    GetNetwork(aUid: number, aCb: (networkInfo: tdata.TNetworkInfo) => void) {
        this._call( function (iface){
            iface.GetNetwork['finish'] = function(data) {
                aCb(tdata.convert_network(data));
            };
            iface.GetNetwork(aUid);
        });
    }
    GetTransponder(aUid: number, aCb: (transponderInfo: tdata.TTransponderInfo) => void) {
        this._call( function (iface){
            iface.GetTransponder['finish'] = function(data) {
                aCb(tdata.convert_transponder(data));
            };
            iface.GetTransponder(aUid);
        });
    }
    GetProvider(aUid: number, aCb: (providerInfo: tdata.TProviderInfo) => void) {
        this._call( function (iface){
            iface.GetProvider['finish'] = function(data) {
                aCb(tdata.convert_provider(data));
            };
            iface.GetProvider(aUid);
        });
    }
    GetGroup(aUid: number, aCb: (groupInfo: tdata.TGroupInfo) => void) {
        this._call( function (iface){
            iface.GetGroup['finish'] = function(data) {
                aCb(tdata.convert_group(data));
            };
            iface.GetGroup(aUid);
        });
    }
    GetBouquet(aUid: number, aCb: (bouquetInfo: tdata.TBouquetInfo) => void) {
        this._call( function (iface){
            iface.GetBouquet['finish'] = function(data) {
                aCb(tdata.convert_bouquet(data));
            };
            iface.GetBouquet(aUid);
        });
    }

    GetLogoUrl(aUid: number, aBufChannelLogoInfo: any, aCb: (channelLogoInfo: tdata.TChannelLogoInfo) => void) {
        this._call( function (iface){
            iface.GetLogoUrl['finish'] = function(data) {
                //aCb(tdata.convert_logoUrl(data));
            };
            iface.GetLogoUrl(aUid);
        });
    }

    GetServiceTriplet(aUid: number, aCb: (aTsid: number, aOnid: number, aSid: number) => void) {
        this._call( function (iface){
            iface.GetServiceTriplet['finish'] = function(data) {
                aCb(data[0], data[1], data[2]);
            };
            iface.GetServiceTriplet(aUid);
        });
    }

    FindServiceByTriplet(aOnId: number, aTsId: number, aSvcid: number, aCb:(service: tdata.TService) => void) {
        this._call( function (iface){
            iface.FindServiceByTriplet['finish'] = function(data) {
                aCb(tdata.convert_service(data));
            };
            iface.FindServiceByTriplet(aOnId, aTsId, aSvcid);
        });
    }

    FindServiceByNumber(aNumber: number, aCb: (service: tdata.TService) => void) {
        this._call( function (iface){
            iface.FindServiceByNumber['finish'] = function(data) {
                aCb(tdata.convert_service(data));
            };
            iface.FindServiceByNumber(aNumber);
        });
    }

    GetServiceList(aCb: (serviceList: tdata.TService[]) => void) {
        this._call( function (iface){
            iface.GetServiceList['finish'] = function(data) {
                console.log(data);
                var serviceList = [];
                data.forEach((s) => {
                    serviceList.push(tdata.convert_service(s));
                });
                aCb(serviceList);
            };
            iface.GetServiceList();
        });
    }


    GetGroupList(aCb: (groupList: tdata.TGroupInfo[]) => void) {
        this._call( function (iface){
            iface.GetGroupList['finish'] = function(data) {
                console.log(data);
                var groupList = [];
                data.forEach((g) => {
                    groupList.push(tdata.convert_group(g));
                });
                aCb(groupList);
            };
            iface.GetGroupList();
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

    SetService(aService: tdata.TService, aCb: () => void) {
        this._call( function (iface){
            iface.SetService['finish'] = function() {
                aCb();
            };
            iface.SetService(aService);
        });
    }

    AddService(aService: tdata.TService, aCb: () => void) {
        this._call( function (iface){
            iface.AddService['finish'] = function() {
                aCb();
            };
            iface.AddService(aService);
        });
    }

    RemoveService(aService: tdata.TService, aCb: () => void) {
        this._call( function (iface){
            iface.RemoveService['finish'] = function() {
                aCb();
            };
            iface.RemoveService(aService);
        });
    }

    Reset(aCb: () => void) {
        this._call( function (iface){
            iface.Reset['finish'] = function() {
                aCb();
            };
            iface.Reset();
        });
    }

    LoadPreferredList(aCb: () => void) {
        this._call( function (iface){
            iface.LoadPreferredList['finish'] = function() {
                aCb();
            };
            iface.LoadPreferredList();
        });
    }

    LoadupdatedList(aCb: () => void) {
        this._call( function (iface){
            iface.LoadupdatedList['finish'] = function() {
                aCb();
            };
            iface.LoadupdatedList();
        });
    }

    ChangeUpdateFlag(aFromFlag: number, aToFlag: number, aCb: () => void) {
        this._call( function (iface){
            iface.ChangeUpdateFlag['finish'] = function() {
                aCb();
            };
            iface.ChangeUpdateFlag(aFromFlag, aToFlag);
        });
    }

    RemoveServiceWithFlag(aFlag: number, aCb: () => void) {
        this._call( function (iface){
            iface.RemoveServiceWithFlag['finish'] = function() {
                aCb();
            };
            iface.RemoveServiceWithFlag(aFlag);
        });
    }
}

export = CMetaService