/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvMediaServer.js
/*jslint nomen:true, vars:true */
//    MEDIA_SERVER_MANAGER: ['layer_domain/service/dl_dsrvMediaServerManager.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("../../model/model");

var ESERVER_MANAGER_STARTED = 1;
var ESERVER_MANAGER_STOPPED = 2;

var dsrvDlna = hx.svc.DLNA;
var dsrvSamba = hx.svc.SAMBA;
var dsrvStorageUtil = hx.svc.STORAGE_UTIL;
var dsrvFrontPanel = hx.svc.FRONT_PANEL;

function fnLog(aMessage: string) {
    hx.log('dsrvMediaServerManager', aMessage);
}

class cDLMediaServerManager extends __KERNEL__ implements ifSVCMediaServerManager {
    mNetworkServerList = [];
    mMediaServerManagerState = ESERVER_MANAGER_STOPPED;
    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('dsrvMediaServerManager');
        hx.log('dsrvMediaServerManager', 'create');
        this.defineEvents(['ServerAdded', 'ServerRemoved']);
        dsrvDlna.addEventCb('BrowsingCdsAdded', this, (server) => {
            fnLog('BrowsingCdsAdded');
            this.fnAddServer(server);
        });
        dsrvDlna.addEventCb('BrowsingCdsRemoved', this, (server) => {
            fnLog('BrowsingCdsRemoved');
            this.fnRemoveServer(server);
        });
        dsrvSamba.addEventCb('SambaServerAdded', this, (server) => {
            fnLog('SambaServerAdded');
            this.fnAddServer(server);
        });
        dsrvSamba.addEventCb('SambaServerRemoved', this, (server) => {
            fnLog('SambaServerRemoved');
            this.fnRemoveServer(server);
        });
        dsrvStorageUtil.addEventCb('PhysicalStorageAttached', this, (e) => {
            fnLog('PhysicalStorageAttached');
            var i, length;
            var server, serverList = this.fnGetLocalServerList(e.storage);
            var physicalStorage = e.physicalStorage;
            if (physicalStorage && physicalStorage.ismount) {
                length = serverList.length;
                for (i = 0; i < length; i += 1) {
                    server = serverList[i];
                    this.fireEvent('ServerAdded', {
                        server : server
                    });
                }
            }
        });
        dsrvStorageUtil.addEventCb('PhysicalStorageDetached', this, (e) => {
            fnLog('PhysicalStorageDetached');
            var i, length;
            var server, serverList = this.fnGetLocalServerList(e.storage);
            length = serverList.length;
            for (i = 0; i < length; i += 1) {
                server = serverList[i];
                this.fireEvent('ServerRemoved', {
                    server : server
                });
            }
        });
    }
    private fnGetAvailableServerList(contentType, serverType?): any {
        fnLog(hx.util.format('fnGetAvailableServerList() - contentType: {0}, serverType: {1}', contentType, serverType));
        var availableServerList = [];
        if (serverType !== undefined) {
            if (serverType === __MODEL__.IMediaServer.TLocationType.ELocal) {
                availableServerList = this.fnGetLocalServerList();
            }
            else {
                availableServerList = this.fnGetNetworkServerList();
            }
        }
        else {
            availableServerList = this.fnGetLocalServerList().concat(this.fnGetNetworkServerList());
        }
        return availableServerList;
    }
    private fnGetLocalServerList(storage?): any {
        var i, j, len1, len2, storages, partition, partitions, storageType, shared, server, serverList = [];
        storages = storage ? [storage] : dsrvStorageUtil.getStorages();
        len1 = storages.length;
        for (i = 0; i < len1; i += 1) {
            partitions = storages[i].partitions;
            len2 = partitions.length;
            for (j = 0; j < len2; j += 1) {
                partition = partitions[j];
                server = new __MODEL__.cLocalServer(partition);
                serverList.push(server);
                storageType = server.getStorageType();
                if (storageType === __MODEL__.IMediaServer.TStorageType.EInternalHdd) {
                    if (hx.config.useInternalPvr === 1) {
                        shared = new __MODEL__.cSharedServer(partition);
                        serverList.push(shared);
                    }
                }
            }
        }
        serverList.sort(function (a, b) {
            return a.getStorageType() - b.getStorageType();
        });
        return serverList;
    }
    private fnGetNetworkServerList(): any {
        // return hx.svc.DLNA.getCdsList();
        return this.mNetworkServerList;
    }
    private fnAddServer(server): void {
        var i, length;
        var exists = false;
        length = this.mNetworkServerList.length;
        for (i = 0; i < length; i += 1) {
            if (this.mNetworkServerList[i].getId().toString() === server.getId().toString()) {
                this.mNetworkServerList[i] = server;
                exists = true;
                break;
            }
        }
        if (!exists) {
            this.mNetworkServerList.push(server);
            this.fireEvent('ServerAdded', {
                server : server
            });
        }
    }
    private fnRemoveServer(server): void {
        var i, length, networkServer;
        length = this.mNetworkServerList.length;
        for (i = length - 1; i >= 0; i -= 1) {
            networkServer = this.mNetworkServerList[i];
            if (networkServer.getId().toString() === server.getId().toString()) {
                this.mNetworkServerList.splice(i, 1);
                this.fireEvent('ServerRemoved', {
                    server : server
                });
            }
        }
    }
    // API
    startScan(): void {
        this.mNetworkServerList = [];
        dsrvDlna.startScan();
        dsrvSamba.startScan();
        this.mMediaServerManagerState = ESERVER_MANAGER_STARTED;
    }
    stopScan(): void {
        dsrvDlna.stopScan();
        dsrvSamba.stopScan();
        this.mMediaServerManagerState = ESERVER_MANAGER_STOPPED;
    }
    getAvailableServerList(param): any {
        return this.fnGetAvailableServerList(param.contentType);
    }
    getAvailableLocalServerList(param): any {
        return this.fnGetAvailableServerList(param.contentType, __MODEL__.IMediaServer.TLocationType.ELocal);
    }
    getAvailableNetworkServerList(param): any {
        return this.fnGetAvailableServerList(param.contentType, __MODEL__.IMediaServer.TLocationType.ENetwork);
    }
    isStarted(): boolean {
        return (this.mMediaServerManagerState === ESERVER_MANAGER_STARTED);
    }
}
export = cDLMediaServerManager;