/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvMediaItemManager.js
/*jslint nomen:true, vars:true */
//    MEDIA_ITEM_MANAGER: ['layer_domain/service/dl_dsrvMediaItemManager.js'],

declare var hx:ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("../../model/model");

var mContentBrowseCb: Function = null;
var mContentSearchCb = null;
var dsrvSamba = hx.svc.SAMBA;
var dsrvDlna = hx.svc.DLNA;
var mTicket = null;
var fnLog = function(log) {
    hx.log('dsrvMediaItemManager', log);
};

class cOperationManager {
    private _operations: any;
    private _mediaItemManager: cDLMediaItemManager;
    constructor(aMediaItemManager) {
        this._operations = {};
        this._mediaItemManager = aMediaItemManager;
    }
    add(ticket, state, token?, fileList?, ref_ticket?) {
        this._operations[ticket] = {
            state : state,
            token : token,
            ticket : ticket,
            fileList : fileList,
            ref_ticket : ref_ticket
        };
    }
    remove(ticket) {
        delete this._operations[ticket];
    }
    getOperation(ticket) {
        return this._operations[ticket];
    }
    hasTicket() {
        var operation, operations;
        operations = this._operations;
        for (operation in operations) {
            if (operations.hasOwnProperty(operation)) {
                return true;
            }
        }
        return false;
    }
    recoveryOperations() {
        var ticket, operation, state, token;
        var operations = this._operations;
        var TOperationState = __MODEL__.IMediaItem.TOperationState;
        for (ticket in operations) {
            if (operations.hasOwnProperty(ticket)) {
                state = this._mediaItemManager.getOperationState(ticket);
                operation = operations[ticket];
                // state = operation.state;
                token = operation.token;
                if (state === TOperationState.ECopying || state === TOperationState.ERemoving || state === TOperationState.EMoving) {
                    this._mediaItemManager.fireEvent('ContentOperate', {
                        ticket : ticket,
                        result : state,
                        fileList : operation.fileList
                    });
                }
            }
        }
    }
    stopFileOperation(state) {
        var ticket, operation;
        var operations = this._operations;
        var stopticket;
        var TOperationState = __MODEL__.IMediaItem.TOperationState;
        for (ticket in operations) {
            if (operations.hasOwnProperty(ticket)) {
                operation = operations[ticket];
                if (operation.state === state) {
                    stopticket = this._mediaItemManager.fnStopFileOperation(operation.token);
                    this.add(stopticket, TOperationState.EStopping, operation.token, null, operation.ticket);
                    fnLog(hx.util.format('stopFileOperation() - ticket: {0}, state: {1}', ticket, state));
                }
            }
        }
    }
}

