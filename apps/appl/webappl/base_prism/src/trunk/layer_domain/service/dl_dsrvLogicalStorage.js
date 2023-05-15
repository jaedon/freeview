/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvLogicalStorage.js
/*jslint vars:true */
/*jslint nomen:true */
//    LOGICAL_STORAGE: ['layer_domain/service/dl_dsrvLogicalStorage.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var _event = {
        eUPDATE_DEVICE: 'LogicalUpdateDevice',
        eMOUNT_RESULT: 'LogicalMountResult'
    };
    var cDLLogicalStorage = (function (_super) {
        __extends(cDLLogicalStorage, _super);
        function cDLLogicalStorage(aServiceName) {
            _super.call(this, aServiceName);
            this._list = [];
            this._type = null;
            hx.log("dsrvLogicalStorage", "initializer service - this");
            hx.log("dsvc", "this's _init() called");
            this._initEventHandler();
        }
        //    External API Start
        cDLLogicalStorage.prototype.updateDevice = function (aParam) {
            var type = this._type = aParam.type;
            hx.log('dsrvLogicalStorage', 'updateDevice');
            this._list = [];
            var deviceList = hx.svc.STORAGE_MASS.getDevice();
            var i, j, partitions, partition;
            var nDevice = deviceList.length, nPartition;
            hx.log('dsrvLogicalStorage', 'getDevice deviceList.length ' + nDevice);
            for (i = 0; i < nDevice; i += 1) {
                partitions = deviceList[i].partitions;
                nPartition = partitions.length;
                for (j = 0; j < nPartition; j += 1) {
                    partition = partitions[j];
                    if (partition.kind === 1 && partition.usage === 1 && partition.pvr === 0) {
                        continue;
                    }
                    this._list.push(this._fnAddUIProperty('local', partition, type));
                }
                hx.log('dsrvLogicalStorage', 'this._list.length : ' + this._list.length);
            }
            this._sendEvent(_event.eUPDATE_DEVICE, 'list', this._list);
            // This is request DLNA & SAMBA data server list search request
            this.updateDlna();
            this.updateSamba();
        };
        cDLLogicalStorage.prototype.unmountAll = function (aParam) {
            hx.log('dsrvLogicalStorage', 'unmountAll');
            hx.svc.STORAGE_SAMBA.unmountAll();
        };
        cDLLogicalStorage.prototype.updateDeviceStop = function (aParam) {
            hx.log('dsrvLogicalStorage', 'updateDeviceStop');
            this._list.length = 0;
            hx.svc.STORAGE_DLNA.stopScan();
            hx.svc.STORAGE_SAMBA.stopScan();
        };
        cDLLogicalStorage.prototype.updateDlna = function (aData) {
            hx.log('dsrvLogicalStorage', 'updateDlna');
            hx.svc.STORAGE_DLNA.stopScan();
            hx.svc.STORAGE_DLNA.startScan();
        };
        cDLLogicalStorage.prototype.updateSamba = function () {
            hx.log('dsrvLogicalStorage', 'updateSamba');
            hx.svc.STORAGE_SAMBA.stopScan();
            hx.svc.STORAGE_SAMBA.startScan();
        };
        cDLLogicalStorage.prototype.userMount = function (aParam) {
            hx.log('dsrvLogicalStorage', 'userMount');
            var list = this._list;
            var i;
            for (i = 0; i < list.length; i += 1) {
                if (aParam.id === list[i].uid) {
                    hx.log('dsrvLogicalStorage', 'found mount device : ' + aParam.id);
                    hx.log('dsrvLogicalStorage', 'found mount id : ' + aParam.loginId);
                    hx.log('dsrvLogicalStorage', 'found mount pw : ' + aParam.loginPw);
                    list[i].id = aParam.loginId;
                    list[i].password = aParam.loginPw;
                    break;
                }
            }
            hx.svc.STORAGE_SAMBA.mount(list[i]);
        };
        cDLLogicalStorage.prototype.mount = function (aServer) {
            hx.log('dsrvLogicalStorage', 'mount');
            hx.svc.STORAGE_SAMBA.mount(aServer);
        };
        cDLLogicalStorage.prototype.setStorageName = function (aParam) {
            var list = this._list;
            var i, handled = false;
            for (i = 0; i < list.length; i += 1) {
                if (aParam.uid === list[i].uid) {
                    list[i].label = list[i].name = aParam.name;
                    break;
                }
            }
            var ret = hx.svc.STORAGE_MASS.setStorageName({
                uid: aParam.uid,
                pid: aParam.pid,
                name: aParam.name
            });
        };
        //    External API End
        //    Internal Function Start
        cDLLogicalStorage.prototype._fnTranslator = function (device, partition) {
            hx.log('dsrvLogicalStorage', 'XXXXXX : pairing : ' + device.pairing);
            //hx.log('dsrvLogicalStorage', 'XXXXXX : interface : ' + device.interfaceType);
            hx.log('dsrvLogicalStorage', 'XXXXXX : type : ' + device.type);
            hx.log('dsrvLogicalStorage', 'XXXXXX : kind : ' + device.kind);
            hx.log('dsrvLogicalStorage', 'XXXXXX : usage : ' + device.usage);
            hx.log('dsrvLogicalStorage', 'XXXXXX : path : ' + partition.path);
            hx.log('dsrvLogicalStorage', 'XXXXXX : pvr : ' + partition.pvr);
            hx.log('dsrvLogicalStorage', 'XXXXXX : device label : ' + device.label);
            hx.log('dsrvLogicalStorage', 'XXXXXX : partition label : ' + partition.label);
            return {
                kind: device.kind,
                usage: device.usage,
                id: device.id,
                pid: partition.id,
                label: partition.label,
                path: partition.path,
                pvr: partition.pvr
            };
        };
        cDLLogicalStorage.prototype._getPartitionInfo = function (device, cb) {
            var storages, nStorage, i;
            storages = device.getLogicalStorages();
            nStorage = storages.length;
            hx.log("dsrvLogicalStorage", "[dsrvLogicalStorage][getDevice] device -> LogicalStorageLength : " + nStorage);
            for (i = 0; i < nStorage; i += 1) {
                cb(this._fnTranslator(device, storages.item(i)));
                hx.log("dsrvLogicalStorage", "[dsrvLogicalStorage][_getPartitionInfo] i : " + i);
            }
        };
        cDLLogicalStorage.prototype._fnAddUIProperty = function (type, aStorage, media) {
            hx.log('dsrvLogicalStorage', '_fnAddUIProperty() -- type : ' + type + ' aStorage.kind : ' + aStorage.kind + ' aStorage.usage : ' + aStorage.usage);
            var handler = {
                local: function () {
                    switch (aStorage.kind) {
                        case 1:
                            // HMXPhysicalStorageKind.EKindHDD
                            if (aStorage.usage === 1) {
                                if (hx.config.useInternalPvr === 1) {
                                    aStorage.name = hx.l('LOC_INTERNALHDD_ID');
                                    aStorage.type = 'hdd';
                                }
                                else {
                                    aStorage.name = aStorage.label || hx.l('LOC_USB_ID');
                                    aStorage.type = 'usbp';
                                }
                                aStorage.path += '/Media/' + media;
                            }
                            else if (aStorage.usage === 3) {
                                aStorage.name = aStorage.label || hx.l('LOC_USB_ID');
                                aStorage.type = 'usb';
                            }
                            else {
                                if (hx.config.useInternalPvr === 1) {
                                    aStorage.name = hx.l('LOC_DETACHHDD_ID');
                                    aStorage.type = 'dtc';
                                }
                                else {
                                    aStorage.name = aStorage.label || hx.l('LOC_USB_ID');
                                    aStorage.type = 'usb';
                                }
                            }
                            break;
                        case 2:
                            // HMXPhysicalStorageKind.EKindUSBMemory
                            aStorage.name = aStorage.label || hx.l('LOC_USB_ID');
                            aStorage.type = 'usb';
                            break;
                        case 3:
                            //// HMXPhysicalStorageKind.EKindSDMemory
                            aStorage.name = aStorage.label || hx.l('LOC_SDCARD_ID');
                            aStorage.type = 'sdm';
                            break;
                    }
                    aStorage.isMounted = true;
                    aStorage.uid = aStorage.id;
                },
                dlna: function () {
                    aStorage.uid = aStorage.udn;
                    aStorage.name = aStorage.friendlyName;
                    aStorage.path = '0';
                    aStorage.type = 'dlna';
                    aStorage.network = true;
                    aStorage.isMounted = true;
                },
                samba: function () {
                    aStorage.uid = aStorage.serverName + '/' + aStorage.sharedName;
                    aStorage.name = aStorage.uid;
                    aStorage.path = '/mnt/hd2/smb/' + aStorage.uid;
                    aStorage.type = 'samba';
                    aStorage.network = true;
                }
            };
            aStorage.network = false;
            handler[type]();
            hx.log('dsrvLogicalStorage', '_fnAddUIProperty() result ---- aStorage.name : ' + aStorage.name + ' aStorage.type : ' + aStorage.type);
            return aStorage;
        };
        cDLLogicalStorage.prototype._sendEvent = function (eventType, type, param) {
            var retParam = {
                'type': type,
                'param': param
            };
            hx.log('dsrvLogicalStorage', '_sendEvent eventType: ' + eventType + ' type : ' + type);
            this.fireEvent_Legacy(eventType, retParam);
        };
        cDLLogicalStorage.prototype._removeDevice = function (aId) {
            var list = this._list;
            var i, len, partition = null;
            if (list) {
                for (i = 0; i < list.length; i += 1) {
                    partition = list[i];
                    if (partition.id === aId) {
                        list.splice(i, 1);
                        hx.log('dsrvLogicalStorage', '_removeDevice partition[' + i + '] removed');
                    }
                }
            }
        };
        //    Event
        cDLLogicalStorage.prototype.onPhysicalStorageAttached = function (aStorage) {
            hx.log("dsrvLogicalStorage", "[_onPhysicalStorageAttached]");
            var list = this._list;
            var partitions = aStorage.partitions;
            var len = partitions.length;
            var i, j, partition, ret = [];
            if (list) {
                hx.log('dsrvLogicalStorage', 'partition length : ' + partitions.length);
                for (i = 0; i < partitions.length; i += 1) {
                    partition = partitions[i];
                    var key;
                    for (key in partition) {
                        hx.log('dsrvLogicalStorage', 'key: ' + key + ', value : ' + partition[key]);
                    }
                    if (partition.kind === 1 && partition.usage === 1 && partition.pvr === 0) {
                        continue;
                    }
                    hx.log('dsrvLogicalStorage', 'i : ' + (i - 1) + partition.kind + partition.usage + partition.pvr);
                    partition = this._fnAddUIProperty('local', partition, this._type);
                    hx.log('dsrvLogicalStorage', 'attached device ID: ' + partition.uid);
                    hx.log('dsrvLogicalStorage', 'attached device name: ' + partition.name);
                    hx.log('dsrvLogicalStorage', 'attached device type: ' + partition.type);
                    ret.push(partition);
                    list.push(partition);
                    hx.log('dsrvLogicalStorage', 'pushed');
                }
                hx.log("dsrvLogicalStorage", "[_onPhysicalStorageAttached] i: " + i);
                this._sendEvent(_event.eUPDATE_DEVICE, 'added', ret);
            }
        };
        cDLLogicalStorage.prototype.onPhysicalStorageDetached = function (aStorage) {
            hx.log("dsrvLogicalStorage", "[_onPhysicalStorageDetached]");
            var id = aStorage.id;
            hx.log('dsrvLogicalStorage', 'remove id : ' + id);
            this._removeDevice(id);
            this._sendEvent(_event.eUPDATE_DEVICE, 'removed', id);
        };
        cDLLogicalStorage.prototype.onPhysicalStorageStatus = function (param, status) {
            hx.log("dsrvLogicalStorage", "[_onPhysicalStorageStatus]");
            var list = this._list;
            var id = param;
            var i;
            switch (status) {
                case 9:
                    for (i = 0; i < list.length; i += 1) {
                        if (id === list[i].id) {
                            this._sendEvent(_event.eUPDATE_DEVICE, 'updateerror', status);
                            break;
                        }
                    }
                    break;
                case 13:
                    for (i = 0; i < list.length; i += 1) {
                        if (id === list[i].id) {
                            this._sendEvent(_event.eUPDATE_DEVICE, 'update', list[i]);
                            break;
                        }
                    }
                    break;
                default:
                    break;
            }
        };
        cDLLogicalStorage.prototype.onCDSScan = function (cds, state) {
            hx.log("dsrvLogicalStorage", "[_onCDSScan] : state " + state);
            var list = this._list;
            var i, len;
            var aCds = cds;
            if (state === 1) {
                hx.log("dsrvLogicalStorage", "[_onCDSScan] udn : " + aCds.udn);
                hx.log("dsrvLogicalStorage", "[_onCDSScan] friendlyName : " + aCds.friendlyName);
                hx.log("dsrvLogicalStorage", "[_onCDSScan] ipAddress : " + aCds.ipAddress);
                for (i = 0; i < list.length; i += 1) {
                    hx.log('dsrvLogicalStorage', 'new searched dmp id : ' + aCds.udn);
                    hx.log('dsrvLogicalStorage', 'already searched dmp id : ' + list[i].uid);
                    if (aCds.udn === list[i].uid) {
                        hx.log('dsrvLogicalStorage', 'pass duplicated udn');
                        return;
                    }
                }
                list.push(this._fnAddUIProperty('dlna', aCds));
                this._sendEvent(_event.eUPDATE_DEVICE, 'added', aCds);
            }
            else if (state === 2) {
                for (i = 0; i < list.length; i += 1) {
                    hx.log('dsrvLogicalStorage', 'new searched dmp id : ' + aCds.udn);
                    hx.log('dsrvLogicalStorage', 'already searched dmp id : ' + list[i].uid);
                    if (aCds.udn === list[i].uid) {
                        hx.log('dsrvLogicalStorage', i + ' remove udn');
                        list.splice(i, 1);
                        this._sendEvent(_event.eUPDATE_DEVICE, 'removed', aCds.udn);
                        return;
                    }
                }
            }
        };
        cDLLogicalStorage.prototype.onScanResult = function (paramServer, state) {
            hx.log("dsrvLogicalStorage", "[onScanResult] : state" + state);
            var list = this._list;
            var i, len;
            var server = paramServer;
            if (state === 1) {
                server.uid = server.serverName + '/' + server.sharedName;
                hx.log("dsrvLogicalStorage", "[_onScanResult] uid : " + server.uid);
                //hx.log("dsrvLogicalStorage", "[_onScanResult] serverName : " + server.serverName);
                //hx.log("dsrvLogicalStorage", "[_onScanResult] sharedName : " + server.sharedName);
                //hx.log("dsrvLogicalStorage", "[_onScanResult] ip : " + server.ip);
                //hx.log("dsrvLogicalStorage", "[_onScanResult] isMounted : " + server.isMounted);
                //hx.log("dsrvLogicalStorage", "[_onScanResult] mountPath : " + server.mountPath);
                //hx.log("dsrvLogicalStorage", "[_onScanResult] id : " + server.id);
                //hx.log("dsrvLogicalStorage", "[_onScanResult] passwrd : " + server.password);
                if (server.sharedName[server.sharedName.length - 1] === '$') {
                    hx.log('dsrvLogicalStorage', server.sharedName + ' blocked $');
                    return;
                }
                for (i = 0; i < list.length; i += 1) {
                    if (server.uid === list[i].uid) {
                        hx.log('dsrvLogicalStorage', 'pass duplicated samba id');
                        return;
                    }
                }
                hx.log("dsrvLogicalStorage", "[onScanResult] : " + server.uid + " mounted?" + server.isMounted);
                list.push(this._fnAddUIProperty('samba', server));
                this._sendEvent(_event.eUPDATE_DEVICE, 'added', server);
            }
            else if (state === 2) {
                server.uid = server.serverName + '/' + server.sharedName;
                hx.log("dsrvLogicalStorage", "[_onScanResult : remove] uid : " + server.uid);
                for (i = 0; i < list.length; i += 1) {
                    if (server.uid === list[i].uid) {
                        hx.log('dsrvLogicalStorage', 'removed samba server : ' + server.uid);
                        list.splice(i, 1);
                        this._sendEvent(_event.eUPDATE_DEVICE, 'removed', server.uid);
                        return;
                    }
                }
            }
            //TODO samba remove
        };
        cDLLogicalStorage.prototype.onMountResult = function (server, state) {
            var list = this._list;
            var i;
            var handled = false;
            hx.log("dsrvLogicalStorage", "[onMountResult] : state" + this[state]);
            server.uid = server.serverName + '/' + server.sharedName;
            for (i = 0; i < list.length; i += 1) {
                if (server.uid === list[i].uid) {
                    hx.log("dsrvLogicalStorage", "[onMountResult] : found" + state);
                    hx.log("dsrvLogicalStorage", "[onMountResult] : mounted?" + server.isMounted);
                    if (server.isMounted !== list[i].isMounted) {
                        list[i] = this._fnAddUIProperty('samba', server);
                    }
                    this._sendEvent(_event.eMOUNT_RESULT, 'result', {
                        server: list[i],
                        state: state
                    });
                    handled = true;
                    break;
                }
            }
            /*
             if (!handled) {
             hx.log("dsrvLogicalStorage", "[onMountResult] : " + server.uid + " mounted?" + server.isMounted);
             list.push(this._fnAddUIProperty('samba', server));
             this._sendEvent(e.eUPDATE_DEVICE, 'added', server);
             }
             */
        };
        cDLLogicalStorage.prototype._on_CDSScan = function (params) {
            hx.log("dsrvLogicalStorage", '_on_CDSScan CB ===> ' + 'cds:' + params.cds + ' state: ' + params.state);
            this.onCDSScan(params.cds, params.state);
        };
        cDLLogicalStorage.prototype._on_SambaScanResult = function (params) {
            hx.log("dsrvLogicalStorage", 'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx');
            hx.log("dsrvLogicalStorage", 'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx');
            hx.log("dsrvLogicalStorage", 'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx');
            hx.log("dsrvLogicalStorage", 'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx');
            hx.log("dsrvLogicalStorage", '_on_ScanResult CB ===> ' + 'server:' + params.server + ' state: ' + params.state);
            this.onScanResult(params.server, params.state);
        };
        cDLLogicalStorage.prototype._on_SambaMountResult = function (params) {
            hx.log("dsrvLogicalStorage", '_on_MountResult CB ===> ' + 'server:' + params.server + ' state: ' + params.state);
            this.onMountResult(params.server, params.state);
        };
        cDLLogicalStorage.prototype._on_PhysicalStorageAttached = function (storage) {
            hx.log("dsrvLogicalStorage", '_on_PhysicalStorageAttached CB ===> ' + 'id:' + storage.id);
            var item = {
                id: storage.id,
                partitions: []
            };
            this._getPartitionInfo(storage, function (partition) {
                item.partitions.push(partition);
            });
            this.onPhysicalStorageAttached(item);
        };
        cDLLogicalStorage.prototype._on_PhysicalStorageDetached = function (storage) {
            hx.log("dsrvLogicalStorage", '_on_PhysicalStorageDetached CB ===> ' + 'id:' + storage.id);
            this.onPhysicalStorageDetached(storage);
        };
        cDLLogicalStorage.prototype._on_PhysicalStorageStatus = function (deviceId, aStatus) {
            hx.log("dsrvLogicalStorage", '_on_PhysicalStorageStatus CB ===> ' + 'id:' + deviceId + 'aStatus :' + aStatus);
            this.onPhysicalStorageStatus(deviceId, aStatus);
        };
        cDLLogicalStorage.prototype._initEventHandler = function () {
            var _this = this;
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageDetached', this, function (aStorage) {
                _this._on_PhysicalStorageDetached(aStorage);
            });
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageStatus', this, function (deviceId, aStatus) {
                _this._on_PhysicalStorageStatus(deviceId, aStatus);
            });
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageAttached', this, function (aStorage) {
                _this._on_PhysicalStorageAttached(aStorage);
            });
            hx.svc.STORAGE_DLNA.addEventListener({
                event: 'CDSScan',
                objTarget: this
            });
            hx.svc.STORAGE_SAMBA.addEventListener({
                event: 'SambaScanResult',
                objTarget: this
            });
            hx.svc.STORAGE_SAMBA.addEventListener({
                event: 'SambaMountResult',
                objTarget: this
            });
        };
        return cDLLogicalStorage;
    })(__KERNEL__);
    return cDLLogicalStorage;
});
