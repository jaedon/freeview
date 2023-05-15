/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvStorageMass.js
/*jslint sloppy:true, nomen:true, vars:true */
//    STORAGE_MASS: ['layer_domain/service/dl_dsrvStorageMass.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var eHMXPhysicalStorageInterface;
    (function (eHMXPhysicalStorageInterface) {
        eHMXPhysicalStorageInterface[eHMXPhysicalStorageInterface["EInterfaceUnknown"] = 0] = "EInterfaceUnknown";
        eHMXPhysicalStorageInterface[eHMXPhysicalStorageInterface["EInterfaceSATA"] = 1] = "EInterfaceSATA";
        eHMXPhysicalStorageInterface[eHMXPhysicalStorageInterface["EInterfaceUSB"] = 2] = "EInterfaceUSB";
    })(eHMXPhysicalStorageInterface || (eHMXPhysicalStorageInterface = {}));
    var eHMXPhysicalStorageType;
    (function (eHMXPhysicalStorageType) {
        eHMXPhysicalStorageType[eHMXPhysicalStorageType["ETypeUnknown"] = 0] = "ETypeUnknown";
        eHMXPhysicalStorageType[eHMXPhysicalStorageType["ETypeInternal"] = 1] = "ETypeInternal";
        eHMXPhysicalStorageType[eHMXPhysicalStorageType["ETypeExternal"] = 2] = "ETypeExternal";
    })(eHMXPhysicalStorageType || (eHMXPhysicalStorageType = {}));
    var eHMXPhysicalStorageKind;
    (function (eHMXPhysicalStorageKind) {
        eHMXPhysicalStorageKind[eHMXPhysicalStorageKind["EKindUnknown"] = 0] = "EKindUnknown";
        eHMXPhysicalStorageKind[eHMXPhysicalStorageKind["EKindHDD"] = 1] = "EKindHDD";
        eHMXPhysicalStorageKind[eHMXPhysicalStorageKind["EKindUSBMemory"] = 2] = "EKindUSBMemory";
        eHMXPhysicalStorageKind[eHMXPhysicalStorageKind["EKindSDMemory"] = 3] = "EKindSDMemory";
    })(eHMXPhysicalStorageKind || (eHMXPhysicalStorageKind = {}));
    var eHMXPhysicalStorageUsage;
    (function (eHMXPhysicalStorageUsage) {
        eHMXPhysicalStorageUsage[eHMXPhysicalStorageUsage["EUsageUnknown"] = 0] = "EUsageUnknown";
        eHMXPhysicalStorageUsage[eHMXPhysicalStorageUsage["EUsagePVR"] = 1] = "EUsagePVR";
        eHMXPhysicalStorageUsage[eHMXPhysicalStorageUsage["EUsageSubPVR"] = 2] = "EUsageSubPVR";
        eHMXPhysicalStorageUsage[eHMXPhysicalStorageUsage["EUsageStorage"] = 3] = "EUsageStorage";
        eHMXPhysicalStorageUsage[eHMXPhysicalStorageUsage["EUsageNeedFormat"] = 4] = "EUsageNeedFormat";
        eHMXPhysicalStorageUsage[eHMXPhysicalStorageUsage["EUsageRecStorage"] = 5] = "EUsageRecStorage";
    })(eHMXPhysicalStorageUsage || (eHMXPhysicalStorageUsage = {}));
    var eHMXPhysicalStoragePairing;
    (function (eHMXPhysicalStoragePairing) {
        eHMXPhysicalStoragePairing[eHMXPhysicalStoragePairing["EPairingUnkown"] = 0] = "EPairingUnkown";
        eHMXPhysicalStoragePairing[eHMXPhysicalStoragePairing["EPairingOK"] = 1] = "EPairingOK";
        eHMXPhysicalStoragePairing[eHMXPhysicalStoragePairing["EPairingFail"] = 2] = "EPairingFail";
        eHMXPhysicalStoragePairing[eHMXPhysicalStoragePairing["EPairingNoInfo"] = 3] = "EPairingNoInfo";
        eHMXPhysicalStoragePairing[eHMXPhysicalStoragePairing["EPairingNow"] = 4] = "EPairingNow";
    })(eHMXPhysicalStoragePairing || (eHMXPhysicalStoragePairing = {}));
    ;
    var mPVRList;
    var mRecStorageList;
    var cDLStorageMass = (function (_super) {
        __extends(cDLStorageMass, _super);
        function cDLStorageMass(aServiceName) {
            _super.call(this, aServiceName);
            this.curPhysicalStorage = null;
            hx.logadd('dsrvStorageMass');
            hx.log("dsvc", "initializer service - hx.svc.STORAGE_MASS");
            hx.log("dsrvStorageMass", "this's _init() called");
            this._initEventHandler();
        }
        //    External API Start
        cDLStorageMass.prototype.doDetach = function (aParam) {
            var _this = this;
            var physicalStorages = hx.stbObject.storageUtil.getPhysicalStorages();
            var i, len = physicalStorages.length;
            var ret;
            var pStorage;
            for (i = 0; i < len; i += 1) {
                pStorage = physicalStorages[i];
                if (pStorage.id === parseInt(aParam.id, 10)) {
                    break;
                }
            }
            if (pStorage) {
                this.curPhysicalStorage = pStorage;
                pStorage.onDetachProgress = function (state, errMessage) {
                    hx.log('dsrvStorageMass', 'DSVC onDetachProgress called! - state[' + state + '] errMessage [' + errMessage + ']');
                    _this.fireEvent('DetachProgress', state, errMessage);
                    /*
                     * enum TDetachProgress
                     {
                     EDetachProgressNone,
                     EDetachProgressOnGoing,
                     EDetachProgressFinished,
                     EDetachProgressError
                     };
                     */
                    if (state >= 2) {
                        _this.curPhysicalStorage = null;
                    }
                };
                pStorage.detach();
                ret = true;
            }
            else {
                ret = false;
            }
            if (aParam.cb !== undefined) {
                setTimeout(aParam.cb(ret), 0);
            }
            return ret;
        };
        cDLStorageMass.prototype.doCleanUp = function (aParam) {
            var _this = this;
            var physicalStorages = hx.stbObject.storageUtil.getPhysicalStorages();
            var i, len = physicalStorages.length;
            var ret;
            var pStorage;
            for (i = 0; i < len; i += 1) {
                pStorage = physicalStorages[i];
                if (pStorage.id === parseInt(aParam.id, 10)) {
                    break;
                }
            }
            if (pStorage) {
                this.curPhysicalStorage = pStorage;
                pStorage.onRecoveryProgress = function (state, progress, progressMax, errMessage) {
                    hx.log('dsrvStorageMass', 'DSVC onRecoveryProgress called! - state[' + state + '] progress[' + progress + '] progressMax[' + progressMax + '] errMessage [' + errMessage + ']');
                    _this.fireEvent('RecoveryProgress', state, progress, progressMax, errMessage);
                    if (progress >= progressMax) {
                        _this.curPhysicalStorage = null;
                    }
                };
                pStorage.recovery();
                ret = true;
            }
            else {
                ret = false;
            }
            return ret;
        };
        cDLStorageMass.prototype.doFormat = function (aParam) {
            var _this = this;
            var physicalStorages = hx.stbObject.storageUtil.getPhysicalStorages();
            var i, len = physicalStorages.length;
            var ret;
            var pStorage;
            for (i = 0; i < len; i += 1) {
                pStorage = physicalStorages[i];
                if (pStorage.id === parseInt(aParam.id, 10)) {
                    break;
                }
            }
            if (pStorage) {
                this.curPhysicalStorage = pStorage;
                pStorage.onFormatProgress = function (state, progress, progressMax, errMessage) {
                    hx.log('dsrvStorageMass', 'DSVC onFormatProgress called! - state[' + state + '] progress[' + progress + '] progressMax[' + progressMax + '] errMessage [' + errMessage + ']');
                    _this.fireEvent('FormatProgress', state, progress, progressMax, errMessage);
                    if (progress >= progressMax) {
                        _this.curPhysicalStorage = null;
                    }
                };
                pStorage.formatAll();
                ret = true;
            }
            else {
                ret = false;
            }
            return ret;
        };
        cDLStorageMass.prototype.getDetailInfo = function (aParam) {
            var physicalStorages = hx.stbObject.storageUtil.getPhysicalStorages();
            var i, len = physicalStorages.length;
            var retObj = {};
            var pStorage;
            for (i = 0; i < len; i += 1) {
                pStorage = physicalStorages[i];
                if (pStorage.id === parseInt(aParam.id, 10)) {
                    break;
                }
            }
            if (pStorage) {
                retObj.result = true;
                retObj.availableSize = (parseInt(pStorage.availableSize, 10) / (1048576)).toFixed(1);
                retObj.usedSize = (parseInt(pStorage.usedSize, 10) / (1048576)).toFixed(1);
                retObj.reservedSize = (parseInt(pStorage.reservedSize, 10) / (1048576)).toFixed(1);
                retObj.totalSize = (parseInt(pStorage.totalSize, 10) / (1048576)).toFixed(1);
                retObj.label = pStorage.label;
                retObj.ismount = pStorage.ismount;
            }
            else {
                retObj.result = false;
            }
            if (aParam.cb !== undefined) {
                setTimeout(aParam.cb(retObj), 0);
            }
            return retObj;
        };
        cDLStorageMass.prototype.getPairedStorage = function (type) {
            var that = this;
            var retArray = this.getWholeStorageInfo();
            var i, pairedStorage;
            for (i = 0; i < retArray.length; i += 1) {
                if (type && (type !== retArray[i].type)) {
                    continue;
                }
                switch (retArray[i].type) {
                    case 'internalPvrHDD':
                    case 'externalPvrHDD':
                    case 'externalHDD':
                        if (retArray[i].pairing == 1 /* EPairingOK */ || retArray[i].pairing == 4 /* EPairingNow */) {
                            pairedStorage = retArray[i];
                        }
                        break;
                    case 'usbMemory':
                    case 'sdMemory':
                    case 'unknown':
                    default:
                        break;
                }
            }
            return pairedStorage;
        };
        cDLStorageMass.prototype.getRecPVRSizeInfo = function (aParam) {
            var physicalStorages = hx.stbObject.storageUtil.getPhysicalStorages() //
            , i, ti, pStorage //
            , kindHddIdx = 1 /* EKindHDD */ //
            , retObj //
            , availableSize = 0 //
            , totalSize = 0 //
            , storageCnt = 0;
            if (physicalStorages) {
                ti = physicalStorages.length;
                for (i = 0; i < ti; i += 1) {
                    pStorage = physicalStorages[i];
                    if ((pStorage.kind === kindHddIdx) && pStorage.ismount) {
                        if (pStorage.usage === 1 /* EUsagePVR */ || pStorage.usage === 2 /* EUsageSubPVR */) {
                            availableSize += (parseInt(pStorage.availablePvrSize, 10) / (1048576));
                            totalSize += (parseInt(pStorage.totalSize, 10) / (1048576));
                            storageCnt += 1;
                        }
                    }
                }
            }
            retObj = {
                availableSize: availableSize.toFixed(1),
                totalSize: totalSize.toFixed(1),
                storageCnt: storageCnt
            };
            if (aParam && aParam.cb !== undefined) {
                setTimeout(aParam.cb(retObj), 0);
            }
            return retObj;
        };
        cDLStorageMass.prototype.getRecPVRHDD = function () {
            var retArray;
            var retObj = false;
            var i = 0;
            retObj = false;
            retArray = this.getWholeStorageInfo(undefined);
            if (retArray.length > 0) {
                for (i = 0; i < retArray.length; i += 1) {
                    if (retArray[i].type === 'internalPvrHDD') {
                        retObj = true;
                        break;
                    }
                }
            }
            else {
                retObj = false;
            }
            return retObj;
        };
        cDLStorageMass.prototype.getRecPVRList = function (aSync) {
            if (!mPVRList || aSync === true) {
                var physicalStorages = hx.stbObject.storageUtil.getPhysicalStorages() //
                , i, ti, pStorage //
                , kindHddIdx = 1 /* EKindHDD */ //
                , len;
                mPVRList = [];
                if (physicalStorages) {
                    ti = physicalStorages.length;
                    for (i = 0; i < ti; i += 1) {
                        pStorage = physicalStorages[i];
                        if ((pStorage.kind === kindHddIdx) && pStorage.ismount) {
                            switch (pStorage.usage) {
                                case 1 /* EUsagePVR */:
                                case 2 /* EUsageSubPVR */:
                                    mPVRList.push(pStorage);
                                    break;
                                default:
                                    if (hx.config.useInternalPvr === 0) {
                                        if (pStorage.usage === 3 /* EUsageStorage */) {
                                            mPVRList.push(pStorage);
                                        }
                                    }
                                    break;
                            }
                        }
                    }
                }
            }
            this.getRecStorageList(physicalStorages);
            return mPVRList;
        };
        cDLStorageMass.prototype.getRecStorageList = function (aPhysicalStorages) {
            if (!mRecStorageList) {
                var physicalStorages = (aPhysicalStorages) ? aPhysicalStorages : hx.stbObject.storageUtil.getPhysicalStorages() //
                , i, ti, pStorage;
                mRecStorageList = [];
                if (physicalStorages) {
                    ti = physicalStorages.length;
                    for (i = 0; i < ti; i += 1) {
                        pStorage = physicalStorages[i];
                        if (pStorage.ismount) {
                            switch (pStorage.usage) {
                                case 1 /* EUsagePVR */:
                                case 2 /* EUsageSubPVR */:
                                case 5 /* EUsageRecStorage */:
                                    mRecStorageList.push(pStorage);
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }
            }
            return mRecStorageList;
        };
        cDLStorageMass.prototype.getWholeStorageInfo = function (aParam) {
            var physicalStorages = hx.stbObject.storageUtil.getPhysicalStorages();
            var pStorage, i, j, ti, type, list = [];
            var logicalStorages, nStorage, isNTFS = false;
            if (physicalStorages) {
                ti = physicalStorages.length;
                for (i = 0; i < ti; i += 1) {
                    pStorage = physicalStorages[i];
                    hx.logadd('StorageInfo');
                    hx.log('StorageInfo', 'pStorage.label : ' + pStorage.label);
                    hx.log('StorageInfo', 'pStorage.interfaceType : ' + pStorage.interfaceType);
                    hx.log('StorageInfo', 'pStorage.type : ' + pStorage.type);
                    hx.log('StorageInfo', 'pStorage.kind : ' + pStorage.kind);
                    hx.log('StorageInfo', 'pStorage.usage : ' + pStorage.usage);
                    hx.log('StorageInfo', 'pStorage.pairing : ' + pStorage.pairing);
                    hx.log('StorageInfo', 'pStorage.uuid : ' + pStorage.uuid);
                    hx.log('StorageInfo', 'pStorage.ismount : ' + pStorage.ismount);
                    switch (pStorage.kind) {
                        case 1 /* EKindHDD */:
                            switch (pStorage.usage) {
                                case 1 /* EUsagePVR */:
                                    type = "internalPvrHDD";
                                    break;
                                case 4 /* EUsageNeedFormat */:
                                    if (pStorage.type === 1 /* ETypeInternal */) {
                                        type = "internalPvrHDD";
                                    }
                                    else if (pStorage.type === 2 /* ETypeExternal */) {
                                        type = "externalPvrHDD";
                                    }
                                    break;
                                case 2 /* EUsageSubPVR */:
                                    type = "externalPvrHDD";
                                    break;
                                default:
                                    if (pStorage.interfaceType === 1 /* EInterfaceSATA */) {
                                        type = "internalPvrHDD";
                                    }
                                    else {
                                        type = "externalHDD";
                                    }
                                    break;
                            }
                            break;
                        case 2 /* EKindUSBMemory */:
                            //hx.log("dsrvStorageMass", "pStorage.kind : a.HMXPhysicalStorageKind.EKindUSBMemory");
                            type = "usbMemory";
                            break;
                        case 3 /* EKindSDMemory */:
                            //hx.log("dsrvStorageMass", "pStorage.kind : a.HMXPhysicalStorageKind.EKindSDMemory");
                            type = "sdMemory";
                            break;
                        default:
                            //hx.log("dsrvStorageMass", "pStorage.kind : unknow ");
                            type = "unknown";
                            break;
                    }
                    hx.log("dsrvStorageMass", "pStorage.kind - type : " + type);
                    switch (type) {
                        case "usbMemory":
                        case "sdMemory":
                            if (pStorage.ismount) {
                                list.push({
                                    'type': type,
                                    'label': pStorage.label,
                                    'id': pStorage.id,
                                    'uuid': pStorage.uuid,
                                    'usage': pStorage.usage
                                });
                            }
                            break;
                        case "externalPvrHDD":
                            logicalStorages = pStorage.getLogicalStorages();
                            nStorage = logicalStorages.length;
                            isNTFS = false;
                            hx.log("dsrvStorageMass", "LogicalStorageLength : " + nStorage);
                            for (j = 0; j < nStorage; j += 1) {
                                hx.log("dsrvStorageMass", "logicalStorages[j].formatType : " + logicalStorages[j].formatType);
                                if (logicalStorages[j].formatType === 'NTFS') {
                                    isNTFS = true;
                                    break;
                                }
                            }
                            list.push({
                                'type': type,
                                'label': pStorage.label,
                                'id': pStorage.id,
                                'uuid': pStorage.uuid,
                                'pairing': pStorage.pairing,
                                'isNTFS': isNTFS,
                                'usage': pStorage.usage
                            });
                            break;
                        default:
                            logicalStorages = pStorage.getLogicalStorages();
                            nStorage = logicalStorages.length;
                            isNTFS = false;
                            hx.log("dsrvStorageMass", "LogicalStorageLength : " + nStorage);
                            for (j = 0; j < nStorage; j += 1) {
                                hx.log("dsrvStorageMass", "logicalStorages[j].formatType : " + logicalStorages[j].formatType);
                                if (logicalStorages[j].formatType === 'NTFS') {
                                    isNTFS = true;
                                    break;
                                }
                            }
                            list.push({
                                'type': type,
                                'label': pStorage.label,
                                'id': pStorage.id,
                                'uuid': pStorage.uuid,
                                'pairing': pStorage.pairing,
                                'isNTFS': isNTFS,
                                'usage': pStorage.usage
                            });
                            break;
                    }
                }
            }
            if (aParam && aParam.cb !== undefined) {
                setTimeout(aParam.cb(list), 0);
            }
            return list;
        };
        cDLStorageMass.prototype.getDevice = function () {
            hx.log("dsrvStorageMass", "[dsrvStorageMass][getDevice]");
            var devices = hx.stbObject.storageUtil.getPhysicalStorages(), len = devices.length, i, device, ret = [];
            hx.log("dsrvStorageMass", "[dsrvStorageMass][getDevice]" + "PhysicalLength : " + len);
            for (i = 0; i < len; i += 1) {
                var item = {};
                hx.log("dsrvStorageMass", "[dsrvStorageMass][getDevice]" + "i : " + i);
                device = devices.item(i);
                item.id = device.id;
                item.partitions = [];
                ret.push(item);
                this._getPartitionInfo(device, function (partition) {
                    item.partitions.push(partition);
                });
            }
            hx.log("dsrvStorageMass", "[getDevice end]");
            return ret;
        };
        cDLStorageMass.prototype.setStorageName = function (aParam) {
            var devices = hx.stbObject.storageUtil.getPhysicalStorages(), partitions, uid = aParam.uid, pid = aParam.pid, name = aParam.name, i, j, ret = false, len;
            len = devices.length;
            for (i = 0; i < len; i += 1) {
                if (uid === devices[i].id) {
                    /* not supported
                     partitions = devices[i].getLogicalStorages();
                     for (j = 0; partitions.length; j += 1) {
                     if (pid === partitions[j].id) {
                     partitions[j].label = name;
                     ret = true;
                     break;
                     }
                     }
                     */
                    devices[i].setStorageName(devices[i].uuid, name);
                    break;
                }
            }
            return ret;
        };
        cDLStorageMass.prototype.doSwitchExternalHdd = function (aParam) {
            var devices = hx.stbObject.storageUtil.getPhysicalStorages();
            var i, len = 0, retObj = false, ulStorageId = 0, bOtherAssignPvr = false, ulOtherStorageId = 0;
            len = devices.length;
            // Find already assign PVR
            if (aParam.assignPvr === true) {
                for (i = 0; i < len; i += 1) {
                    if (devices[i].usage === 1 /* EUsagePVR */) {
                        bOtherAssignPvr = true;
                        ulOtherStorageId = devices[i].uuid;
                        break;
                    }
                }
            }
            ulStorageId = Number(aParam.id);
            for (i = 0; i < len; i += 1) {
                if (ulStorageId === devices[i].id) {
                    hx.log("dsrvStorageMass", "doSwitchExternalHdd id :" + devices[i].id + "uuid :" + devices[i].uuid);
                    devices[i].setSwitchHddForPvr(devices[i].uuid, (aParam.assignPvr) ? "true" : "false", (bOtherAssignPvr) ? ulOtherStorageId : "false");
                    break;
                }
            }
        };
        //    External API End
        //    Internal Function Start
        cDLStorageMass.prototype._initEventHandler = function () {
            var _this = this;
            this.defineEvents(['DetachProgress', 'PhysicalStorageDetached', 'FormatProgress', 'PhysicalStorageStatus', 'RecoveryProgress', 'PhysicalStorageAttached']);
            // Apply Channel List Listener.
            hx.stbObject.storageUtil.onPhysicalStorageAttached = function (physicalStorage) {
                hx.log('dsrvStorageMass', 'DSVC onPhysicalStorageAttached called!!!!');
                _this.fireEvent('PhysicalStorageAttached', physicalStorage);
            };
            hx.stbObject.storageUtil.onPhysicalStorageDetached = function (physicalStorage) {
                hx.log('dsrvStorageMass', 'DSVC onPhysicalStorageDetached called!!!!');
                _this.fireEvent('PhysicalStorageDetached', physicalStorage);
            };
            hx.stbObject.storageUtil.onPhysicalStorageStatus = function (id, status) {
                hx.log('dsrvStorageMass', 'DSVC onPhysicalStorageStatus called!!!!');
                _this.fireEvent('PhysicalStorageStatus', id, status);
            };
        };
        cDLStorageMass.prototype._getPartitionInfo = function (device, cb) {
            var storages, nStorage, i;
            storages = device.getLogicalStorages();
            nStorage = storages.length;
            hx.log("dsrvStorageMass", "[dsrvStorageMass][getDevice] device -> LogicalStorageLength : " + nStorage);
            for (i = 0; i < nStorage; i += 1) {
                cb(this._fnTranslator(device, storages.item(i)));
                hx.log("dsrvStorageMass", "[dsrvStorageMass][_getPartitionInfo] i : " + i);
            }
        };
        cDLStorageMass.prototype._fnTranslator = function (device, partition) {
            hx.log('dsrvStorageMass', 'XXXXXX : id : ' + device.id);
            hx.log('dsrvStorageMass', 'XXXXXX : usage : ' + device.usage);
            hx.log('dsrvStorageMass', 'XXXXXX : kind : ' + device.kind);
            hx.log('dsrvStorageMass', 'XXXXXX : path : ' + partition.path);
            hx.log('dsrvStorageMass', 'XXXXXX : pvr : ' + partition.pvr);
            hx.log('dsrvStorageMass', 'XXXXXX : device label : ' + device.label);
            hx.log('dsrvStorageMass', 'XXXXXX : partition label : ' + partition.label);
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
        return cDLStorageMass;
    })(__KERNEL__);
    return cDLStorageMass;
});