class cDLMediaItemManager extends __KERNEL__ implements ifSVCMediaItemManager {
    private mContentMgr = this.getPluginInstance('ContentManager');
    private mOperationManager;
    constructor(aServiceName:string) {
        super(aServiceName);
        hx.logadd('dsrvMediaItemManager');
        hx.log('dsrvMediaItemManager', 'create');
        this.mOperationManager = new cOperationManager(this);
        this.defineEvents(['ContentBrowse', 'ContentSearch', 'ContentOperate', 'ContentScanSurvey', 'MountSucceeded', 'MountFailed']);
        this.mContentMgr.addEventListener('ContentBrowse', (e) => {
            if (e.ticket === mTicket) {
                this.onContentBrowse(e.contents, e.state);
            }
        });
        this.mContentMgr.addEventListener('ContentSearch', (e) => {
            this.onContentSearch(e.contents, e.state);
        });
        this.mContentMgr.addEventListener('ContentOperate', (e) => {
            this.onContentOperate(e.ticket, e.result);
        });
        this.mContentMgr.addEventListener('ContentScanSurvey', (e) => {
            this.onContentScanSurvey(e.state, e.path);
        });
        dsrvSamba.addEventCb('MountSucceeded', this, (server) => {
            this.onMountSucceeded(server);
        });
        dsrvSamba.addEventCb('MountFailed', this, (server) => {
            this.onMountFailed(server);
        });
        dsrvDlna.addEventCb('BrowsingItemAdded', this, (items) => {
            this.fireEvent('ContentBrowse', {
                mediaItemList : items
            });
        });
    }
    private fnCreateOperationToken(): any {
        var token = this.mContentMgr.createOperationToken();
        return token;
    }
    // API
    autoCopy(aParam): any {
        fnLog(hx.util.format('fnAutoCopy() - type: {0}, src: {1}, recursive: {2}, dest: {3}', aParam.type, aParam.src, aParam.recursive, aParam.dest));
        return this.mContentMgr.autoCopy(aParam.type, aParam.src, aParam.recursive, aParam.dest);
    }
    copyContent(aParam): any {
        var file, src, dest, ext, ticket, token, i, length;
        dest = aParam.server.getPath(aParam.contentType);
        ext = __MODEL__.IMediaItem.ExtensionList[aParam.contentType];
        token = this.mContentMgr.createOperationToken(ext);
        length = aParam.fileList.length;
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog(hx.util.format('fnCopyContent() - dest: {0}', dest));
        for (i = 0; i < length; i += 1) {
            file = aParam.fileList[i];
            src = file.getPath();
            token.addPath(src);
            fnLog(hx.util.format('fnCopyContent() - src: {0}', src));
        }
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        ticket = this.mContentMgr.copyContent(token, dest);
        this.mOperationManager.add(ticket, __MODEL__.IMediaItem.TOperationState.ECopying, token, aParam.fileList);
        return ticket;
    }
    moveContent(aParam): any {
        var file, src, dest, ext, ticket, token, i, length;
        dest = aParam.server.getPath(aParam.contentType);
        ext = __MODEL__.IMediaItem.ExtensionList[aParam.contentType];
        token = this.mContentMgr.createOperationToken(ext);
        length = aParam.fileList.length;
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog(hx.util.format('fnMoveContent() - dest: {0}', dest));
        for (i = 0; i < length; i += 1) {
            file = aParam.fileList[i];
            src = file.getPath();
            token.addPath(src);
            fnLog(hx.util.format('fnMoveContent() - src: {0}', src));
        }
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        ticket = this.mContentMgr.moveContent(token, dest)
        this.mOperationManager.add(ticket, __MODEL__.IMediaItem.TOperationState.EMoving, token, aParam.fileList);
        return ticket;
    }
    deleteContent(aParam): any {
        var file, src, ext, ticket, token, i, length;
        ext = __MODEL__.IMediaItem.ExtensionList[aParam.contentType];
        token =this.mContentMgr.createOperationToken(ext);
        length = aParam.fileList.length;
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        for (i = 0; i < length; i += 1) {
            file = aParam.fileList[i];
            src = file.getPath();
            token.addPath(src);
            fnLog(hx.util.format('fnDeleteContent() - src: {0}', src));
        }
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        ticket = this.mContentMgr.deleteContent(token);
        this.mOperationManager.add(ticket, __MODEL__.IMediaItem.TOperationState.ERemoving, token, aParam.fileList);
        return ticket;
    }
    makeDir(aParam): any {
        fnLog(hx.util.format('fnMakeDir() - folderName: {0}', aParam.folderName));
        return this.mContentMgr.makeDir(aParam.folderName);
    }
    removeDir(aParam): any {
        fnLog(hx.util.format('fnRemoveDir() - folderName: {0}', aParam.folderName));
        return this.mContentMgr.removeDir(aParam.folderName);
    }
    hasFile(aParam): boolean {
        fnLog(hx.util.format('fnHasFile() - path: {0}', aParam.path));
        return this.mContentMgr.hasFile(aParam.path);
    }
    isExist(aParam): boolean {
        fnLog(hx.util.format('fnIsExist() - path: {0}', aParam.path));
        return this.mContentMgr.isExist(aParam.path);
    }
    stopFileOperation(aParam): any {
        fnLog(hx.util.format('fnStopFileOperation() - ticket: {0}', aParam.state));
        return this.mOperationManager.stopFileOperation(aParam.state);
    }
    fnStopFileOperation(ticket): any {
        fnLog(hx.util.format('fnStopFileOperation() - ticket: {0}', ticket));
        return this.mContentMgr.stopFileOperation(ticket);
    }
    getOperationState(aParam): any {
        fnLog(hx.util.format('fnGetOperationState() - ticket: {0}', aParam));
        return this.mContentMgr.getOperationState(aParam);
    }
    getOperationFileList(aParam): any {
        var fileList = null;
        var operation = this.mOperationManager.getOperation(aParam);
        if (operation) {
            fileList = operation.fileList;
        }
        return fileList;
    }
    browseContent(path, type, count, ext, sort, rootPath, cb?): any {
        fnLog(hx.util.format('fnBrowseContent() - path: {0}, type: {1}, count: {2}, ext: {3}, sort: {4}, rootPath: {5}', path, type, count, ext, sort, rootPath));
        mContentBrowseCb = null;
        if (cb) {
            mContentBrowseCb = cb;
        }
        var ticket = this.mContentMgr.browseContent(path, type, count, ext, sort, rootPath);
        mTicket = ticket;
        return ticket;
    }
    reScan(aParam): any {
        fnLog(hx.util.format('fnReScan() - scanPath: {0}, recursive: {1}, ext: {2}', aParam.scanPath, aParam.recursive, aParam.ext));
        return this.mContentMgr.reScan(aParam.scanPath, aParam.recursive, aParam.ext);
    }
    searchContent(aParam): any {
        fnLog(hx.util.format('fnSearchContent() - path: {0}, category: {1}, keyword: {2}', aParam.path, aParam.category, aParam.keyword));
        var count = 16;
        var sort = 0;
        if (aParam.cb) {
            mContentSearchCb = aParam.cb;
        } else {
            mContentSearchCb = null;
        }
        return this.mContentMgr.searchContent(aParam.path, aParam.category, aParam.keyword, count, sort);
    }
    getState(): any {
//        return fnGetState();
        fnLog('fnGetState()');
        return this.mContentMgr.getState();
    }
    requestScanNotify(aParam): void {
        fnLog(hx.util.format('fnRequestScanNotify() - path: {0}', aParam.path));
    }
    mount(aParam): void {
        fnLog('fnMount');
        var TStorageType = __MODEL__.IMediaServer.TStorageType;
        var storageType = aParam.server.getStorageType();
        switch(storageType) {
            case TStorageType.EInternalHdd:
            case TStorageType.EDetachableHdd:
            case TStorageType.EUsb:
            case TStorageType.ESdCard:
            case TStorageType.EDlna:
            case TStorageType.EWoon:
                this.onMountSucceeded(aParam.server);
                break;
            case TStorageType.ESamba:
                dsrvSamba.mount({
                    server : aParam.server
                });
                break;
            default:
                break;
        }
    }
    findMediaItems(aParam, aPath, cb?): void {
        var TViewType = __MODEL__.IMediaServer.TViewType;
        var TStorageType = __MODEL__.IMediaServer.TStorageType;
        var storageType = aParam.server.getStorageType();
        var path = aPath ? aPath : aParam.folder.getPath(aParam.contentType);
        var type = aParam.server.getViewType();
        var count = 16;
        var sort = 0;
        var ext = __MODEL__.IMediaItem.ExtensionList[aParam.contentType];
        var rootPath = aParam.server.getPath(aParam.contentType);
        switch(storageType) {
            case TStorageType.EInternalHdd:
            case TStorageType.EDetachableHdd:
            case TStorageType.EUsb:
            case TStorageType.ESdCard:
            case TStorageType.ESamba:
            case TStorageType.EShared:
                this.browseContent(path, type, count, ext, sort, rootPath, cb);
                break;
            case TStorageType.EDlna:
                dsrvDlna.browseContent({
                    server : aParam.server,
                    folder : aParam.folder,
                    contentType : aParam.contentType
                });
                break;
            case TStorageType.EWoon:
                break;
            default:
                break;
        }
    }
    recoveryOperations(): void {
        this.mOperationManager.recoveryOperations();
    }
    isFileOp(): boolean {
        return this.mOperationManager.hasTicket();
    }
    // EVENT
    onContentBrowse(aContents, aState) {
        if (!aContents) {
            fnLog(hx.util.format('onContentBrowse() - state: {0}', aState));
            return;
        }
        var i, length, mediaItem, mediaItemList = [];
        length = aContents.length;
        fnLog(hx.util.format('onContentBrowse() - contents.length: {0}, state: {1}', length, aState));
        for ( i = 0; i < length; i += 1) {
            mediaItem = new __MODEL__.CMediaItem(aContents.item(i));
            mediaItemList.push(mediaItem);
        }
        if (mContentBrowseCb) {
            mContentBrowseCb({
                mediaItemList : mediaItemList,
                state : aState
            });
        }
        else {
            this.fireEvent('ContentBrowse', {
                mediaItemList : mediaItemList,
                state : aState
            });
        }
    }
    onContentSearch(aContents, aState) {
        if (!aContents) {
            fnLog(hx.util.format('onContentSearch() - state: {0}', aState));
            return;
        }
        fnLog(hx.util.format('onContentSearch() - contents: {0}, state: {1}', aContents, aState));
        var i, length, mediaItem, mediaItemList = [];
        length = aContents.length;
        for ( i = 0; i < length; i += 1) {
            mediaItem = new __MODEL__.CMediaItem(aContents.item(i));
            if (mContentSearchCb) {
                mContentSearchCb({
                    item : mediaItem
                });
            } else {
                mediaItemList.push(mediaItem);
            }
        }
        if (!mContentSearchCb) {
            this.fireEvent('ContentBrowse', {
                mediaItemList: mediaItemList
            });
        }
    }
    onContentOperate(aTicket, aResult) {
        fnLog('##################################################');
        fnLog(hx.util.format('onContentOperate() - ticket: {0}', aTicket));
        fnLog(hx.util.format('onContentOperate() - result: {0}', aResult));
        fnLog('##################################################');
        var TOperationState = __MODEL__.IMediaItem.TOperationState;
        var fileList = this.getOperationFileList(aTicket);
        this.fireEvent('ContentOperate', {
            ticket : aTicket,
            result : aResult,
            fileList : fileList
        });
        switch(aResult) {
            case TOperationState.EStopComplete:
                var oper = this.mOperationManager.getOperation(aTicket);
                if ( oper ) {
                    this.mOperationManager.remove(oper.ref_ticket, aResult);
                }
                this.mOperationManager.remove(aTicket, aResult);
                break;
            case TOperationState.EIdle:
            case TOperationState.ECopyComplete:
            case TOperationState.ECopyError:
            case TOperationState.ERemoveComplete:
            case TOperationState.ERemoveError:
            case TOperationState.EMoveComplete:
            case TOperationState.EMoveError:
            case TOperationState.ERenameComplete:
            case TOperationState.ERenameError:
            case TOperationState.EMakeDirComplete:
            case TOperationState.EMakeDirError:
            case TOperationState.EStopError:
            case TOperationState.EScanComplete:
            case TOperationState.EScanError:
            case TOperationState.ERemoveDirComplete:
            case TOperationState.ERemoveDirError:
            case TOperationState.EAutoScanComplete:
            case TOperationState.EAutoScanError:
                this.mOperationManager.remove(aTicket);
                break;
            // case TOperationState.ECopying:
            // case TOperationState.ERemoving:
            // case TOperationState.EMoving:
            case TOperationState.ERenaming:
            case TOperationState.EMakingDir:
            //case TOperationState.EStopping:
            case TOperationState.EScanning:
            case TOperationState.ERemovingDir:
            case TOperationState.EAutoScanStarted:
                this.mOperationManager.add(aTicket, aResult);
                break;
            case TOperationState.EInfoError:
            case TOperationState.EUnknownTicket:
                break;
            case TOperationState.E4GFat32:
            case TOperationState.ELackSpace:
                this.mOperationManager.remove(aTicket, aResult);
                break;
        }
    }
    onContentScanSurvey(aState, aPath) {
        fnLog(hx.util.format('onContentOperate() - state: {0}, path: {1}', aState, aPath));
        this.fireEvent('ContentScanSurvey', {
            state : aState,
            path : aPath
        });
    }
    onMountSucceeded(aServer) {
        this.fireEvent('MountSucceeded', {
            server : aServer
        });
    }
    onMountFailed(aServer) {
        this.fireEvent('MountFailed', {
            server : aServer
        });
    }
}
export = cDLMediaItemManager;