/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvMediaServer.js
/*jslint nomen:true, vars:true */
//    MEDIA_SERVER_MANAGER: ['layer_domain/service/dl_dsrvMediaServerManager.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "../../model/model"], function (require, exports, __KERNEL__, __MODEL__) {
    var ESERVER_MANAGER_STARTED = 1;
    var ESERVER_MANAGER_STOPPED = 2;
    var dsrvDlna = hx.svc.DLNA;
    var dsrvSamba = hx.svc.SAMBA;
    var dsrvStorageUtil = hx.svc.STORAGE_UTIL;
    var dsrvFrontPanel = hx.svc.FRONT_PANEL;
    function fnLog(aMessage) {
        hx.log('dsrvMediaServerManager', aMessage);
    }
    var cDLMediaServerManager = (function (_super) {
        __extends(cDLMediaServerManager, _super);
        function cDLMediaServerManager(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mNetworkServerList = [];
            this.mMediaServerManagerState = ESERVER_MANAGER_STOPPED;
            hx.logadd('dsrvMediaServerManager');
            hx.log('dsrvMediaServerManager', 'create');
            this.defineEvents(['ServerAdded', 'ServerRemoved']);
            dsrvDlna.addEventCb('BrowsingCdsAdded', this, function (server) {
                fnLog('BrowsingCdsAdded');
                _this.fnAddServer(server);
            });
            dsrvDlna.addEventCb('BrowsingCdsRemoved', this, function (server) {
                fnLog('BrowsingCdsRemoved');
                _this.fnRemoveServer(server);
            });
            dsrvSamba.addEventCb('SambaServerAdded', this, function (server) {
                fnLog('SambaServerAdded');
                _this.fnAddServer(server);
            });
            dsrvSamba.addEventCb('SambaServerRemoved', this, function (server) {
                fnLog('SambaServerRemoved');
                _this.fnRemoveServer(server);
            });
            dsrvStorageUtil.addEventCb('PhysicalStorageAttached', this, function (e) {
                fnLog('PhysicalStorageAttached');
                var i, length;
                var server, serverList = _this.fnGetLocalServerList(e.storage);
                var physicalStorage = e.physicalStorage;
                if (physicalStorage && physicalStorage.ismount) {
                    length = serverList.length;
                    for (i = 0; i < length; i += 1) {
                        server = serverList[i];
                        _this.fireEvent('ServerAdded', {
                            server: server
                        });
                    }
                }
            });
            dsrvStorageUtil.addEventCb('PhysicalStorageDetached', this, function (e) {
                fnLog('PhysicalStorageDetached');
                var i, length;
                var server, serverList = _this.fnGetLocalServerList(e.storage);
                length = serverList.length;
                for (i = 0; i < length; i += 1) {
                    server = serverList[i];
                    _this.fireEvent('ServerRemoved', {
                        server: server
                    });
                }
            });
        }
        cDLMediaServerManager.prototype.fnGetAvailableServerList = function (contentType, serverType) {
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
        };
        cDLMediaServerManager.prototype.fnGetLocalServerList = function (storage) {
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
        };
        cDLMediaServerManager.prototype.fnGetNetworkServerList = function () {
            // return hx.svc.DLNA.getCdsList();
            return this.mNetworkServerList;
        };
        cDLMediaServerManager.prototype.fnAddServer = function (server) {
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
                    server: server
                });
            }
        };
        cDLMediaServerManager.prototype.fnRemoveServer = function (server) {
            var i, length, networkServer;
            length = this.mNetworkServerList.length;
            for (i = length - 1; i >= 0; i -= 1) {
                networkServer = this.mNetworkServerList[i];
                if (networkServer.getId().toString() === server.getId().toString()) {
                    this.mNetworkServerList.splice(i, 1);
                    this.fireEvent('ServerRemoved', {
                        server: server
                    });
                }
            }
        };
        // API
        cDLMediaServerManager.prototype.startScan = function () {
            this.mNetworkServerList = [];
            dsrvDlna.startScan();
            dsrvSamba.startScan();
            this.mMediaServerManagerState = ESERVER_MANAGER_STARTED;
        };
        cDLMediaServerManager.prototype.stopScan = function () {
            dsrvDlna.stopScan();
            dsrvSamba.stopScan();
            this.mMediaServerManagerState = ESERVER_MANAGER_STOPPED;
        };
        cDLMediaServerManager.prototype.getAvailableServerList = function (param) {
            return this.fnGetAvailableServerList(param.contentType);
        };
        cDLMediaServerManager.prototype.getAvailableLocalServerList = function (param) {
            return this.fnGetAvailableServerList(param.contentType, __MODEL__.IMediaServer.TLocationType.ELocal);
        };
        cDLMediaServerManager.prototype.getAvailableNetworkServerList = function (param) {
            return this.fnGetAvailableServerList(param.contentType, __MODEL__.IMediaServer.TLocationType.ENetwork);
        };
        cDLMediaServerManager.prototype.isStarted = function () {
            return (this.mMediaServerManagerState === ESERVER_MANAGER_STARTED);
        };
        return cDLMediaServerManager;
    })(__KERNEL__);
    return cDLMediaServerManager;
});
