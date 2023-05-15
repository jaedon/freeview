var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "model/model", 'pages/util/controls', './prism.page'], function (require, exports, __MODEL__, __CONTROLS__, __PRISM_CPAGE__) {
    /// <reference path="../def/prism.d.ts" />
    /// <reference path="../def/sugar.d.ts" />
    var mPrism;
    (function (mPrism) {
        var TOptionMenuItem = {
            EDelete: 0,
            ECopy: 1,
            EMove: 2,
            EStopCopying: 3,
            EStopMoving: 4,
            ESelect: 5,
            ESelectAll: 6,
            ECancel: 7,
            EEject: 8,
            ERename: 9,
            EDeleteAllFiles: 10,
            EAddToPlaylist: 11,
            ERemoveToPlaylist: 12
        };
        var mViewType = 0 /* ELIST */;
        ;
        var mSelectedServer = null;
        var mSelectedItem = null;
        var mMultiSelection = false;
        var mFileOperationMode = false;
        var mCheckedItems = [];
        var fnGetIndexInCheckedItems = function (item) {
            var i, length;
            length = mCheckedItems.length;
            for (i = 0; i < length; i += 1) {
                if (item.getFullPath() === mCheckedItems[i].getFullPath()) {
                    return i;
                }
            }
            return -1;
        };
        var fnServerItemDrawer = function (param) {
            var item = param.item;
            var $item = param.$item;
            var name = item.getName();
            $item.addClass('prism-button').addClass(item.getStorageCssClass()).text(name);
        };
        var fnSetMultiSelection = function (multiSelection) {
            mCheckedItems = [];
            mMultiSelection = multiSelection;
        };
        var fnDoFileOperation = function (a, operation, data, server) {
            var i, length, item, items;
            if (a[operation]) {
                items = data instanceof Array ? data : [data];
                length = items.length;
                if (length) {
                    a[operation](items, server);
                }
                else {
                    a.showNotificationMsg(tr('LOC_SELECT_MORE_THAN_1_ITEM_ID'));
                }
            }
        };
        var fnOptionMenuHandleDelete = function (a, item) {
            var buttonDlg = prism.createButtonDlg(a);
            var title = item instanceof Array ? "LOC_DELETE_THESE_FILES_ID" : 'LOC_DELETE_THIS_FILE_ID';
            buttonDlg.setTextHtml(tr(title));
            buttonDlg.setButtonInfoList([
                {
                    text: tr('LOC_YES_ID'),
                    fn: function () {
                        fnDoFileOperation(a, 'doDeleteFile', item);
                    }
                },
                {
                    text: tr('LOC_NO_ID')
                }
            ]);
            buttonDlg.open({
                dimm: true
            });
        };
        var fnSortStorageList = function (storageList) {
            var list = storageList.slice(0);
            list.sort(function (a, b) {
                return a.getStorageType() - b.getStorageType();
            });
            return list;
        };
        var fnOptionMenuHandleCopy = function (a, item) {
            a.onRequestLocalServerList(function (serverList) {
                if (serverList.length) {
                    var selectionDlg = prism.createStorageSelectionDlg2(a);
                    selectionDlg.setSelectionItems(fnSortStorageList(serverList));
                    selectionDlg.setSelectionIndex(0);
                    selectionDlg.onSelectionChanged = function (param) {
                        var server = param.item;
                        fnDoFileOperation(a, 'doCopyFile', item, server);
                        selectionDlg.close();
                    };
                    selectionDlg.open({
                        dimm: true,
                        transition: {
                            newLayer: 'scaleY'
                        },
                        onClosed: function () {
                        }
                    });
                }
                else {
                    a.showNotificationMsg(tr('LOC_REC_NOSTORAGE_COPYTO_ID'));
                }
            });
        };
        var fnOptionMenuHandleMove = function (a, item) {
            a.onRequestLocalServerList(function (serverList) {
                if (serverList.length) {
                    var selectionDlg = prism.createStorageSelectionDlg2(a);
                    selectionDlg.setSelectionItems(fnSortStorageList(serverList));
                    selectionDlg.setSelectionIndex(0);
                    selectionDlg.onSelectionChanged = function (param) {
                        var server = param.item;
                        fnDoFileOperation(a, 'doMoveFile', item, server);
                        selectionDlg.close();
                    };
                    selectionDlg.open({
                        dimm: true,
                        transition: {
                            newLayer: 'scaleY'
                        },
                        onClosed: function () {
                        }
                    });
                }
                else {
                    a.showNotificationMsg(tr('LOC_REC_NOSTORAGE_MOVETO_ID'));
                }
            });
        };
        var fnOptionMenuHandleStopCopying = function (a, item) {
            a.doStopCopying();
        };
        var fnOptionMenuHandleStopMoving = function (a, item) {
            a.doStopMoving();
        };
        var fnOptionMenuHandleSelect = function (a, item) {
            var focusedItem, selectedItem, $selectedItem;
            var dataControl = a.getDataControl();
            var data = dataControl._drawnItems.getItemsByArray();
            var i, length = data.length;
            for (i = 0; i < length; i += 1) {
                data[i].addClass('unchecked');
            }
            mFileOperationMode = true;
            fnSetMultiSelection(true);
            focusedItem = dataControl.getFocusedItem();
            if (focusedItem) {
                selectedItem = focusedItem.item;
                $selectedItem = focusedItem.$item;
                mCheckedItems.push(selectedItem);
                $selectedItem.removeClass('unchecked');
                $selectedItem.addClass('checked');
            }
            a.reloadContextMenuItems(dataControl);
        };
        var fnOptionMenuHandleSelectAll = function (a, item) {
            var newItem;
            var dataControl = a.getDataControl();
            var data = dataControl._drawnItems.getItemsByArray();
            var dataProvider = dataControl._dataProvider;
            var i, length = data.length;
            for (i = 0; i < length; i += 1) {
                data[i].removeClass('unchecked');
                data[i].addClass('checked');
            }
            mFileOperationMode = true;
            fnSetMultiSelection(true);
            length = dataProvider.getCount();
            if (length) {
                for (i = 0; i < length; i += 1) {
                    newItem = dataProvider.getItem(i);
                    if (!(newItem instanceof __MODEL__.CUpperFolderItem)) {
                        mCheckedItems.push(newItem);
                    }
                }
            }
            a.reloadContextMenuItems(dataControl);
        };
        var fnOptionMenuHandleCancel = function (a, item) {
        };
        var fnOptionMenuHandleEject = function (a, item) {
            var id = item.getId();
            a.doEject(id);
        };
        var fnOptionMenuHandleRename = function (a, item) {
            var renameDlg = prism.createStorageRenameDlg(a);
            renameDlg.setButtonInfoList([
                {
                    text: tr('LOC_OK_ID'),
                    fn: function (name) {
                        var id = item.getId();
                        a.doRename(id, name);
                        var index = a._localServerDataProvider.getItemIndex(item, function (currentItem) {
                            if (currentItem.getId() === item.getId()) {
                                return true;
                            }
                            return false;
                        });
                        if (index > -1) {
                            item.setName(name);
                            a._localServerDataProvider.updateItem(index, item);
                            a._localServerListControl.redraw();
                        }
                        setTimeout(function () {
                            renameDlg.close();
                        }, 1);
                    }
                },
                {
                    text: tr('LOC_CANCEL2_ID'),
                    fn: function () {
                        renameDlg.close();
                    }
                }
            ]);
            renameDlg.open({
                dimm: true,
                transition: {
                    newLayer: 'scaleY'
                }
            });
        };
        var fnOptionMenuHandleDeleteAllFiles = function (a, item) {
            var text = tr('LOC_DELETE_ALL_PHOTO_FILES');
            if (a instanceof CMusicPage) {
                text = tr('LOC_DELETE_ALL_MUSIC_FILES');
            }
            else if (a instanceof CVideoPage) {
                text = tr('LOC_DELETE_ALL_VIDEO_FILES');
            }
            var buttonDlg = prism.createButtonDlg(a);
            buttonDlg.setTextHtml(text);
            buttonDlg.setButtonInfoList([
                {
                    text: tr('LOC_YES_ID'),
                    fn: function () {
                        fnDoFileOperation(a, 'doDeleteFile', item);
                    }
                },
                {
                    text: tr('LOC_NO_ID')
                }
            ]);
            buttonDlg.open({
                dimm: true
            });
        };
        var fnOptionMenuHandleAddToPlaylist = function (a, items) {
            a.onRequestAddToPlaylist({
                items: items
            });
        };
        var fnOptionMenuHandleRemoveToPlaylist = function (a, items) {
            a.onRequestRemoveToPlaylist({
                items: items
            });
            a.removeMediaItem(items);
        };
        var fnOptionMenuProvider = function (a, activeFocusInfo) {
            var ret;
            prism.log(hx.util.format('prism.page.media.js: fnOptionMenuProvider - activeFocusInfo = {0}', activeFocusInfo));
            if (mSelectedServer && mSelectedServer.getLocationType() === __MODEL__.IMediaServer.TLocationType.ENetwork) {
                return ret;
            }
            if (activeFocusInfo) {
                var control = activeFocusInfo.control;
                if (control === null) {
                    return ret;
                }
                var focusedItemInfo = control.getFocusedItem();
                var item = focusedItemInfo ? focusedItemInfo.item : null;
                if (!item) {
                    return ret;
                }
                if (item instanceof __MODEL__.CUpperFolderItem) {
                    if (!mFileOperationMode) {
                        fnSetMultiSelection(false);
                    }
                    return ret;
                }
                var menuItemList = [];
                var menuItems = [];
                var TOperationState = __MODEL__.IMediaItem.TOperationState;
                var storageType = item.getStorageType ? item.getStorageType() : null;
                var fileType = item.getFileType ? item.getFileType() : null;
                var serverId = mSelectedServer && mSelectedServer.getId ? mSelectedServer.getId() : null;
                var fnMenuItemCreator = function (name, fn, noclose, redraw) {
                    return {
                        name: name,
                        fn: function () {
                            prism.log(hx.util.format('prism.page.media.js: fnMenuItemCreator - name = {0}', name));
                            fn(a, mMultiSelection ? mCheckedItems : item);
                        },
                        noclose: noclose,
                        redraw: redraw
                    };
                };
                menuItemList[TOptionMenuItem.EDelete] = fnMenuItemCreator(tr('LOC_DELETE_ID'), fnOptionMenuHandleDelete);
                menuItemList[TOptionMenuItem.ECopy] = fnMenuItemCreator(tr('LOC_COPY_ID'), fnOptionMenuHandleCopy);
                menuItemList[TOptionMenuItem.EMove] = fnMenuItemCreator(tr('LOC_MOVE_ID'), fnOptionMenuHandleMove);
                menuItemList[TOptionMenuItem.EStopCopying] = fnMenuItemCreator(tr('LOC_REC_STOP_COPYING_ID'), fnOptionMenuHandleStopCopying);
                menuItemList[TOptionMenuItem.EStopMoving] = fnMenuItemCreator(tr('LOC_REC_STOP_MOVING_ID'), fnOptionMenuHandleStopMoving);
                menuItemList[TOptionMenuItem.ESelect] = fnMenuItemCreator(tr('LOC_SELECT_ID'), fnOptionMenuHandleSelect);
                menuItemList[TOptionMenuItem.ESelectAll] = fnMenuItemCreator(tr('LOC_SELECT_ALL_ID'), fnOptionMenuHandleSelectAll, true, true);
                menuItemList[TOptionMenuItem.ECancel] = fnMenuItemCreator(tr('LOC_CANCEL2_ID'), fnOptionMenuHandleCancel);
                menuItemList[TOptionMenuItem.EEject] = fnMenuItemCreator(tr('LOC_STR_EJECT_ID'), fnOptionMenuHandleEject);
                menuItemList[TOptionMenuItem.ERename] = fnMenuItemCreator(tr('LOC_RENAME_ID'), fnOptionMenuHandleRename);
                menuItemList[TOptionMenuItem.EDeleteAllFiles] = fnMenuItemCreator(tr('LOC_STR_DELETE_ALL_FILES_ID'), fnOptionMenuHandleDeleteAllFiles);
                menuItemList[TOptionMenuItem.EAddToPlaylist] = fnMenuItemCreator(tr('LOC_ADD_PLAYLIST_LOWER_ID'), fnOptionMenuHandleAddToPlaylist);
                menuItemList[TOptionMenuItem.ERemoveToPlaylist] = fnMenuItemCreator(tr('LOC_REMOVE_FROM_PLAYLIST_ID'), fnOptionMenuHandleRemoveToPlaylist);
                if (item instanceof __MODEL__.IMediaServer) {
                    if (storageType === __MODEL__.IMediaServer.TStorageType.EUsb || storageType === __MODEL__.IMediaServer.TStorageType.ESdCard) {
                        menuItems.push(menuItemList[TOptionMenuItem.EEject]);
                        menuItems.push(menuItemList[TOptionMenuItem.ERename]);
                        menuItems.push(menuItemList[TOptionMenuItem.EDeleteAllFiles]);
                        ret = {
                            text: tr('LOC_EDIT_ID'),
                            menuItems: menuItems
                        };
                    }
                    return ret;
                }
                if (storageType === __MODEL__.IMediaServer.TStorageType.ERecent) {
                    return {
                        text: tr('LOC_REMOVE_ID').toUpperCase(),
                        menuItems: []
                    };
                }
                if (storageType === __MODEL__.IMediaServer.TStorageType.EPlaylist) {
                    if (mFileOperationMode) {
                        menuItems.push(menuItemList[TOptionMenuItem.ERemoveToPlaylist]);
                        menuItems.push(menuItemList[TOptionMenuItem.ECancel]);
                    }
                    else {
                        menuItems.push(menuItemList[TOptionMenuItem.ERemoveToPlaylist]);
                        menuItems.push(menuItemList[TOptionMenuItem.ESelect]);
                        menuItems.push(menuItemList[TOptionMenuItem.ESelectAll]);
                    }
                }
                prism.log(hx.util.format('prism.page.media.js: fnOptionMenuProvider - a._currentOperation = {0}', a._currentOperation));
                switch (a._currentOperation) {
                    case TOperationState.EIdle:
                    case TOperationState.ECopyComplete:
                    case TOperationState.ECopyError:
                    case TOperationState.ERemoveComplete:
                    case TOperationState.ERemoveError:
                    case TOperationState.EMoveComplete:
                    case TOperationState.EMoveError:
                    case TOperationState.ERenaming:
                    case TOperationState.ERenameComplete:
                    case TOperationState.ERenameError:
                    case TOperationState.EMakingDir:
                    case TOperationState.EMakeDirComplete:
                    case TOperationState.EMakeDirError:
                    case TOperationState.EStopping:
                    case TOperationState.EStopComplete:
                    case TOperationState.EStopError:
                    case TOperationState.EScanning:
                    case TOperationState.EScanComplete:
                    case TOperationState.EScanError:
                    case TOperationState.ERemovingDir:
                    case TOperationState.ERemoveDirComplete:
                    case TOperationState.ERemoveDirError:
                    case TOperationState.EAutoScanStarted:
                    case TOperationState.EAutoScanComplete:
                    case TOperationState.EAutoScanError:
                    case TOperationState.EInfoError:
                    case TOperationState.E4GFat32:
                    case TOperationState.ELackSpace:
                    case TOperationState.EUnknownTicket:
                        break;
                    case TOperationState.ECopying:
                        menuItems.push(menuItemList[TOptionMenuItem.EStopCopying]);
                        break;
                    case TOperationState.EMoving:
                        menuItems.push(menuItemList[TOptionMenuItem.EStopMoving]);
                        break;
                    case TOperationState.ERemoving:
                        menuItems.push(menuItemList[TOptionMenuItem.EDelete]);
                        menuItems.push(menuItemList[TOptionMenuItem.ESelectAll]);
                        break;
                }
                prism.log(hx.util.format('prism.page.media.js: fnOptionMenuProvider - menuItems.length = {0}', menuItems.length));
                if (menuItems.length === 0) {
                    if (a._type !== __MODEL__.IMediaItem.ContentTypeCssList[__MODEL__.IMediaItem.TContentType.EVideo]) {
                        menuItems.push(menuItemList[TOptionMenuItem.EAddToPlaylist]);
                    }
                    if (mFileOperationMode) {
                        if (fileType !== __MODEL__.IMediaItem.TFileType.EGroup && serverId !== __MODEL__.__artistGuid__ && serverId !== __MODEL__.__albumGuid__) {
                            menuItems.push(menuItemList[TOptionMenuItem.EDelete]);
                            menuItems.push(menuItemList[TOptionMenuItem.ECopy]);
                            menuItems.push(menuItemList[TOptionMenuItem.EMove]);
                        }
                        menuItems.push(menuItemList[TOptionMenuItem.ECancel]);
                    }
                    else {
                        if (fileType !== __MODEL__.IMediaItem.TFileType.EGroup && serverId !== __MODEL__.__artistGuid__ && serverId !== __MODEL__.__albumGuid__) {
                            menuItems.push(menuItemList[TOptionMenuItem.EDelete]);
                            menuItems.push(menuItemList[TOptionMenuItem.ECopy]);
                            menuItems.push(menuItemList[TOptionMenuItem.EMove]);
                        }
                        menuItems.push(menuItemList[TOptionMenuItem.ESelect]);
                        menuItems.push(menuItemList[TOptionMenuItem.ESelectAll]);
                    }
                }
                if (!mFileOperationMode) {
                    fnSetMultiSelection(false);
                }
            }
            ret = {
                text: tr('LOC_EDIT_ID'),
                menuItems: menuItems
            };
            return ret;
        };
        var fnCreateViewType = function (a) {
            var viewTypeBtnDrawer = function (index, $item) {
                var $icon, $text;
                var menuText;
                if (index === 0) {
                    menuText = 'LIST';
                }
                else {
                    menuText = 'GRID';
                }
                if (mViewType === 0 /* ELIST */) {
                    if (index === 0) {
                        $item.addClass('selected');
                    }
                }
                else {
                    if (index === 1) {
                        $item.addClass('selected');
                    }
                }
                if (a._isMultiSelect) {
                    $item.addClass('dim30');
                }
                else {
                    $item.removeClass('dim30');
                }
                $item.addClass(menuText.toLowerCase());
                $icon = $('<img>');
                $text = $('<div>').text(menuText);
                $item.append($icon).append($text);
                return true;
            };
            //var viewTypeControl = new CHorizontalLayoutControl(null);
            //viewTypeControl.addClass('viewType');
            //viewTypeControl.setDrawParam({
            //	padding : 1
            //});
            //viewTypeControl.setItemDrawers([viewTypeBtnDrawer, viewTypeBtnDrawer]);
            //viewTypeControl.onItemSelected = function (index, $item) {
            //	a._viewTypeControl._$self.children().removeClass('selected');
            //	$item.addClass('selected');
            //	//a._recordingView.destroy();
            //	//a._recordingView = null;
            //	//if (index === 0) {
            //	//	a.drawListRecordings();
            //	//} else {
            //	//	a.drawGridRecordings();
            //	//}
            //};
            //viewTypeControl.doShow = function() {
            //	viewTypeControl.addClass('show');
            //};
            //viewTypeControl.doHide = function() {
            //	viewTypeControl.removeClass('show');
            //};
            //viewTypeControl.setFocusedItem(0);
            //viewTypeControl.addClass('show');
            //a._viewTypeControl = viewTypeControl;
        };
        var fnCreateMenu = function (a) {
            var tryToSelectServer = function (id, control) {
                var i, length, data, dataArray, provider;
                provider = control._dataProvider;
                if (provider) {
                    dataArray = provider.getDataArray();
                    for (i = 0; i < dataArray.length; i += 1) {
                        data = dataArray[i];
                        if (data.getId().toString() === id.toString()) {
                            control.setFocusedItemByKey(i);
                            return true;
                        }
                    }
                }
                return false;
            };
            var j, result, control, controlList, initFocus = 0;
            var userServerListControl = new CListControl(null);
            userServerListControl['showOption'] = false;
            userServerListControl.setDataProvider(a._userServerDataProvider);
            userServerListControl.setDrawParam({
                itemHeight: 62,
                scrollByItemSize: true
            });
            userServerListControl.setDataDrawer(fnServerItemDrawer);
            userServerListControl.onItemSelected = function (param) {
                onMediaServerSelected(a, param);
            };
            a.addOptionMenuTarget(userServerListControl);
            var localServerListControl = new CListControl(null);
            localServerListControl['showOption'] = true;
            localServerListControl.setDataProvider(a._localServerDataProvider);
            localServerListControl.setDrawParam({
                itemHeight: 62,
                scrollByItemSize: true
            });
            localServerListControl.setDataDrawer(fnServerItemDrawer);
            localServerListControl.onItemSelected = function (param) {
                onMediaServerSelected(a, param);
            };
            a.addOptionMenuTarget(localServerListControl);
            var networkServerListControl = new CListControl(null);
            networkServerListControl['showOption'] = false;
            networkServerListControl.setDataProvider(a._networkServerDataProvider);
            networkServerListControl.setDrawParam({
                itemHeight: 62,
                scrollByItemSize: true
            });
            networkServerListControl.setDataDrawer(fnServerItemDrawer);
            networkServerListControl.onItemSelected = function (param) {
                onMediaServerSelected(a, param);
            };
            a._userServerListControl = userServerListControl;
            a._localServerListControl = localServerListControl;
            a._networkServerListControl = networkServerListControl;
            var menuView = new CViewGroupControl(a.$menuArea());
            controlList = [userServerListControl, localServerListControl, networkServerListControl];
            if (a._initServerId !== undefined) {
                for (j = 0; j < controlList.length; j += 1) {
                    control = controlList[j];
                    result = tryToSelectServer(a._initServerId, control);
                    if (result) {
                        initFocus = j;
                        break;
                    }
                }
            }
            //localServerListControl.marqueeOn('em');
            //networkServerListControl.marqueeOn('em');
            //menuView.enableTop();
            menuView.setChildControl(controlList, initFocus);
            menuView.addClass('show');
            a._menuView = menuView;
            return {
                menu: menuView,
                cb: function () {
                    setTimeout(function () {
                        var c = controlList[initFocus];
                        c.setActiveFocus(true);
                        c.doKeyOk();
                    }, 1);
                }
            };
        };
        var fnCreateContent = function (a) {
            var dataControl = null;
            if (a instanceof CMusicPage) {
                dataControl = new CGridControl(null);
                dataControl.setDrawParam({
                    maxColCount: 5,
                    itemWidth: 159 + 18,
                    itemHeight: 159 + 18,
                    scrollByItemSize: true,
                    scrollPageSizeByPageUpDownKey: true,
                });
            }
            else {
                dataControl = new CGridControl(null);
                dataControl.setDrawParam({
                    maxColCount: 4,
                    itemWidth: 210 + 10,
                    itemHeight: 118 + 10,
                    scrollByItemSize: true,
                    scrollPageSizeByPageUpDownKey: true
                });
            }
            a._contentDataProvider = a._contentDataProvider || new CDataProvider([]);
            dataControl.setDataDrawer(a.getItemDrawer());
            dataControl.setDataProvider(a._contentDataProvider);
            dataControl.onItemSelected = function (param) {
                var $item = param.$item;
                var item = param.item;
                var index = -1;
                if (mMultiSelection) {
                    if ($item.hasClass('checked')) {
                        $item.removeClass('checked');
                        $item.addClass('unchecked');
                        index = fnGetIndexInCheckedItems(item);
                        if (index > -1) {
                            mCheckedItems.splice(index, 1);
                        }
                    }
                    else {
                        $item.removeClass('unchecked');
                        $item.addClass('checked');
                        mCheckedItems.push(item);
                    }
                }
                else {
                    onMediaItemSelected(a, param);
                }
            };
            a.addOptionMenuTarget(dataControl);
            a._dataControl = dataControl;
            dataControl.addEventHandler('FocusChanged', dataControl, function ($old, $new) {
                if (a._$contentsIndex) {
                    var dpMedia = a._contentDataProvider;
                    var total, focusedIndex, text;
                    var focusedItem, $focusedItem;
                    var newIndex, oldIndex;
                    if (dpMedia) {
                        total = dpMedia.getCount();
                        focusedItem = a._dataControl.getFocusedItem();
                        if (focusedItem) {
                            $focusedItem = focusedItem.$item;
                            if ($focusedItem) {
                                focusedIndex = $focusedItem.attr('data');
                                text = 'Total ' + '<span>' + (parseInt(focusedIndex) + 1) + ' / ' + total + '</span>';
                                a._$contentsIndex.html(text);
                            }
                        }
                    }
                }
            });
            var mediaViewContainer = new CViewGroupControl(a.$contentArea());
            mediaViewContainer.setDrawParam({
                showScroll: true
            });
            mediaViewContainer.setChildControl([dataControl]);
            a._mediaViewContainer = mediaViewContainer;
            var view = new CVerticalGroupControl(null);
            //view.setChildControl([a._viewTypeControl, a._mediaViewContainer], 1);
            view.setChildControl([a._mediaViewContainer]);
            a._contentView = view;
            return view;
        };
        var fnNotFoundMsgOnOff = function (a, onOff) {
            var $contentArea = a.$contentArea();
            var $notFound = $contentArea.find('.noContent');
            var info = a.getNotFoundText();
            var title = info.title;
            var content = info.content;
            if (onOff) {
                if (!$notFound.length) {
                    $contentArea.append($('<div>', {
                        'class': 'noContent img'
                    })).append($('<div>', {
                        'class': 'noContent title',
                        'html': title
                    })).append($('<div>', {
                        'class': 'noContent context',
                        'html': hx.util.format('<div>{0}</div>', content)
                    }));
                    $contentArea.addClass(a.getType());
                    var doKeyRight = function () {
                        var handled = false;
                        if (a.stackSize() === 0) {
                            a.doKeyRight = doKeyRight;
                            handled = true;
                        }
                        else {
                            a.doKeyRight = undefined;
                            handled = CLayeredGroupControl.prototype.doKey.call(a, hx.key.KEY_RIGHT);
                        }
                        return handled;
                    };
                    a.doKeyRight = doKeyRight;
                }
                if (a._viewTypeControl) {
                    a._viewTypeControl.removeClass('show');
                }
            }
            else {
                if ($notFound.length) {
                    $contentArea.removeClass(a.getType());
                    $notFound.remove();
                    a.doKeyRight = undefined;
                }
                if (a._viewTypeControl) {
                    a._viewTypeControl.addClass('show');
                }
            }
        };
        var fnContentsIndexOnOff = function (a, onOff) {
            var $layer = a.$layer();
            var $contentsIndex = a._$contentsIndex;
            var dpMedia = a._contentDataProvider;
            var focusedItem, $focusedItem;
            var total, focusedIndex, text;
            if (dpMedia) {
                total = dpMedia.getCount();
                if (a._dataControl) {
                    focusedItem = a._dataControl.getFocusedItem();
                    if (focusedItem) {
                        $focusedItem = focusedItem.$item;
                        if ($focusedItem) {
                            focusedIndex = $focusedItem.attr('data');
                            text = 'Total ' + '<span>' + (parseInt(focusedIndex) + 1) + ' / ' + total + '</span>';
                        }
                    }
                }
            }
            if (onOff) {
                if (!$contentsIndex) {
                    $contentsIndex = $('<div>', {
                        'class': 'contentsIndex'
                    }).html(text);
                    $layer.append($contentsIndex);
                    a._$contentsIndex = $contentsIndex;
                }
                else {
                    $contentsIndex.html(text);
                }
            }
            else {
                if ($contentsIndex) {
                    $contentsIndex.remove();
                    a._$contentsIndex = null;
                }
            }
        };
        var fnLoadingMsgOnOff = function (a, onOff, text) {
            var $loading = a._$self.find('.nonblockLoading');
            if (onOff) {
                if (!$loading.length) {
                    $loading = $('<div>', {
                        'class': 'nonblockLoading',
                        'text': text
                    });
                    a._$self.append($loading);
                }
            }
            else {
                if ($loading.length) {
                    $loading.remove();
                }
            }
        };
        /**************************************************
         * Event Handlers
         **************************************************/
        var onMediaServerSelected = function (a, param) {
            var item = param.item;
            var $item = param.$item;
            a._currentServer = item.getPath();
            a._currentServerId = item.getId();
            a.$menuArea().find('.prism-button').removeClass('selected');
            $item.addClass('selected');
            mViewType = item.getViewType();
            mSelectedServer = item;
            fnNotFoundMsgOnOff(a, true);
            fnContentsIndexOnOff(a, false);
            if (a.onMediaServerSelected) {
                a.onMediaServerSelected({
                    'item': item,
                    'server': mSelectedServer
                });
            }
            a.setTitleText(a._title);
            delete a._currentPath;
        };
        var onMediaItemSelected = function (a, param) {
            var item = param.item;
            var itemKey = param.itemKey;
            var fullPath = item.getFullPath();
            var info = fullPath.replace(mSelectedServer.getPath(), '');
            var reduced = false;
            var TLocationType = __MODEL__.IMediaServer.TLocationType;
            var locationType = mSelectedServer.getLocationType();
            a._currentPath = fullPath;
            if (a.onMediaItemSelected) {
                a.onMediaItemSelected({
                    'item': item,
                    'itemKey': itemKey,
                    'server': mSelectedServer
                });
            }
            if (fullPath.indexOf('//') > -1) {
                return;
            }
            var fnReducePath = function (path) {
                var index = path.indexOf('/');
                if (path.length > 40 && index !== -1) {
                    path = path.replace(path.substr(0, index + 1), '');
                    reduced = true;
                    fnReducePath(path);
                }
                return path;
            };
            if (item.getFileType() === __MODEL__.IMediaItem.TFileType.EFolder && !(locationType === TLocationType.ENetwork)) {
                info = fnReducePath(info);
                info = reduced ? '... /' + info : info;
                a.setTitleText(a._title + ' <span class="info">' + info + '</span>');
            }
        };
        var CMediaPage = (function (_super) {
            __extends(CMediaPage, _super);
            function CMediaPage($el) {
                _super.call(this, $el);
            }
            CMediaPage.prototype.init = function ($el) {
                var a = this;
                fnSetMultiSelection(false);
                _super.prototype.init.call(a, $el);
                a.setId('media');
                a.setOptionMenuProvider(function (activeFocusInfo) {
                    return fnOptionMenuProvider(a, activeFocusInfo);
                });
            };
            CMediaPage.prototype.destroy = function () {
                var a = this;
                if (a._contentDataProvider) {
                    a._contentDataProvider.destroy();
                    delete a._contentDataProvider;
                }
                if (a._localServerDataProvider) {
                    a._localServerDataProvider.destroy();
                    delete a._localServerDataProvider;
                }
                if (a._networkServerDataProvider) {
                    a._networkServerDataProvider.destroy();
                    delete a._networkServerDataProvider;
                }
                if (a._userServerDataProvider) {
                    delete a._userServerDataProvider;
                }
                if (a._localServerListControl) {
                    delete a._localServerListControl;
                }
                if (a._networkServerListControl) {
                    delete a._networkServerListControl;
                }
                fnSetMultiSelection(false);
                _super.prototype.destroy.call(a);
            };
            CMediaPage.prototype.hasFile = function (aPath) {
                return false;
            };
            CMediaPage.prototype.setUserServerDataProvider = function (dataProvider) {
                this._userServerDataProvider = dataProvider;
            };
            CMediaPage.prototype.setLocalServerDataProvider = function (dataProvider) {
                this._localServerDataProvider = dataProvider;
            };
            CMediaPage.prototype.setNetworkServerDataProvider = function (dataProvider) {
                this._networkServerDataProvider = dataProvider;
            };
            CMediaPage.prototype.setInitServerId = function (initServerId) {
                this._initServerId = initServerId;
            };
            CMediaPage.prototype.draw = function (param) {
                var _this = this;
                var a = this;
                if (!a.$layer()) {
                    a.createLayer({
                        fnAfterCreate: function () {
                            var result = false;
                            var childControl = [];
                            fnNotFoundMsgOnOff(a, true);
                            fnContentsIndexOnOff(a, false);
                            a.setTitleText(a._title);
                            a.$contentArea();
                            a.$menuArea();
                            fnCreateViewType(a);
                            var info = fnCreateMenu(a);
                            childControl.push(info.menu);
                            childControl.push(fnCreateContent(a));
                            a.setChildControl(childControl);
                            _super.prototype.draw.call(a);
                            info.cb();
                        }
                    });
                }
                else {
                    _super.prototype.draw.call(a, param);
                }
            };
            CMediaPage.prototype.getType = function () {
                return this._type;
            };
            CMediaPage.prototype.getItemDrawer = function () {
                return this._itemDrawer;
            };
            CMediaPage.prototype.addMediaItems = function (mediaItemList) {
                var a = this;
                var focusedChildIndex = 0;
                fnNotFoundMsgOnOff(a, false);
                var createViewDlg = function () {
                    var viewDlg = prism.createViewDlg(a);
                    viewDlg.setTitleHtml('Group');
                    viewDlg.setDataProvider(a._contentDataProvider);
                    viewDlg.setDataDrawer(a.getItemDrawer());
                    viewDlg.onDataItemSelected = function (param) {
                        onMediaItemSelected(a, param);
                    };
                    viewDlg.setOptionMenuTarget(a);
                    viewDlg.open({
                        transition: {
                            prevLayer: 'fadeOut',
                            newLayer: 'fadeIn'
                        },
                        onClosed: function () {
                            if (a._contentDataProvider) {
                                a._contentDataProvider.removeEventHandler(a);
                                a._contentDataProvider.destroy();
                                delete a._contentDataProvider;
                            }
                            delete a._viewDlg;
                        }
                    });
                    return viewDlg;
                };
                var removeLateResponse = function (aMediaItemList) {
                    var mediaItemFullPath, currentPath;
                    var mediaItemList = aMediaItemList;
                    for (var i = 0; i < mediaItemList.length; i++) {
                        mediaItemFullPath = mediaItemList[i].getFullPath();
                        if (!(mediaItemList[i] instanceof __MODEL__.CUpperFolderItem) && !(mediaItemList[i] instanceof __MODEL__.CNetworkMediaItem)) {
                            currentPath = mediaItemFullPath.substr(0, mediaItemFullPath.lastIndexOf('/'));
                            if (a._currentPath && currentPath !== a._currentPath) {
                                mediaItemList.splice(i, 1);
                                i--;
                                continue;
                            }
                            if (!a._currentPath && a._currentServer && mediaItemFullPath.indexOf(a._currentServer) !== 0) {
                                mediaItemList.splice(i, 1);
                                i--;
                                continue;
                            }
                        }
                    }
                    return mediaItemList;
                };
                if (a._contentDataProvider.getCount() === 0) {
                    mediaItemList = removeLateResponse(mediaItemList);
                    a._contentDataProvider = new __CONTROLS__.CDataProvider(mediaItemList);
                    if (mSelectedServer.getViewType() === __MODEL__.IMediaServer.TViewType.EOneDepthView && a._contentView.hasFocus()) {
                        a._viewDlg = createViewDlg();
                        var dataControl = a._viewDlg.getDataControl();
                        dataControl.onItemSelected = function (param) {
                            var $item = param.$item;
                            var item = param.item;
                            var index = -1;
                            if (mMultiSelection) {
                                if ($item.hasClass('checked')) {
                                    $item.removeClass('checked');
                                    $item.addClass('unchecked');
                                    index = fnGetIndexInCheckedItems(item);
                                    if (index > -1) {
                                        mCheckedItems.splice(index, 1);
                                    }
                                }
                                else {
                                    $item.removeClass('unchecked');
                                    $item.addClass('checked');
                                    mCheckedItems.push(item);
                                }
                            }
                            else {
                                onMediaItemSelected(a, param);
                            }
                        };
                    }
                    else {
                        a._dataControl.setDataProvider(a._contentDataProvider);
                        a._mediaViewContainer.setTop(0, true);
                    }
                }
                else {
                    mediaItemList = removeLateResponse(mediaItemList);
                    a._contentDataProvider.appendItems(mediaItemList);
                }
                fnContentsIndexOnOff(a, true);
            };
            CMediaPage.prototype.removeMediaItem = function (items) {
                var a = this;
                var path, fullPath;
                var i, length, item;
                items = items instanceof Array ? items : [items];
                length = items.length;
                for (i = 0; i < length; i += 1) {
                    item = items[i];
                    if (item.getFullPath) {
                        path = item.getPath ? item.getPath() : item.getFullPath();
                        fullPath = item.getFullPath();
                        if (fullPath === path) {
                            var index = a._contentDataProvider.getItemIndex(item, function (currentItem) {
                                if (currentItem.getFullPath() === item.getFullPath()) {
                                    return true;
                                }
                                return false;
                            });
                            if (index > -1) {
                                a._contentDataProvider.removeItem(index);
                            }
                        }
                        if (fullPath !== path || a._contentDataProvider.getCount() === 0) {
                            a.doNavi('l');
                            var focusedChild = a._menuView.getFocusedChild();
                            if (focusedChild) {
                                focusedChild.doKeyOk();
                            }
                        }
                    }
                    else {
                        a._userServerListControl.doKeyOk();
                    }
                }
                fnContentsIndexOnOff(a, true);
            };
            CMediaPage.prototype.removeAllMediaItems = function () {
                var a = this;
            };
            CMediaPage.prototype.clearContent = function () {
                var a = this;
                a._contentDataProvider = new __CONTROLS__.CDataProvider([]);
                a._dataControl.setDataProvider(a._contentDataProvider);
                // a._contentView.draw();
            };
            CMediaPage.prototype.isContentFocus = function () {
                var a = this;
                return a._contentView.hasFocus();
            };
            CMediaPage.prototype.doKeyBack = function () {
                var a = this;
                var handled = false;
                var firstItem = null;
                var i, controls = a._controls.slice(0);
                if (a.getFocusedChildIndex() === 1) {
                    if (a._contentDataProvider) {
                        firstItem = a._contentDataProvider.getItem(0);
                    }
                    if (firstItem && firstItem instanceof __MODEL__.CUpperFolderItem) {
                        onMediaItemSelected(a, {
                            item: firstItem
                        });
                    }
                    else {
                        a.doNavi('l');
                    }
                    handled = true;
                }
                else {
                    for (i = 0; i < controls.length; i += 1) {
                        var c = controls[i];
                        if (c.hasFocus() && c['close']) {
                            c['close']();
                            handled = true;
                        }
                    }
                    if (!handled) {
                        a.onRequestChangePage({
                            target: 'CPgLiveController'
                        });
                    }
                    handled = true;
                }
                a.onContextMenuClose();
                return handled;
            };
            CMediaPage.prototype.doKeyOption = function () {
                var a = this;
                a.reloadContextMenuItems();
                var dataControl = a.getDataControl();
                var dataProvider = dataControl._dataProvider;
                var focusedItem = dataControl.getFocusedItem();
                var item = focusedItem ? focusedItem.item : null;
                var currentIndex = dataControl.getCurrentIndex();
                var consumed = false;
                if (item) {
                    if (!(item instanceof __MODEL__.cMediaPlaybackPlaylistItem) && (item instanceof __MODEL__.cMediaPlaybackHistoryItem && a.getFocusedChildIndex() !== 0)) {
                        dataProvider.removeItem(currentIndex);
                        if (dataProvider.getCount() === 0) {
                            a.doNavi('l');
                        }
                        a.onRequestRemoveRecentItem({
                            item: item
                        });
                        if (dataProvider.getCount() === 0) {
                            a._userServerListControl.doKeyOk();
                        }
                        this.showNotificationMsg(hx.l('LOC_FILE_REMOVED_FROM_RECENT_ID'));
                        consumed = true;
                    }
                }
                if (!consumed) {
                    consumed = _super.prototype.doKeyOption.call(a);
                }
                return consumed;
            };
            CMediaPage.prototype.addServer = function (server) {
                var a = this;
                var TLocationType = __MODEL__.IMediaServer.TLocationType;
                if (server.getLocationType() === TLocationType.ELocal) {
                    a._localServerDataProvider.appendItem(server);
                }
                else {
                    a._networkServerDataProvider.appendItem(server);
                }
            };
            CMediaPage.prototype.removeServer = function (server) {
                var a = this;
                var index = 0;
                var TLocationType = __MODEL__.IMediaServer.TLocationType;
                var fnRemoveServer = function (control, item) {
                    if (!control) {
                        return true;
                    }
                    var provider = control._dataProvider;
                    var selectedIndex = control.getSelectedIndex();
                    var index = provider.getItemIndex(item, function (currentItem) {
                        if (currentItem.getId() === item.getId()) {
                            return true;
                        }
                        return false;
                    });
                    if (index > -1) {
                        provider.removeItem(index);
                        setTimeout(function () {
                            if (mSelectedServer.getId() === server.getId()) {
                                if (a._contentView.hasFocus()) {
                                    a.setFocusedChld(a._menuView);
                                }
                                if (control.hasFocus() && provider.getCount() > 0) {
                                    a._menuView.setFocusedChld(control);
                                    control.setActiveFocus(true);
                                    control.doKeyOk();
                                }
                                else {
                                    a._menuView.setFocusedChld(a._localServerListControl);
                                    a._localServerListControl.setActiveFocus(true);
                                    a._localServerListControl.doKeyOk();
                                }
                            }
                        }, 1);
                    }
                };
                if (server.getLocationType() === TLocationType.ELocal) {
                    fnRemoveServer(a._localServerListControl, server);
                    a._menuView.draw();
                }
                else {
                    fnRemoveServer(a._networkServerListControl, server);
                }
            };
            CMediaPage.prototype.showNotificationMsg = function (text) {
                var a = this;
                if (a._notificationDlg) {
                    hx.msg.close('progress', a._notificationDlg);
                }
                var notificationDlg = hx.msg('response', {
                    text: text,
                    auto_close: true,
                    close_time: 3000
                });
                a._notificationDlg = notificationDlg;
            };
            CMediaPage.prototype.showLoadingMsg = function (text) {
                var a = this;
                fnLoadingMsgOnOff(a, true, text);
            };
            CMediaPage.prototype.hideLoadingMsg = function () {
                var a = this;
                fnLoadingMsgOnOff(a, false);
            };
            CMediaPage.prototype.showErrorMsg = function (text) {
                var a = this;
                var buttonDlg = prism.createButtonDlg(a);
                buttonDlg.setTextHtml(text);
                buttonDlg.setButtonInfoList([
                    {
                        text: tr('LOC_OK_ID'),
                        fn: function () {
                        }
                    }
                ]);
                buttonDlg.open({
                    dimm: true
                });
            };
            CMediaPage.prototype.setCurrentOperation = function (operation) {
                this._currentOperation = operation;
            };
            CMediaPage.prototype.onContextMenuClose = function (param) {
                var a = this;
                var dataControl = a.getDataControl();
                var data = dataControl._drawnItems ? dataControl._drawnItems.getItemsByArray() : null;
                if (data) {
                    var length = data.length;
                    var i, drawnItem;
                    for (i = 0; i < length; i += 1) {
                        drawnItem = data[i];
                        if (mFileOperationMode) {
                            a.reloadContextMenuItems(dataControl);
                            mFileOperationMode = false;
                        }
                        drawnItem.removeClass('unchecked checked');
                    }
                }
            };
            CMediaPage.prototype.getContentDataProvider = function () {
                return this._contentDataProvider;
            };
            CMediaPage.prototype.getDataControl = function () {
                var a = this;
                return a._viewDlg ? a._viewDlg.getDataControl() : a._dataControl;
            };
            CMediaPage.prototype.getNotFoundText = function () {
                // should set this text on the page for use.
                return {
                    title: '',
                    content: ''
                };
            };
            return CMediaPage;
        })(__PRISM_CPAGE__.CPage);
        mPrism.CMediaPage = CMediaPage;
        var CMusicPage = (function (_super) {
            __extends(CMusicPage, _super);
            function CMusicPage($el) {
                _super.call(this, $el);
                var a = this;
                a._type = __MODEL__.IMediaItem.ContentTypeCssList[__MODEL__.IMediaItem.TContentType.EMusic];
                a._itemDrawer = function (param) {
                    var $item = param.$item;
                    var item = param.item;
                    var thumbnailUrl = item.getThumbnailUrl();
                    var title = item.getTitle();
                    var duration = item.getDuration();
                    var artistName = item.getArtistName();
                    var albumName = item.getAlbumName();
                    var storageCssClass = mSelectedServer.getStorageCssClass();
                    var fileType = item.getFileType();
                    var hasFile = (fileType === __MODEL__.IMediaItem.TFileType.EFolder && a.hasFile(item.getFullPath()));
                    var date = item.getDate();
                    $item.append($('<div>', {
                        'class': 'thumbnail'
                    }));
                    $item.append($('<div>', {
                        'class': 'title'
                    }));
                    $item.append($('<div>', {
                        'class': 'secondary'
                    }));
                    var $thumbnail = $item.children('.thumbnail');
                    if (fileType === __MODEL__.IMediaItem.TFileType.EFolder || fileType === __MODEL__.IMediaItem.TFileType.EGroup) {
                        $item.addClass('group');
                    }
                    else {
                        if (thumbnailUrl) {
                            $thumbnail.css('background', 'url("' + thumbnailUrl + '") no-repeat center/100% 100%');
                        }
                    }
                    var $title = $item.children('.title');
                    if (fileType === __MODEL__.IMediaItem.TFileType.EFile) {
                        var titleHtml = "<p>" + title + "</p>";
                        $title.html(titleHtml);
                        var $secondary = $item.children('.secondary');
                        var dispSecondary = hx.util.format('{0} / {1}', new Date(1970, 1, 1, 0, 0, duration).format('HH:MM:ss'), artistName);
                        $secondary.html(dispSecondary);
                    }
                    else {
                        if (storageCssClass === 'artist') {
                            title = artistName;
                        }
                        else if (storageCssClass === 'album') {
                            title = albumName;
                        }
                    }
                    var titleHtml = "<p>" + title + "</p>";
                    $title.html(titleHtml);
                    if (fileType === __MODEL__.IMediaItem.TFileType.EFolder || fileType === __MODEL__.IMediaItem.TFileType.EGroup) {
                        $item.addClass('folder');
                        if (hasFile) {
                            $item.addClass('plus');
                        }
                        if (item instanceof __MODEL__.CUpperFolderItem) {
                            $item.addClass('upper');
                        }
                        $thumbnail.css('background', 'url("") no-repeat center/100% 100%');
                    }
                    $item.addClass('music');
                    if (mFileOperationMode) {
                        if (fnGetIndexInCheckedItems(item) > -1) {
                            $item.removeClass('unchecked');
                            $item.addClass('checked');
                        }
                        else {
                            $item.removeClass('checked');
                            $item.addClass('unchecked');
                        }
                    }
                };
            }
            CMusicPage.prototype.init = function ($el) {
                var a = this;
                a._title = tr('LOC_STR_MUSIC_01_ID');
                CMediaPage.prototype.init.call(a, $el);
            };
            CMusicPage.prototype.getNotFoundText = function () {
                var a = this;
                return {
                    title: 'No music file available',
                    content: 'Play the music files saved in your PC or USB memory stick right on your TV screen.<br>Move the music files to a shared folder in your PC or connect your USB memory stick to the set-top box.'
                };
            };
            return CMusicPage;
        })(CMediaPage);
        mPrism.CMusicPage = CMusicPage;
        var CVideoPage = (function (_super) {
            __extends(CVideoPage, _super);
            function CVideoPage($el) {
                _super.call(this, $el);
                var a = this;
                a._type = __MODEL__.IMediaItem.ContentTypeCssList[__MODEL__.IMediaItem.TContentType.EVideo];
                a._itemDrawer = function (param) {
                    var $item = param.$item;
                    var item = param.item;
                    var thumbnailUrl = item.getThumbnailUrl();
                    var title = item.getTitle();
                    var duration = item.getDuration();
                    var artistName = item.getArtistName();
                    var fileType = item.getFileType();
                    var contentType = item.getContentType();
                    var hasFile = (fileType === __MODEL__.IMediaItem.TFileType.EFolder && a.hasFile(item.getFullPath()));
                    var date = item.getDate();
                    $item.append($('<div>', {
                        'class': 'thumbnail'
                    }));
                    $item.append($('<div>', {
                        'class': 'title'
                    }));
                    $item.append($('<div>', {
                        'class': 'secondary'
                    }));
                    var $thumbnail = $item.children('.thumbnail');
                    if (fileType !== __MODEL__.IMediaItem.TFileType.EFolder) {
                        if (thumbnailUrl) {
                            $thumbnail.css('background', 'url("' + thumbnailUrl + '") no-repeat center/100% 100%');
                        }
                    }
                    var $title = $item.children('.title');
                    var titleHtml = "<p>" + title + "</p>";
                    $title.html(titleHtml);
                    if (fileType === __MODEL__.IMediaItem.TFileType.EFile) {
                        var $secondary = $item.children('.secondary');
                        var dispSecondary = hx.util.format('{0} / {1}', new Date(1970, 1, 1, 0, 0, duration).format('HH:MM:ss'), date);
                        $secondary.html(dispSecondary);
                    }
                    if (fileType === __MODEL__.IMediaItem.TFileType.EFolder) {
                        $item.addClass('folder');
                        if (hasFile) {
                            $item.addClass('plus');
                        }
                        if (item instanceof __MODEL__.CUpperFolderItem) {
                            $item.addClass('upper');
                        }
                        $thumbnail.css('background', 'url("") no-repeat center/100% 100%');
                    }
                    $item.addClass('video');
                    if (mFileOperationMode) {
                        if (fnGetIndexInCheckedItems(item) > -1) {
                            $item.removeClass('unchecked');
                            $item.addClass('checked');
                        }
                        else {
                            $item.removeClass('checked');
                            $item.addClass('unchecked');
                        }
                    }
                };
            }
            CVideoPage.prototype.init = function ($el) {
                var a = this;
                a._title = tr('LOC_STR_VIDEO_01_ID');
                CMediaPage.prototype.init.call(a, $el);
            };
            CVideoPage.prototype.getNotFoundText = function () {
                var a = this;
                return {
                    title: 'No video file available',
                    content: 'Play the video files saved in your PC or USB memory stick right on your TV screen.<br>Move the video files to a shared folder in your PC or connect your USB memory stick to the set-top box.'
                };
            };
            return CVideoPage;
        })(CMediaPage);
        mPrism.CVideoPage = CVideoPage;
        var CPhotoPage = (function (_super) {
            __extends(CPhotoPage, _super);
            function CPhotoPage($el) {
                _super.call(this, $el);
                var a = this;
                a._type = __MODEL__.IMediaItem.ContentTypeCssList[__MODEL__.IMediaItem.TContentType.EPhoto];
                a._itemDrawer = function (param) {
                    var $item = param.$item;
                    var item = param.item;
                    var thumbnailUrl = item.getThumbnailUrl();
                    var title = item.getTitle();
                    var fileType = item.getFileType();
                    var hasFile = (fileType === __MODEL__.IMediaItem.TFileType.EFolder && a.hasFile(item.getFullPath()));
                    $item.append($('<div>', {
                        'class': 'thumbnail'
                    }));
                    $item.append($('<div>', {
                        'class': 'title'
                    }));
                    var $thumbnail = $item.children('.thumbnail');
                    if (fileType !== __MODEL__.IMediaItem.TFileType.EFolder) {
                        if (fileType === __MODEL__.IMediaItem.TFileType.EGroup) {
                            $item.addClass('group');
                        }
                        else {
                            if (thumbnailUrl) {
                                $thumbnail.css('background', 'url("' + thumbnailUrl || '' + '") no-repeat center/100% 100%');
                            }
                        }
                    }
                    var $title = $item.children('.title');
                    var titleHtml = "<p>" + title + "</p>";
                    $title.html(titleHtml);
                    if (fileType === __MODEL__.IMediaItem.TFileType.EFolder || fileType === __MODEL__.IMediaItem.TFileType.EGroup) {
                        $item.addClass('folder');
                        if (hasFile) {
                            $item.addClass('plus');
                        }
                        if (item instanceof __MODEL__.CUpperFolderItem) {
                            $item.addClass('upper');
                        }
                        $thumbnail.css('background', 'url("") no-repeat center/100% 100%');
                    }
                    $item.addClass('photo');
                    if (mFileOperationMode) {
                        if (fnGetIndexInCheckedItems(item) > -1) {
                            $item.removeClass('unchecked');
                            $item.addClass('checked');
                        }
                        else {
                            $item.removeClass('checked');
                            $item.addClass('unchecked');
                        }
                    }
                };
            }
            CPhotoPage.prototype.init = function ($el) {
                var a = this;
                a._title = tr('LOC_STR_PHOTO_01_ID');
                CMediaPage.prototype.init.call(a, $el);
            };
            CPhotoPage.prototype.getNotFoundText = function () {
                var a = this;
                return {
                    title: 'No photo file available',
                    content: 'Play the photo files saved in your PC or USB memory stick right on your TV screen.<br>Move the photo files to a shared folder in your PC or connect your USB memory stick to the set-top box.'
                };
            };
            return CPhotoPage;
        })(CMediaPage);
        mPrism.CPhotoPage = CPhotoPage;
        if (!window.prism) {
            window.prism = {};
        }
        prism.createMediaPage = function ($parent, contentType) {
            var page;
            var $page = $('<div>', {
                'class': 'prism-page'
            });
            $parent.append($page);
            switch (contentType) {
                case __MODEL__.IMediaItem.TContentType.EMusic:
                    page = new CMusicPage($page);
                    break;
                case __MODEL__.IMediaItem.TContentType.EVideo:
                    page = new CVideoPage($page);
                    break;
                case __MODEL__.IMediaItem.TContentType.EPhoto:
                    page = new CPhotoPage($page);
                    break;
            }
            return page;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
