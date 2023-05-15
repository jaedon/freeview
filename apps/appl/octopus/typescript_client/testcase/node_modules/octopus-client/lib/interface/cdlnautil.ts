/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CDlnaUtil extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Dlna.Util', '/Octopus/Appkit/Dlna/Util');
    }

    StartDMPEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number) => void) {
        this._call( function (iface){
            iface.on('dmp_event', function(msgInt: number, msgStr: string, param1: number, param2: number) {
                aCb(msgInt, msgStr, param1, param2);
            });
        });
    }

    StartDMREvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number) => void) {
        this._call( function (iface){
            iface.on('dmr_event', function(msgInt: number, msgStr: string, param1: number, param2: number) {
                aCb(msgInt, msgStr, param1, param2);
            });
        });
    }

    StartDMSEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number) => void) {
        this._call( function (iface){
            iface.on('dms_event', function(msgInt: number, msgStr: string, param1: number, param2: number) {
                aCb(msgInt, msgStr, param1, param2);
            });
        });
    }

    StartDIALEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number) => void) {
        this._call( function (iface){
            iface.on('dial_event', function(msgInt: number, msgStr: string, param1: number, param2: number) {
                aCb(msgInt, msgStr, param1, param2);
            });
        });
    }

    StartWSEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number) => void) {
        this._call( function (iface){
            iface.on('ws_event', function(msgInt: number, msgStr: string, param1: number, param2: number) {
                aCb(msgInt, msgStr, param1, param2);
            });
        });
    }

    StartSATIPEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number) => void) {
        this._call( function (iface){
            iface.on('satip_event', function(msgInt: number, msgStr: string, param1: number, param2: number) {
                aCb(msgInt, msgStr, param1, param2);
            });
        });
    }

    StartDLNARUISEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number) => void) {
        this._call( function (iface){
            iface.on('dlnaruis_event', function(msgInt: number, msgStr: string, param1: number, param2: number) {
                aCb(msgInt, msgStr, param1, param2);
            });
        });
    }

    StartOBAMAEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number) => void) {
        this._call( function (iface){
            iface.on('obama_event', function(msgInt: number, msgStr: string, param1: number, param2: number) {
                aCb(msgInt, msgStr, param1, param2);
            });
        });
    }

    DMP_Start(aCb: () => void) {
        this._call( function (iface){
            iface.DMP_Start['finish'] = function() {
                aCb();
            };
            iface.DMP_Start();
        });
    }

    DMP_Stop(aCb: () => void) {
        this._call( function (iface){
            iface.DMP_Stop['finish'] = function() {
                aCb();
            };
            iface.DMP_Stop();
        });
    }

    DMP_GetListOfDMS(aCb: (aServerDeviceList: tdata.TDMSInfo[]) => void) {
        this._call( function (iface){
            iface.DMP_GetListOfDMS['finish'] = function(data) {
                var serverDeviceList = [];
                data.forEach((d) => {
                    serverDeviceList.push(tdata.convert_dmsinfo(d));
                });
                aCb(serverDeviceList);
            };
            iface.DMP_GetListOfDMS();
        });
    }

    DMP_BrowsingChildItem(aUdn: string, aCid: string, aSort: string, aStartIndex: number, aRequest: number, aCb: () => void) {
        this._call( function (iface){
            iface.DMP_BrowsingChildItem['finish'] = function() {
                aCb();
            };
            iface.DMP_BrowsingChildItem(aUdn, aCid, aSort, aStartIndex, aRequest);
        });
    }

    DMP_SearchingItem(aUdn: string, aCid: string, aCategory: string, aKeyword: string, aSort: string, aStartIndex: number, aRequest: number, aRequestId: number, aCb: () => void) {
        this._call( function (iface){
            iface.DMP_SearchingItem['finish'] = function() {
                aCb();
            };
            iface.DMP_SearchingItem(aUdn, aCid, aCategory, aKeyword, aSort, aStartIndex, aRequest, aRequestId);
        });
    }

    DMP_SearchingAll(aCid: string, aCategory: string, aKeyword: string, aSort: string, aRequest: number, aCb: () => void) {
        this._call( function (iface){
            iface.DMP_SearchingAll['finish'] = function() {
                aCb();
            };
            iface.DMP_SearchingAll(aCid, aCategory, aKeyword, aSort, aRequest);
        });
    }

    DMP_GetRecordDestinations(aUdn: string, aCb: () => void) {
        this._call( function (iface){
            iface.DMP_GetRecordDestinations['finish'] = function() {
                aCb();
            };
            iface.DMP_GetRecordDestinations(aUdn);
        });
    }

    DMP_GetRecordDiskInfo(aUdn: string, aID: string, aCb: () => void) {
        this._call( function (iface){
            iface.DMP_GetRecordDiskInfo['finish'] = function() {
                aCb();
            };
            iface.DMP_GetRecordDiskInfo(aUdn, aID);
        });
    }

    DMP_RemoveItem(aUdn: string, aID: string, aCb: (aRetval: string) => void) {
        this._call( function (iface){
            iface.DMP_RemoveItem['finish'] = function(aRetval: string) {
                aCb(aRetval);
            };
            iface.DMP_RemoveItem(aUdn, aID);
        });
    }

    DMP_GetIsIPDubbing(aCb: (aRetval: number) => void) {
        this._call( function (iface){
            iface.DMP_GetIsIPDubbing['finish'] = function(aRetval: number) {
                aCb(aRetval);
            };
            iface.DMP_GetIsIPDubbing();
        });
    }

    DMP_RefreshDms(aUdn: string, aCb: () => void) {
        this._call( function (iface){
            iface.DMP_RefreshDms['finish'] = function() {
                aCb();
            };
            iface.DMP_RefreshDms();
        });
    }

    DMR_Start(aFriendlyName: string, aCb: () => void) {
        this._call( function (iface){
            iface.DMR_Start['finish'] = function() {
                aCb();
            };
            iface.DMR_Start(aFriendlyName);
        });
    }

    DMR_Stop(aCb: () => void) {
        this._call( function (iface){
            iface.DMR_Stop['finish'] = function() {
                aCb();
            };
            iface.DMR_Stop();
        });
    }

    DMR_GetDefaultFriendlyName(aCb: (aFriendlyName: string) => void) {
        this._call( function (iface){
            iface.DMR_GetDefaultFriendlyName['finish'] = function(aFriendlyName: string) {
                aCb(aFriendlyName);
            };
            iface.DMR_GetDefaultFriendlyName();
        });
    }

    DMR_SetFriendlyName(aFriendlyName: string, aCb: () => void) {
        this._call( function (iface){
            iface.DMR_SetFriendlyName['finish'] = function() {
                aCb();
            };
            iface.DMR_SetFriendlyName(aFriendlyName);
        });
    }

    DMR_SetBringInUri(aUrl: string, aCb: () => void) {
        this._call( function (iface){
            iface.DMR_SetBringInUri['finish'] = function() {
                aCb();
            };
            iface.DMR_SetBringInUri(aUrl);
        });
    }

    DMR_SetOperationMode(aMode: number, aCb: () => void) {
        this._call( function (iface){
            iface.DMR_SetOperationMode['finish'] = function() {
                aCb();
            };
            iface.DMR_SetOperationMode(aMode);
        });
    }

    DMS_Start(aFriendlyName: string, aBootTimeStart: number, aCb: () => void) {
        this._call( function (iface){
            iface.DMS_Start['finish'] = function() {
                aCb();
            };
            iface.DMS_Start(aFriendlyName, aBootTimeStart);
        });
    }

    DMS_Stop(aCb: () => void) {
        this._call( function (iface){
            iface.DMS_Stop['finish'] = function() {
                aCb();
            };
            iface.DMS_Stop();
        });
    }

    DMS_GetDefaultFriendlyName(aCb: (aFriendlyName: string) => void) {
        this._call( function (iface){
            iface.DMS_GetDefaultFriendlyName['finish'] = function(aFriendlyName: string) {
                aCb(aFriendlyName);
            };
            iface.DMS_GetDefaultFriendlyName();
        });
    }

    DMS_SetFriendlyName(aFriendlyName: string, aCb: () => void) {
        this._call( function (iface){
            iface.DMS_SetFriendlyName['finish'] = function() {
                aCb();
            };
            iface.DMS_SetFriendlyName(aFriendlyName);
        });
    }

    DIAL_Start(aFriendlyName: string, aCb: () => void) {
        this._call( function (iface){
            iface.DIAL_Start['finish'] = function() {
                aCb();
            };
            iface.DIAL_Start(aFriendlyName);
        });
    }

    DIAL_Stop(aCb: () => void) {
        this._call( function (iface){
            iface.DIAL_Stop['finish'] = function() {
                aCb();
            };
            iface.DIAL_Stop();
        });
    }

    DIAL_SetFriendlyName(aFriendlyName: string, aCb: () => void) {
        this._call( function (iface){
            iface.DIAL_SetFriendlyName['finish'] = function() {
                aCb();
            };
            iface.DIAL_SetFriendlyName(aFriendlyName);
        });
    }

    DIAL_AppRegist(aApp: string, aCb: () => void) {
        this._call( function (iface){
            iface.DIAL_AppRegist['finish'] = function() {
                aCb();
            };
            iface.DIAL_AppRegist(aApp);
        });
    }

    DIAL_AppUnregist(aApp: string, aCb: () => void) {
        this._call( function (iface){
            iface.DIAL_AppUnregist['finish'] = function() {
                aCb();
            };
            iface.DIAL_AppUnregist(aApp);
        });
    }

    DIAL_SetAppState(aApp: string, aState: number, aCb: () => void) {
        this._call( function (iface){
            iface.DIAL_SetAppState['finish'] = function() {
                aCb();
            };
            iface.DIAL_SetAppState(aApp, aState);
        });
    }

    WOONSERVER_Start(aFriendlyName: string, aCb: () => void) {
        this._call( function (iface){
            iface.WOONSERVER_Start['finish'] = function() {
                aCb();
            };
            iface.WOONSERVER_Start(aFriendlyName);
        });
    }

    WOONSERVER_Stop(aCb: () => void) {
        this._call( function (iface){
            iface.WOONSERVER_Stop['finish'] = function() {
                aCb();
            };
            iface.WOONSERVER_Stop();
        });
    }

    WOONSERVER_GetDefaultFriendlyName(aCb: (aFriendlyName: string) => void) {
        this._call( function (iface){
            iface.WOONSERVER_GetDefaultFriendlyName['finish'] = function(aFriendlyName: string) {
                aCb(aFriendlyName);
            };
            iface.WOONSERVER_GetDefaultFriendlyName();
        });
    }

    WOONSERVER_SetFriendlyName(aFriendlyName: string, aCb: () => void) {
        this._call( function (iface){
            iface.WOONSERVER_SetFriendlyName['finish'] = function() {
                aCb();
            };
            iface.WOONSERVER_SetFriendlyName(aFriendlyName);
        });
    }

    WOONSERVER_TestConnection(aCb: () => void) {
        this._call( function (iface){
            iface.WOONSERVER_TestConnection['finish'] = function() {
                aCb();
            };
            iface.WOONSERVER_TestConnection();
        });
    }

    SATIP_Start(aFriendlyName: string, aCb: () => void) {
        this._call( function (iface){
            iface.SATIP_Start['finish'] = function() {
                aCb();
            };
            iface.SATIP_Start(aFriendlyName);
        });
    }

    SATIP_Stop(aCb: () => void) {
        this._call( function (iface){
            iface.SATIP_Stop['finish'] = function() {
                aCb();
            };
            iface.SATIP_Stop();
        });
    }

    SATIP_GetDefaultFriendlyName(aCb: (aFriendlyName: string) => void) {
        this._call( function (iface){
            iface.SATIP_GetDefaultFriendlyName['finish'] = function(aFriendlyName: string) {
                aCb(aFriendlyName);
            };
            iface.SATIP_GetDefaultFriendlyName();
        });
    }

    SATIP_SetFriendlyName(aFriendlyName: string, aCb: () => void) {
        this._call( function (iface){
            iface.SATIP_SetFriendlyName['finish'] = function() {
                aCb();
            };
            iface.SATIP_SetFriendlyName(aFriendlyName);
        });
    }

    DLNARUIS_SendMessage(aDeviceHandle: number, aReqHandle: number, aHeader: number[], aMessage: number[], aCb: () => void) {
        this._call( function (iface){
            iface.DLNARUIS_SendMessage['finish'] = function() {
                aCb();
            };
            iface.DLNARUIS_SendMessage(aDeviceHandle, aReqHandle, aHeader, aMessage);
        });
    }

    DLNARUIS_SendMulticast(aDeviceHandle: number, aEventLevel: number, aNotifCEHTML: number[], aFriendlyName: string, aProfileList: number[], aCb: () => void) {
        this._call( function (iface){
            iface.DLNARUIS_SendMulticast['finish'] = function() {
                aCb();
            };
            iface.DLNARUIS_SendMulticast(aDeviceHandle, aEventLevel, aNotifCEHTML, aFriendlyName, aProfileList);
        });
    }

    DLNARUIS_SendErrorCode(aDeviceHandle: number, aReqHandle: number, aError: number, aCb: () => void) {
        this._call( function (iface){
            iface.DLNARUIS_SendErrorCode['finish'] = function() {
                aCb();
            };
            iface.DLNARUIS_SendErrorCode(aDeviceHandle, aReqHandle, aError);
        });
    }

    LiveStream_Start(aTranscode: number, aSvcHandle: number, aProfileID: number, aIsPcrTsPacketInsert: number, aSource: number, aOutput: number, aCb: () => void) {
        this._call( function (iface){
            iface.LiveStream_Start['finish'] = function() {
                aCb();
            };
            iface.LiveStream_Start((aTranscode, aSvcHandle, aProfileID, aIsPcrTsPacketInsert, aSource, aOutput));
        });
    }

    LiveStream_Stop(aHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.LiveStream_Stop['finish'] = function() {
                aCb();
            };
            iface.LiveStream_Stop(aHandle);
        });
    }

    LiveStream_ItemindexByProfile(aProfileName: string, aCb: (aIndex: number) => void) {
        this._call( function (iface){
            iface.LiveStream_ItemindexByProfile['finish'] = function(aIndex: number) {
                aCb(aIndex);
            };
            iface.LiveStream_ItemindexByProfile(aProfileName);
        });
    }

    LiveStream_Status(aHandle: number, aType: number, aCb: (aTranscode: number, aRating: number) => void) {
        this._call( function (iface){
            iface.LiveStream_Status['finish'] = function(data) {
                aCb(data[0], data[1]);
            };
            iface.LiveStream_Status(aHandle, aType);
        });
    }
}

export = CDlnaUtil