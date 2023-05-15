/// <reference path="../../../def/node.d.ts" />
import tdata = require('./tdata');
import dbusConn = require('../util/dbus_connect');
declare class CMetaService extends dbusConn.CDBusInterface {
    constructor();
    public StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void): void;
    public GetService(aUid: number, aCb: (service: tdata.TService) => void): void;
    public GetNetwork(aUid: number, aCb: (networkInfo: tdata.TNetworkInfo) => void): void;
    public GetTransponder(aUid: number, aCb: (transponderInfo: tdata.TTransponderInfo) => void): void;
    public GetProvider(aUid: number, aCb: (providerInfo: tdata.TProviderInfo) => void): void;
    public GetGroup(aUid: number, aCb: (groupInfo: tdata.TGroupInfo) => void): void;
    public GetBouquet(aUid: number, aCb: (bouquetInfo: tdata.TBouquetInfo) => void): void;
    public GetLogoUrl(aUid: number, aBufChannelLogoInfo: any, aCb: (channelLogoInfo: tdata.TChannelLogoInfo) => void): void;
    public GetServiceTriplet(aUid: number, aCb: (aTsid: number, aOnid: number, aSid: number) => void): void;
    public FindServiceByTriplet(aOnId: number, aTsId: number, aSvcid: number, aCb: (service: tdata.TService) => void): void;
    public FindServiceByNumber(aNumber: number, aCb: (service: tdata.TService) => void): void;
    public GetServiceList(aCb: (serviceList: tdata.TService[]) => void): void;
    public GetGroupList(aCb: (groupList: tdata.TGroupInfo[]) => void): void;
    public Load(aCb: () => void): void;
    public Save(aCb: () => void): void;
    public SetService(aService: tdata.TService, aCb: () => void): void;
    public AddService(aService: tdata.TService, aCb: () => void): void;
    public RemoveService(aService: tdata.TService, aCb: () => void): void;
    public Reset(aCb: () => void): void;
    public LoadPreferredList(aCb: () => void): void;
    public LoadupdatedList(aCb: () => void): void;
    public ChangeUpdateFlag(aFromFlag: number, aToFlag: number, aCb: () => void): void;
    public RemoveServiceWithFlag(aFlag: number, aCb: () => void): void;
}
export = CMetaService;
