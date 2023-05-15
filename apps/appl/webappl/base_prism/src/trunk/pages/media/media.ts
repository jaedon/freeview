///<reference path="../../def/sugar.d.ts" />
///<reference path="../../def/prism.d.ts"/>

declare var hx;
import __MODEL__ = require("model/model");
import __PAGE_KERNEL__ = require("pages/page_kernel");
// file name : pages/media/media.js
/*jslint sloppy:true, nomen:true, vars:true */

class cCPgMedia extends __PAGE_KERNEL__ {
    name : any;

    _mediaPage : any;
    _isVisible : boolean;
    _waitingDlg : any;

    constructor() {
        super();
        hx.logadd("CPgMedia");
    }

    create (aContentType, aInfo) {
        var that = this;
        var mSearchInfo = {
            contentType : aContentType,
            server : null,
            folder : null,
            history : [],
            entry : false,
            response : true
        };
        var mOngoingOperations = {};
        var dsrvBridge = hx.svc.BRIDGE;
        var dsrvHistory = hx.svc.HISTORY;
        var dsrvMediaServerManager = hx.svc.MEDIA_SERVER_MANAGER;
        var dsrvMediaItemManager = hx.svc.MEDIA_ITEM_MANAGER;
        var dsrvPlayback = hx.svc.PLAYBACK;
        var dsrvPlaylist = hx.svc.PLAYLIST;
        var dsrvSettingUtil = hx.svc.SETTING_UTIL;
        var dsrvStorageUtil = hx.svc.STORAGE_UTIL;
        var dsrvWebStorage = hx.svc.WEB_STORAGE;
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        if (!dsrvMediaServerManager.isStarted()) {
            dsrvMediaServerManager.startScan();
        }
        var mNetworkAvailable = dsrvSettingUtil.getNetworkAvailable();
        var localServerList = that._getAvailableLocalServerList(aContentType);
        var networkServerList = that._getAvailableNetworkServerList(aContentType);
        var mediaPage = prism.createMediaPage($('#wrap'), aContentType);
        hx.il.updatePageBackground();
        var fnGetUserItemList = (aId, aContentType) : any => {
            var items : any = [];
            var i : number, length : number, info : any, infoList : any, userItemList : any = [];
            if (aId === __MODEL__.__recentGuid__) {
                infoList = hx.svc.HISTORY.getMediaList();
                length = infoList.length;
                for ( i = 0; i < length; i += 1) {
                    info = infoList[i];
                    if (info instanceof __MODEL__.cMediaPlaybackHistoryItem) {
                        if (info.getContentType() === aContentType) {
                            userItemList.push(info);
                        }
                    }
                }
            }
            else if (aId === __MODEL__.__playlistGuid__) {
                userItemList = hx.svc.PLAYLIST.getPlaylist(aContentType);
            }
            return userItemList;
        };
        var fnRequestMediaItems = (aServer, aFolder)=> {
            var fullPath : string, upperFolder : any, serverPath : string, folderPath : string;
            var waitingDlg : any, serverId : any, path : string, category : string, keyword : string;
            mSearchInfo.server = aServer;
            mSearchInfo.folder = aFolder;
            if (!(aServer instanceof __MODEL__.CUserMediaServer)) {
                mediaPage.clearContent();
            }
            if (aServer.isMounted()) {
                serverPath = aServer.getPath(aContentType);
                folderPath = aFolder.getPath(aContentType);
                if (serverPath.toString() !== folderPath.toString() && aServer.getViewType() !== __MODEL__.IMediaServer.TViewType.EOneDepthView) {
                    if (aFolder instanceof __MODEL__.CUpperFolderItem) {
                        mSearchInfo.history.pop();
                        if (mSearchInfo.history.length) {
                            upperFolder = mSearchInfo.history[mSearchInfo.history.length - 1];
                        }
                        else {
                            upperFolder = new __MODEL__.CUpperFolderItem({
                                contentType : aContentType,
                                fullPath : serverPath + '/',
                                parentID : '0'
                            });
                        }
                    }
                    else {
                        upperFolder = new __MODEL__.CUpperFolderItem(aFolder.getPluginObject());
                        mSearchInfo.history.push(upperFolder);
                    }
                    mediaPage.addMediaItems([upperFolder]);
                }
                waitingDlg = prism.createWaitingNotificationDlg(mediaPage);
                waitingDlg.setTextHtml(hx.l('LOC_CONNECTING_DOT_ID'));
                waitingDlg.open();
                that._waitingDlg = waitingDlg;
                if (aServer instanceof __MODEL__.CNetworkServer) {
                    setTimeout(function() {
                        if (that._waitingDlg) {
                            that._waitingDlg.close();
                            if (!mNetworkAvailable) {
                                prism.createButtonDlg(mediaPage, {
                                    textHtml : hx.l('LOC_STR_MESG_904_ID'),
                                    buttonList : [hx.l('LOC_OK_ID')]
                                }).open();
                            }
                        }
                    }, 10000);
                }
                if (aServer instanceof __MODEL__.CUserMediaServer) {
                    serverId = aServer.getId();
                    if(serverId === __MODEL__.__albumGuid__ || serverId === __MODEL__.__artistGuid__) {
                        // Album, Artist
                        path = '/';
                        category = aServer.getStorageCssClass();
                        keyword = '*';
                        if (aFolder.getFileType && aFolder.getFileType() === __MODEL__.IMediaItem.TFileType.EGroup) {
                            mediaPage._contentDataProvider = new CDataProvider([]);
                            keyword = serverId === __MODEL__.__albumGuid__ ? aFolder.getAlbumName() : aFolder.getArtistName();
                        }
                        else {
                            mediaPage.clearContent();
                            category += 'list';
                        }
                        dsrvMediaItemManager.searchContent({
                            path : path,
                            category : category,
                            keyword : keyword
                        });
                    }
                    else {
                        // Recent, Playlist
                        mediaPage.clearContent();
                        onContentBrowse(fnGetUserItemList(serverId, aContentType));
                    }
                }
                else {
                    dsrvMediaItemManager.findMediaItems(mSearchInfo);
                }
            }
            else {
                dsrvMediaItemManager.mount({
                    server : aServer
                });
            }
        };
        var fnRequestPlay = (aItemKey, aSelectedItem)=> {
            hx.log('CPgMedia', hx.util.format('fnRequestPlay() - itemKey : {0}', aItemKey));
            var i : number, length : number, item : any, items : any = [];
            var selectedIndex : number = 0;
            var provider : any = mediaPage.getContentDataProvider();
            var dataArray : any = provider.getDataArray();
            var index : number = 0;
            var storageType : any = mSearchInfo.server.getStorageType();

            length = dataArray.length;
            for (i = 0; i < length; i += 1) {
                item = dataArray[i];
                if (item.getFileType() === __MODEL__.IMediaItem.TFileType.EFile) {
                    if (i === aItemKey) {
                        selectedIndex = index;
                    }
                    index += 1;
                    items.push(item);
                }
            }

            if (storageType === __MODEL__.IMediaServer.TStorageType.ERecent) {
                var btnDelDlg = null;
                var selectItem = items[selectedIndex];
                var bExistItem = hx.svc.MEDIA_ITEM_MANAGER.isExist({
                    path : selectItem.getId()
                });

                if (!bExistItem) {
                    var callbackFuncOfDelDlg = function(p) {
                        if (btnDelDlg !== null) {
                            btnDelDlg = null;
                            mediaPage.doKeyOption()
                        }
                    };

                    btnDelDlg = hx.msg('confirm', {
                        text : hx.l('LOC_MEDIA_RECENT_PLAY_ERROR_ID'),
                        auto_close : false,
                        btn_title_arr : [hx.l('LOC_OK_ID')],
                        timeout : 3000,
                        callback_fn : callbackFuncOfDelDlg
                    });
                } else {
                    dsrvBridge.requestPlay(items, selectedIndex);
                }
            } else if (storageType === __MODEL__.IMediaServer.TStorageType.EPlaylist) {
                var selectItem = items[selectedIndex];
                var bExistItem = hx.svc.MEDIA_ITEM_MANAGER.isExist({
                    path : selectItem.getId()
                });

                if (!bExistItem) {
                    hx.msg('response', {
                        text : 'You cannot play the file on the external storage that is disconnected.',
                        auto_close : true,
                        close_time : 3000
                    });
                } else {
                    dsrvBridge.requestPlay(items, selectedIndex);
                }
            } else {
                dsrvBridge.requestPlay(items, selectedIndex);
            }
        };
        var onContentBrowse = (aMediaItemList)=> {
            if (that._waitingDlg) {
                that._waitingDlg.close();
                delete that._waitingDlg;
                that._waitingDlg = null;
            }
            if (aMediaItemList && aMediaItemList.length) {
                mediaPage.addMediaItems(aMediaItemList);
            }
        };
        hx.log('CPgMedia', 'create() - localServerList.length: ' + localServerList.length);
        hx.log('CPgMedia', 'create() - networkServerList.length: ' + networkServerList.length);
        var userServerList = (function() {
            var list : any = [];
            if (aContentType === __MODEL__.IMediaItem.TContentType.EPhoto) {
                list = __MODEL__.__photoUserMediaServerList__;
            }
            else if (aContentType === __MODEL__.IMediaItem.TContentType.EVideo){
                list = __MODEL__.__videoUserMediaServerList__;
            }
            else if (aContentType === __MODEL__.IMediaItem.TContentType.EMusic){
                list = __MODEL__.__musicUserMediaServerList__;
            }
            return list;
        }());
        mediaPage.setInitServerId(hx.svc.BRIDGE.getTicket());
        mediaPage.setUserServerDataProvider(new CDataProvider(userServerList));
        mediaPage.setLocalServerDataProvider(new CDataProvider(localServerList));
        mediaPage.setNetworkServerDataProvider(new CDataProvider(networkServerList));
        mediaPage.hasFile = function(path) {
            return dsrvMediaItemManager.hasFile({
                path : path
            });
        };
        mediaPage.onMediaServerSelected = (param)=> {
            var item = param.item;
            var server = param.server;
            var dsrvMediaItemManager = hx.svc.MEDIA_ITEM_MANAGER;
            mSearchInfo.history.length = 0;
            mSearchInfo.entry = false;
            mSearchInfo.response = true;
            fnRequestMediaItems(server, item);
        };
        mediaPage.onMediaItemSelected = (param)=> {
            var item = param.item;
            var itemKey = param.itemKey;
            var server : any = param.server;
            var fileType : number = item.getFileType();
            switch (fileType) {
                case __MODEL__.IMediaItem.TFileType.EFolder:
                case __MODEL__.IMediaItem.TFileType.EGroup:
                    mSearchInfo.entry = true;
                    fnRequestMediaItems(server, item);
                    break;
                case __MODEL__.IMediaItem.TFileType.EFile:
                    fnRequestPlay(itemKey, item);
                    break;
            }
        };
        mediaPage.onRequestUserServerList = (aFinished, aContentType)=> {
            setTimeout(function() {
                var i : number, length : number, server : any, serverList : any = [];
                var availableServerList : any = [

                ];
                length = availableServerList.length;
                for (i = 0; i < length; i += 1) {
                    server = availableServerList[i];
                    if (server.getPath() !== mSearchInfo.server.getPath()) {
                        serverList.push(server);
                    }
                }
                aFinished(serverList);
            }, 1);
        };
        mediaPage.onRequestLocalServerList = (aFinished)=> {
            setTimeout(function() {
                var i, length, server, serverList = [];
                var availableServerList = that._getAvailableLocalServerList(mSearchInfo.contentType);
                length = availableServerList.length;
                for (i = 0; i < length; i += 1) {
                    server = availableServerList[i];
                    if (server.getPath() !== mSearchInfo.server.getPath() && !(server instanceof __MODEL__.cSharedServer)) {
                        serverList.push(server);
                    }
                }
                aFinished(serverList);
            }, 1);
        };
        mediaPage.onRequestChangePage = (aParam)=> {
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            dsrvFrontPanel.clearFrontPanelText();
            that.sendEventToEntity({
                alEvent : 'alChangePage',
                target : aParam.target,
                me : that
            });
        };
        mediaPage.onRequestRemoveRecentItem = (aParam)=> {
            var item = aParam.item;
            if (item) {
                dsrvHistory.removeHistoryItem({
                    type : __MODEL__.THistoryType.Media,
                    id : item.getFullPath()
                });
            }
        };
        mediaPage.onRequestAddToPlaylist = (aParam)=> {
            var i : number, length : number, item : any, count : number = 0;
            var items : any = aParam.items;
            var fileType, server, serverId, path, category, keyword;
            items = items instanceof Array ? items : [items];
            length = items.length;
            for (i = 0; i < length; i += 1) {
                item = items[i];
                fileType = (item && item.getFileType) ? item.getFileType() : null;
                if (fileType === __MODEL__.IMediaItem.TFileType.EFile) {
                    dsrvPlaylist.add({
                        item : item
                    });
                    count += 1;
                } else if (fileType === __MODEL__.IMediaItem.TFileType.EGroup) {
                    var server = mSearchInfo.server;
                    if (server) {
                        serverId = server.getId();
                        path = '/mnt/hd2/';
                        category = server.getStorageCssClass();
                        keyword = serverId === __MODEL__.__albumGuid__ ? item.getAlbumName() : item.getArtistName();
                        dsrvMediaItemManager.searchContent({
                            path : path,
                            category : category,
                            keyword : keyword,
                            cb : function(param) {
                                dsrvPlaylist.add({
                                    item : param.item
                                });
                            }
                        });
                        count += 1;
                    }
                } else if (fileType === __MODEL__.IMediaItem.TFileType.EFolder) {
                    server = mSearchInfo.server;
                    if (server) {
                        dsrvMediaItemManager.findMediaItems(mSearchInfo, item.getPath(), function(param) {
                            var i = 0, item = null;
                            var itemList = param.mediaItemList;
                            var len = itemList ? itemList.length : 0;
                            for (i = 0; i < len; i += 1) {
                                item = itemList[i];
                                if (item.getFileType() === __MODEL__.IMediaItem.TFileType.EFile) {
                                    dsrvPlaylist.add({
                                        item: item
                                    });
                                }
                            }
                        });
                        count += 1;
                        //dsrvMediaItemManager.searchContent({~~~~~ use another API to replace this
                    }
                }
            }
            if (count === 1) {
                mediaPage.showNotificationMsg(hx.l('LOC_FILE_ADDED_FROM_PLAYLIST_01_ID'));
            } else if (count > 1) {
                mediaPage.showNotificationMsg(hx.l('LOC_FILES_ADDED_FROM_PLAYLIST_01_ID'));
            }
        };
        mediaPage.onRequestRemoveToPlaylist = (aParam)=> {
            var i: number, length: number, item: any;
            var type : any, id : any;
            var items : any = aParam.items;
            var count = 0;
            items = items instanceof Array ? items : [items];
            length = items.length;
            for(i = 0; i < length; i += 1) {
                item = items[i];
                if (item) {
                    type = item.getType();
                    id = item.getId();
                    dsrvPlaylist.remove({
                        type : type,
                        id : id
                    });
                    count += 1;
                }
            }
            if (length === 1) {
                mediaPage.showNotificationMsg(hx.l('LOC_FILE_REMOVED_FROM_PLAYLIST_01_ID'));
            } else if (length > 1) {
                mediaPage.showNotificationMsg(hx.l('LOC_FILES_REMOVED_FROM_PLAYLIST_01_ID'));
            }
        };
        dsrvStorageUtil.addEventCb('PhysicalStorageDetached', this, (e) => {
            if (mediaPage._currentServerId === e.storage.id) {
                hx.svc.BRIDGE.requestStopPlayback();
            }
        });
        dsrvStorageUtil.addEventCb('DetachProgressOnGoing', that, (e) => {
            hx.log('CPgMedia', 'DetachProgressOnGoing');
        });
        dsrvStorageUtil.addEventCb('DetachProgressFinished', that, (e) => {
            hx.log('CPgMedia', 'DetachProgressFinished');
            mediaPage.showNotificationMsg(hx.l('LOC_STORAGE_REMOVED_SAFELY_ID'));
        });
        dsrvStorageUtil.addEventCb('DetachProgressError', that, (e) => {
            var errMsg = e.errMsg;
            hx.log('CPgMedia', hx.util.format('DetachProgressError - errMsg: {0}', errMsg));
            mediaPage.showNotificationMsg(errMsg);
        });
        dsrvMediaServerManager.addEventCb('ServerAdded', that, (e) => {
            var server = e.server;
            var name = server.getName();
            var path = server.getPath();
            hx.log('CPgMedia', hx.util.format('ServerAdded - name: {0}, path: {1}', name, path));
            mediaPage.addServer(server);
        });
        dsrvMediaServerManager.addEventCb('ServerRemoved', that, (e) => {
            var server = e.server;
            var currentServer = mSearchInfo.server;
            var name = server.getName();
            var path = server.getPath();
            var TStorageType = __MODEL__.IMediaServer.TStorageType;
            var storageType = server.getStorageType();
            hx.log('CPgMedia', hx.util.format('ServerRemoved - name: {0}, path: {1}', name, path));
            hx.log('error','media ServerRemoved');
            if (currentServer && currentServer.getStorageType() === storageType) {
                if (TStorageType.EDlna === storageType && currentServer.getUdn() === server.getUdn()) {
                    hx.log('CPgMedia', hx.util.format('Server UDN - {0}', currentServer.getUdn()));
                    if ( mSearchInfo.response && mNetworkAvailable ) {
                        if(this._mediaPage){
                            this._mediaPage.showNotificationMsg(hx.l('LOC_MSG_PLAYBACK_ERROR_04_ID'));
                        }
                    }

                    setTimeout(function() {
                        if(mediaPage){
                            mediaPage.clearContent();
                            if (mediaPage.isContentFocus()) {
                                //mediaPage.close();
                                that.sendEventToEntity({
                                    alEvent : 'alMessageToPage',
                                    target : 'CPgMain/serverremove'
                                });
                            }
                            mediaPage.removeServer(server);
                        }
                    }, 1500);

                    mSearchInfo.server = null;
                    return;
                } else if (TStorageType.ESamba === storageType && currentServer.getPath() === path) {

                    setTimeout(function() {
                        mediaPage.clearContent();
                        if (mediaPage.isContentFocus()) {
                            mediaPage.doKeyBack();
                        }
                        mediaPage.removeServer(server);
                    }, 500);
                    mSearchInfo.server = null;
                    return;
                }
            }
            mediaPage.removeServer(server);
        });
        dsrvMediaItemManager.addEventCb('ContentBrowse', that, (e) => {
            var mediaItemList = e.mediaItemList;
            onContentBrowse(mediaItemList);
        });
        dsrvMediaItemManager.addEventCb('ContentOperate', that, (e) => {
            var i : number, length : number, ticket : any, result : any, file : any, fileList : any;
            ticket = e.ticket;
            result = e.result;
            fileList = e.fileList;
            hx.log('CPgMedia', hx.util.format('ContentOperate - ticket: {0}, result: {1}', ticket, result));
            mediaPage.setCurrentOperation(result);
            var fnHandleIdle = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleCopying = function() {
                mediaPage.showLoadingMsg(hx.l('LOC_REC_COPYING_ID'));
            };
            var fnHandleCopyComplete = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleCopyError = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleRemoving = function() {
                mediaPage.showLoadingMsg(hx.l('LOC_REC_DELETING_ID'));
            };
            var fnHandleRemoveComplete = function() {
                var file : any;
                hx.log('CPgMedia', '####################################################################################################');
                hx.log('CPgMedia', '----------------------------------------------------------------------------------------------------');
                hx.log('CPgMedia', '[fnHandleRemoveComplete]');
                if (fileList) {
                    for (i = 0; i < fileList.length; i += 1) {
                        file = fileList[i];
                        mediaPage.removeMediaItem(file);
                    }
                }
                hx.log('CPgMedia', '----------------------------------------------------------------------------------------------------');
                hx.log('CPgMedia', '####################################################################################################');
                mediaPage.hideLoadingMsg();
            };
            var fnHandleRemoveError = function() {
                hx.log('CPgMedia', '####################################################################################################');
                hx.log('CPgMedia', '----------------------------------------------------------------------------------------------------');
                hx.log('CPgMedia', '[fnHandleRemoveError]');
                hx.log('CPgMedia', '----------------------------------------------------------------------------------------------------');
                hx.log('CPgMedia', '####################################################################################################');
                mediaPage.hideLoadingMsg();
            };
            var fnHandleMoving = function() {
                mediaPage.showLoadingMsg(hx.l('LOC_REC_MOVING_ID'));
            };
            var fnHandleMoveComplete = function() {
                hx.log('CPgMedia', '####################################################################################################');
                hx.log('CPgMedia', '----------------------------------------------------------------------------------------------------');
                hx.log('CPgMedia', '[fnHandleMoveComplete]');
                hx.log('CPgMedia', '----------------------------------------------------------------------------------------------------');
                hx.log('CPgMedia', '####################################################################################################');
                if (fileList) {
                    for (i = 0; i < fileList.length; i += 1) {
                        mediaPage.removeMediaItem(fileList[i]);
                    }
                }
                mediaPage.hideLoadingMsg();
            };
            var fnHandleMoveError = function() {
                hx.log('CPgMedia', '####################################################################################################');
                hx.log('CPgMedia', '----------------------------------------------------------------------------------------------------');
                hx.log('CPgMedia', '[fnHandleMoveError]');
                hx.log('CPgMedia', '----------------------------------------------------------------------------------------------------');
                hx.log('CPgMedia', '####################################################################################################');
                mediaPage.hideLoadingMsg();
            };
            var fnHandleRenaming = function() {
                hx.log('CPgMedia', 'fnHandleRenaming() - NOT IMPLEMENTED YET');
            };
            var fnHandleRenameComplete = function() {
                hx.log('CPgMedia', 'fnHandleRenameComplete() - NOT IMPLEMENTED YET');
            };
            var fnHandleRenameError = function() {
                hx.log('CPgMedia', 'fnHandleRenameError() - NOT IMPLEMENTED YET');
            };
            var fnHandleMakingDir = function() {
                hx.log('CPgMedia', 'fnHandleMakingDir() - NOT IMPLEMENTED YET');
            };
            var fnHandleMakeDirComplete = function() {
                hx.log('CPgMedia', 'fnHandleMakeDirComplete() - NOT IMPLEMENTED YET');
            };
            var fnHandleMakeDirError = function() {
                hx.log('CPgMedia', 'fnHandleMakeDirError() - NOT IMPLEMENTED YET');
            };
            var fnHandleStopping = function() {
                hx.log('CPgMedia', 'fnHandleStopping() - NOT IMPLEMENTED YET');
            };
            var fnHandleStopComplete = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleStopError = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleScanning = function() {
                hx.log('CPgMedia', 'fnHandleScanning() - NOT IMPLEMENTED YET');
            };
            var fnHandleScanComplete = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleScanError = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleRemovingDir = function() {
                hx.log('CPgMedia', 'fnHandleRemovingDir() - NOT IMPLEMENTED YET');
            };
            var fnHandleRemoveDirComplete = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleRemoveDirError = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleAutoScanStarted = function() {
                hx.log('CPgMedia', 'fnHandleAutoScanStarted() - NOT IMPLEMENTED YET');
            };
            var fnHandleAutoScanComplete = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleAutoScanError = function() {
                mediaPage.hideLoadingMsg();
            };
            var fnHandleInfoError = function() {
                mediaPage.showErrorMsg(hx.l('LOC_MSG_FILE_COPY_ERROR_ID'));
                mediaPage.hideLoadingMsg();
            };
            var fnHandle4GFat32 = function() {
                mediaPage.showErrorMsg(hx.l('LOC_STR_MESG_1650_ID'));
                mediaPage.hideLoadingMsg();
            };
            var fnHandleLackSpace = function() {
                mediaPage.hideLoadingMsg();
                mediaPage.showErrorMsg(hx.l('LOC_STR_MESG_900_ID'));
            };
            var fnHandleUnknownTicket = function() {
                mediaPage.hideLoadingMsg();
            };
            var TOperationState : any = __MODEL__.IMediaItem.TOperationState;
            var operationList : any = [];
            operationList[TOperationState.EIdle] = fnHandleIdle;
            operationList[TOperationState.ECopying] = fnHandleCopying;
            operationList[TOperationState.ECopyComplete] = fnHandleCopyComplete;
            operationList[TOperationState.ECopyError] = fnHandleCopyError;
            operationList[TOperationState.ERemoving] = fnHandleRemoving;
            operationList[TOperationState.ERemoveComplete] = fnHandleRemoveComplete;
            operationList[TOperationState.ERemoveError] = fnHandleRemoveError;
            operationList[TOperationState.EMoving] = fnHandleMoving;
            operationList[TOperationState.EMoveComplete] = fnHandleMoveComplete;
            operationList[TOperationState.EMoveError] = fnHandleMoveError;
            operationList[TOperationState.ERenaming] = fnHandleRenaming;
            operationList[TOperationState.ERenameComplete] = fnHandleRenameComplete;
            operationList[TOperationState.ERenameError] = fnHandleRenameError;
            operationList[TOperationState.EMakingDir] = fnHandleMakingDir;
            operationList[TOperationState.EMakeDirComplete] = fnHandleMakeDirComplete;
            operationList[TOperationState.EMakeDirError] = fnHandleMakeDirError;
            operationList[TOperationState.EStopping] = fnHandleStopping;
            operationList[TOperationState.EStopComplete] = fnHandleStopComplete;
            operationList[TOperationState.EStopError] = fnHandleStopError;
            operationList[TOperationState.EScanning] = fnHandleScanning;
            operationList[TOperationState.EScanComplete] = fnHandleScanComplete;
            operationList[TOperationState.EScanError] = fnHandleScanError;
            operationList[TOperationState.ERemovingDir] = fnHandleRemovingDir;
            operationList[TOperationState.ERemoveDirComplete] = fnHandleRemoveDirComplete;
            operationList[TOperationState.ERemoveDirError] = fnHandleRemoveDirError;
            operationList[TOperationState.EAutoScanStarted] = fnHandleAutoScanStarted;
            operationList[TOperationState.EAutoScanComplete] = fnHandleAutoScanComplete;
            operationList[TOperationState.EAutoScanError] = fnHandleAutoScanError;
            operationList[TOperationState.EInfoError] = fnHandleInfoError;
            operationList[TOperationState.E4GFat32] = fnHandle4GFat32;
            operationList[TOperationState.ELackSpace] = fnHandleLackSpace;
            operationList[TOperationState.EUnknownTicket] = fnHandleUnknownTicket;
            var operation = operationList[result];
            if (operation) {
                operation();
            }
        });
        dsrvMediaItemManager.addEventCb('MountSucceeded', that, (e) => {
            var server : any = e.server;
            var name : any = server.getName();
            var path : any = server.getPath();
            hx.log('CPgMedia', hx.util.format('MountSucceeded - name: {0}, path: {1}', name, path));
            fnRequestMediaItems(server, mSearchInfo.folder);
        });
        dsrvMediaItemManager.addEventCb('MountFailed', that, (e) => {
            var server : any = e.server;
            var name : any = server.getName();
            var path : any = server.getPath();
            hx.log('CPgMedia', hx.util.format('MountFailed - name: {0}, path: {1}', name, path));
            var loginDlg = prism.createLoginDlg(mediaPage);
            loginDlg.setTitleHtml(hx.util.format(tr('LOC_MSG_SAMBA_ENTER_ID'), name));
            loginDlg.setButtonInfoList([{
                text : tr('LOC_OK_ID'),
                fn : function(id, password) {
                    server.setId(id);
                    server.setPassword(password);
                    // server.setId('mspark');
                    // server.setPassword('1');
                    fnRequestMediaItems(server, mSearchInfo.folder);
                    setTimeout(function() {
                        loginDlg.close();
                    }, 1);
                }
            }, {
                text : tr('LOC_CANCEL_ID'),
                fn : function() {
                    setTimeout(function() {
                        loginDlg.close();
                    }, 1);
                }
            }]);
            loginDlg.open({
                dimm : true,
                transition : {
                    newLayer : 'scaleY'
                }
            });
        });
        dsrvSettingUtil.addEventCb('NetworkAvailableChanged', that, (networkAvailable) => {
            mNetworkAvailable = networkAvailable;
        });
        mediaPage.doCopyFile = function(aFileList, aServer) {
            var ticket = dsrvMediaItemManager.copyContent({
                server : aServer,
                fileList : aFileList,
                contentType : mSearchInfo.contentType
            });
        };
        mediaPage.doMoveFile = function(aFileList, aServer) {
            var ticket = dsrvMediaItemManager.moveContent({
                server : aServer,
                fileList : aFileList,
                contentType : mSearchInfo.contentType
            });
        };
        mediaPage.doDeleteFile = function(aFileList) {
            var ticket = dsrvMediaItemManager.deleteContent({
                fileList : aFileList,
                contentType : mSearchInfo.contentType
            });
        };
        mediaPage.doStopCopying = function() {
            dsrvMediaItemManager.stopFileOperation({
                state : __MODEL__.IMediaItem.TOperationState.ECopying
            });
        };
        mediaPage.doStopMoving = function() {
            dsrvMediaItemManager.stopFileOperation({
                state : __MODEL__.IMediaItem.TOperationState.EMoving
            });
        };
        mediaPage.doEject = function(aId) {
            dsrvStorageUtil.detach({
                id : aId
            });
        };
        mediaPage.doRename = function(aId, aName) {
            dsrvStorageUtil.rename({
                id : aId,
                name : aName
            });
        };
        hx.al.compose(that);
        dsrvMediaItemManager.recoveryOperations();
        mediaPage.draw();
        dsrvFrontPanel.setFrontPanelText('-MEDIA-');
        that._mediaPage = mediaPage;
        that._setVisible(true);
        return that;
    }
    destroy () {
        hx.svc.WEB_STORAGE.setLastMediaPageName(this.name);
        var dsrvMediaServerManager = hx.svc.MEDIA_SERVER_MANAGER;
        var dsrvMediaItemManager = hx.svc.MEDIA_ITEM_MANAGER;
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        dsrvMediaServerManager.stopScan();
        dsrvMediaServerManager.clearEventCb(this);
        dsrvMediaItemManager.clearEventCb(this);
        if (this._mediaPage) {
        this._mediaPage.destroy();
            dsrvFrontPanel.clearFrontPanelText();
        }
        this._setVisible(false);
        delete this._mediaPage;
    }
    isActive () : boolean {
        return this._isVisible;
    }
    //visible () : boolean {
    //    return this._isVisible;
    //}
    _setVisible (aVisible : boolean) : void {
        this._isVisible = aVisible;
        this.sendEventToEntity({
            alEvent : 'alCheckPageVisible'
        });
    }
    _getAvailableLocalServerList (aContentType) {
        var localServerList = hx.svc.MEDIA_SERVER_MANAGER.getAvailableLocalServerList({
            contentType : aContentType
        });
        return localServerList;
    }
    _getAvailableNetworkServerList (aContentType) {
        var networkServerList = hx.svc.MEDIA_SERVER_MANAGER.getAvailableNetworkServerList({
            contentType : aContentType
        });
        return networkServerList;
    }
    _exitToLive () : void {
        this.sendEventToEntity({
            alEvent : 'alChangePage',
            target : 'CPgLiveController',
            me : this
        });
    }
    _on_alMessageToPage (aParam) {

    }
    _on_alKey (aParam) : boolean {
        var bConsumed : boolean = false, k = hx.key, kc : number = aParam.alKey;
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        bConsumed = this._mediaPage.doKey(kc);
        hx.log('CPgMedia', hx.util.format('_on_alKey - keyCode: {0}, consumed: {1}', kc, bConsumed));
        if (!bConsumed) {
            switch(kc) {
                case k.KEY_BACK:
                case k.KEY_ESCAPE:
                    this._exitToLive();
                    bConsumed = true;
                    break;
                case k.KEY_SEARCH:
                    this.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgSmartSearch'
                    });
                    bConsumed = true;
                    break;
                default:
                    break;
            }
        }
        return bConsumed;
    }
}
//hx.al.CPgMedia =  cCPgMedia;
export = cCPgMedia;