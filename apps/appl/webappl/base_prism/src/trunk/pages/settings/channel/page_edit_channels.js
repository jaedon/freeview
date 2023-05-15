var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    // file name : pages/settings/channel/edit_channels.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var CPgEditChannels = (function (_super) {
        __extends(CPgEditChannels, _super);
        function CPgEditChannels() {
            _super.call(this);
            this._contextListMap = [['LOC_DELETE_ID', 'LOC_LOCK_ID', 'LOC_RENAME_ID', 'LOC_CHANGE_GROUP_ID', 'LOC_SELECT_ALL_ID', 'LOC_DESELECT_ALL_ID'], ['LOC_TV_ID', 'LOC_RADIO_ID']];
            this.aniConfig = {
                fadeIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
                    }
                },
                channelMenu: {
                    fadeIn: {
                        start: {
                            'opacity': 0,
                            'top': '20px'
                        },
                        end: {
                            'opacity': 1,
                            'top': 0
                        }
                    },
                    fadeOut: {
                        start: {
                            'opacity': 1,
                            'top': 0
                        },
                        end: {
                            'opacity': 0,
                            'top': '20px'
                        }
                    }
                }
            };
            this.SVC_SECTION = {
                SECTION_DEFAULT: 1,
                SECTION_LCN: 2,
                SECTION_NONE_LCN: 16
            };
            this._strCasID = ['CAS_FTA', 'CAS_NAGRA', 'CAS_IRDETO', 'CAS_NDS', 'CAS_VIACCESS', 'CAS_CONAX', 'CAS_MEDIAGUARD', 'CAS_CRYPTOWORKS'];
            this._circularTVArray = [];
            this._circularRADIOArray = [];
            this._circularTVArrayBackup = [];
            this._circularRADIOArrayBackup = [];
            this._curCircularArray = [];
            this._availableGroup = false;
            this._currentList = [];
            this._currentMoveItemList = [];
            this._moveBackupList = [];
            this._moveBackupFocus = 0;
            this._svcList = [];
            this._allNum = 0;
            this._selNum = 0;
            this._currentState = 'none';
            this._currentMovePoint = 100;
            this._isChange = false;
            this._recentDataList = [];
            this._isChangePageInSetting = false;
        }
        CPgEditChannels.prototype.create = function () {
            var _this = this;
            _super.prototype.create.call(this);
            var ccid;
            var i, currentChIdx = 0;
            var chTypeStr = '';
            this._circularTVArray = [];
            this._circularRADIOArray = [];
            this._circularTVArrayBackup = [];
            this._circularRADIOArrayBackup = [];
            this._curCircularArray = [];
            this._availableGroup = false;
            this._currentList = [];
            this._currentMoveItemList = [];
            this._moveBackupList = [];
            this._moveBackupFocus = 0;
            this._svcList = [];
            this._allNum = 0;
            this._selNum = 0;
            this._currentState = 'none';
            this._currentMovePoint = 100;
            this._isChange = false;
            this._recentDataList = [];
            this._isChangePageInSetting = false;
            var set_title = $('#set .title');
            if (set_title[0]) {
                set_title[0].children[0].innerHTML = hx.l('LOC_EDIT_CHANNEL_LIST_ID');
            }
            set_title.ani({
                style: this.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            this.progressMsg = hx.msg('progress', {
                text: hx.l('LOC_LOADING_DOT_ID'),
                _use_backkey: false,
                auto_close: false
            });
            $.extend(this, {
                _$chItem: $('<article>', {
                    'class': 'chItem'
                }),
                _$opt: $('<section>', {
                    'id': 'opt'
                }),
                _$optTit: $('<p>', {
                    'class': 'tit',
                    'html': hx.l('LOC_EDIT_ID')
                }),
                _optList: CPgEditChannels.optListMenu(this._contextListMap[0], 'menu'),
                _optGroupList: CPgEditChannels.optListMenu(this._contextListMap[1], 'menu')
            });
            hx.al.compose(this).append(this._optList);
            //animation
            var $target = $('#DIA .vaTop');
            hx.il.ani.action({
                type: 'in',
                targets: $target,
                cb: function () {
                    $('#set').append(_this._$chItem);
                    $('#set').after(_this._$opt.append(_this._$optTit).append(_this._optList.$));
                    _this._recentDataList.length = 0;
                    var recentHistory = hx.svc.WEB_STORAGE.getRecentItems();
                    if (recentHistory) {
                        if (recentHistory.recentData) {
                            _this._recentDataList = recentHistory.recentData.split(/\s*,\s*/);
                        }
                    }
                    _this._isChangePageInSetting = false;
                    //this._chList.on();
                    var chObj = hx.svc.CH_LIST.getCurrentChannel();
                    _this._currentChObj = chObj;
                    if (chObj) {
                        if (chObj.result) {
                            if (chObj.channel) {
                                ccid = chObj.channel.ccid;
                                if (chObj.group) {
                                    _this._chType = chObj.group;
                                    if (_this._chType === 'TV' || _this._chType === 'HDTV') {
                                        _this._chType = 'TV';
                                    }
                                    else if (_this._chType === 'RADIO') {
                                        _this._chType = 'RADIO';
                                    }
                                    else {
                                        _this._chType = (chObj.channel.channelType === 1) ? 'RADIO' : 'TV';
                                    }
                                }
                                else {
                                    _this._chType = 'TV';
                                }
                            }
                            else {
                                ccid = undefined;
                                _this._chType = 'TV';
                            }
                        }
                        else {
                            ccid = undefined;
                            _this._chType = 'TV';
                        }
                    }
                    else {
                        ccid = undefined;
                        _this._chType = 'TV';
                    }
                    _this.getChlist();
                    //getChlist with group
                    _this._hgroup = CPgEditChannels.hgroup(_this._selNum, _this._allNum);
                    if (_this._chType === 'TV') {
                        chTypeStr = hx.l('LOC_TV_ID');
                    }
                    else if (_this._chType === 'RADIO') {
                        chTypeStr = hx.l('LOC_RADIO_ID');
                    }
                    _this._hgroup.$.find('h1').html(chTypeStr);
                    for (i = 0; i < _this._curCircularArray._array.length; i += 1) {
                        if (_this._curCircularArray._array[i].ccid === ccid) {
                            currentChIdx = i;
                            break;
                        }
                    }
                    for (i = 0; i < currentChIdx; i += 1) {
                        _this._curCircularArray.inc();
                        while (_this._curCircularArray.curItem().removed === true) {
                            _this._curCircularArray.inc();
                        }
                    }
                    _this._currentList = _this.makeCurrentShowList(_this._getCircularItem(8, 0));
                    _this._chList = CPgEditChannels.chListMenu(_this._currentList);
                    _this._$chItem.append(_this._hgroup.$).append(_this._chList.$);
                    hx.msg.close('progress', _this.progressMsg);
                    $('#set .chItem').ani({
                        style: _this.aniConfig.channelMenu.fadeIn,
                        trans: {
                            dur: 200,
                            delay: 400
                        },
                        cb: function () {
                            _this.append(_this._chList);
                            var dsrvRecordings = hx.svc.RECORDINGS;
                            dsrvRecordings.addEventCb('RecordingItemAdded', _this, function (param) {
                                if (_this.progressMsg) {
                                    hx.msg.close('progress', _this.progressMsg);
                                }
                                if (_this.confirm) {
                                    hx.msg.close('confirm', _this.confirm);
                                }
                                hx.svc.SETTING_UTIL.setFrontPanelString({
                                    text: ''
                                });
                                _this.sendEventToEntity({
                                    alEvent: 'alChangePage',
                                    target: 'CPgLiveController',
                                    me: _this
                                });
                            });
                            _this._chList.on();
                        }
                    });
                }
            });
        };
        CPgEditChannels.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            hx.svc.RECORDINGS.clearEventCb(this, 'RecordingItemAdded');
            hx.svc.CH_LIST.outFromEditChannels();
            this._$chItem.remove();
            this._$opt.remove();
            this._optList = undefined;
            this._optGroupList = undefined;
            this._chList = undefined;
            this._currentList = undefined;
            this._svcList = undefined;
            this._curCircularArray = undefined;
            this._circularRADIOArray = undefined;
            this._circularRADIOArrayBackup = undefined;
            this._circularTVArray = undefined;
            this._circularTVArrayBackup = undefined;
            this._allNum = undefined;
            this._selNum = undefined;
            this._tvList = undefined;
            this._radioList = undefined;
            this._currentMovePoint = undefined;
            this._currentState = undefined;
            this._isChange = undefined;
            this._recentDataList = undefined;
            this._moveBackupList = undefined;
            this._currentMoveItemList = undefined;
            this._isAvBlock = undefined;
            this._currentChObj = undefined;
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
        };
        CPgEditChannels.prototype._changeToAvailCh = function (group) {
            // tune to available channel
            if (this._isAvBlock) {
                hx.svc.AV_CTRL.availableChannel({
                    group: group
                });
            }
        };
        CPgEditChannels.prototype._goExit = function (aTargetPage) {
            var _this = this;
            var nextTargetPage;
            if (aTargetPage === 'setting') {
                this._isChangePageInSetting = true;
                nextTargetPage = CPgEditChannels.targetPage[0];
                hx.al.decompose(this);
                $('#set .chItem').ani({
                    style: this.aniConfig.channelMenu.fadeOut,
                    trans: {
                        dur: 200,
                        delay: 100
                    },
                    cb: function () {
                        _this.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: nextTargetPage,
                            me: _this
                        });
                    }
                });
            }
            else if (aTargetPage === 'live') {
                hx.svc.SETTING_UTIL.setFrontPanelString({
                    text: ''
                });
                nextTargetPage = CPgEditChannels.targetPage[1];
                this.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: nextTargetPage,
                    me: this
                });
            }
        };
        CPgEditChannels.prototype._on_alClicked = function (aParam) {
            var data = aParam.alData, target = data.me, i, currentFocus, bConsumed = false;
            var num = 0, smallestLcn, smallestLcnIdx, roopIdx, roopCount;
            switch (target) {
                case this._chList:
                    bConsumed = true;
                    if (this._allNum === 0) {
                        return;
                    }
                    if (this._currentState === 'none') {
                        this._select_unselect_Item(data.index);
                    }
                    else if (this._currentState === 'mov') {
                        this._isChange = true;
                        if (hx.config.useFavouriteCollection) {
                            hx.svc.CH_LIST.moveChListItem({
                                favtype: this._chType,
                                ccid: this._currentMoveItemList[0].ccid,
                                insertTargetccid: this._curCircularArray.getNextItem().ccid
                            });
                        }
                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._currentMoveItemList[0]);
                        //insert
                        currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;
                        smallestLcn = this._curCircularArray._array[0].majorChannel;
                        //find smallestLcnIdx
                        smallestLcnIdx = 0;
                        for (i = 1; i < this._curCircularArray.length(); i += 1) {
                            if (smallestLcn > this._curCircularArray._array[i].majorChannel) {
                                smallestLcn = this._curCircularArray._array[i].majorChannel;
                                smallestLcnIdx = i;
                            }
                        }
                        var tempSmallestLcnIdx = smallestLcnIdx;
                        //if you move smallestLcnItem, find next smallestLcnItem
                        if (this._curCircularArray._array[tempSmallestLcnIdx].ccid === this._currentMoveItemList[0].ccid) {
                            smallestLcn = this._curCircularArray._array[0].majorChannel;
                            smallestLcnIdx = 0;
                            for (i = 1; i < this._curCircularArray.length(); i += 1) {
                                if (smallestLcn > this._curCircularArray._array[i].majorChannel) {
                                    if (tempSmallestLcnIdx === i) {
                                        continue;
                                    }
                                    smallestLcn = this._curCircularArray._array[i].majorChannel;
                                    smallestLcnIdx = i;
                                }
                            }
                        }
                        //if insertTargetIdx is just before smallestLcnIdx, set smallestLcnIdx with moveItem
                        if (this._curCircularArray.getNextItem().ccid === this._curCircularArray._array[smallestLcnIdx].ccid) {
                            for (i = 0; i < this._curCircularArray.length(); i += 1) {
                                if (this._currentMoveItemList[0].ccid === this._curCircularArray._array[i].ccid) {
                                    smallestLcnIdx = i;
                                    break;
                                }
                            }
                        }
                        if (ENV.listModel.IR4000HD === ENV.model) {
                            if (this._chType === 'TV') {
                                num = 1;
                            }
                            else if (this._chType === 'RADIO') {
                                num = 5000;
                            }
                        }
                        else {
                            if (hx.config.useLcnUpdate) {
                                if (this._chType === 'TV') {
                                    num = (this._lastMajorChannelOfTvLcnList + 1);
                                }
                                else if (this._chType === 'RADIO') {
                                    num = (this._lastMajorChannelOfRadioLcnList + 1);
                                }
                            }
                            else {
                                if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                                    num = 100;
                                }
                            }
                        }
                        roopIdx = smallestLcnIdx;
                        roopCount = this._curCircularArray.length();
                        for (i = 0; i < roopCount; i += 1) {
                            if (this._curCircularArray._array[roopIdx].removed) {
                                roopIdx += 1;
                                if (roopIdx > (this._curCircularArray.length() - 1)) {
                                    roopIdx = 0;
                                }
                                continue;
                            }
                            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                                this._curCircularArray._array[roopIdx].majorChannel = num;
                            }
                            roopIdx += 1;
                            if (roopIdx > (this._curCircularArray.length() - 1)) {
                                roopIdx = 0;
                            }
                            num += 1;
                        }
                        for (i = 0; i < currentFocus; i += 1) {
                            this._curCircularArray.dec();
                            while (this._curCircularArray.curItem().removed === true) {
                                this._curCircularArray.dec();
                            }
                        }
                        //this._chList.$.remove();
                        this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
                        this.appendChList(true);
                        this._chList.setItemFocus(currentFocus);
                        for (i = 0; i < currentFocus; i += 1) {
                            this._curCircularArray.inc();
                            while (this._curCircularArray.curItem().removed === true) {
                                this._curCircularArray.inc();
                            }
                        }
                        //var item = this._chList.$.children('.mov');
                        //item.removeClass('mov');
                        //item.addClass('on');
                        this._selNum = 0;
                        this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
                        this._currentState = 'none';
                    }
                    else if (this._currentState === 'multimov') {
                        this._isChange = true;
                        if (hx.config.useFavouriteCollection) {
                            var targetChIndex = hx.svc.CH_LIST.moveChListItem({
                                favtype: this._chType,
                                ccid: this._currentMoveItemList[0].ccid,
                                insertTargetccid: this._curCircularArray.getNextItem().ccid
                            });
                            for (i = 1; i < this._currentMoveItemList.length; i += 1) {
                                if (hx.config.useFavouriteCollection) {
                                    hx.svc.CH_LIST.moveChListItemWithTargetIdx({
                                        favtype: this._chType,
                                        ccid: this._currentMoveItemList[i].ccid,
                                        targetIdx: targetChIndex,
                                        offset: i
                                    });
                                }
                            }
                        }
                        this._currentMoveItemList[0].majorChannel = 100000;
                        //remove previous number
                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._currentMoveItemList[0]);
                        for (i = 1; i < this._currentMoveItemList.length; i += 1) {
                            this._currentMoveItemList[i].majorChannel = 100000;
                            //remove previous number
                            this._curCircularArray._array.splice(this._curCircularArray._idxCur + i, 0, this._currentMoveItemList[i]);
                        }
                        currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;
                        var lastMoveItemIdx;
                        //find smallestLcnIdx
                        smallestLcn = this._curCircularArray._array[0].majorChannel;
                        smallestLcnIdx = 0;
                        for (i = 1; i < this._curCircularArray.length(); i += 1) {
                            if (smallestLcn > this._curCircularArray._array[i].majorChannel) {
                                smallestLcn = this._curCircularArray._array[i].majorChannel;
                                smallestLcnIdx = i;
                            }
                        }
                        for (i = 0; i < this._curCircularArray.length(); i += 1) {
                            if (this._currentMoveItemList[this._currentMoveItemList.length - 1].ccid === this._curCircularArray._array[i].ccid) {
                                lastMoveItemIdx = i;
                                break;
                            }
                        }
                        //if insertTargetIdx is just before smallestLcnIdx, set smallestLcnIdx with moveItem
                        if (this._curCircularArray.getNextItemWithCurIdx(lastMoveItemIdx).ccid === this._curCircularArray._array[smallestLcnIdx].ccid) {
                            for (i = 0; i < this._curCircularArray.length(); i += 1) {
                                if (this._currentMoveItemList[0].ccid === this._curCircularArray._array[i].ccid) {
                                    smallestLcnIdx = i;
                                    break;
                                }
                            }
                        }
                        if (ENV.listModel.IR4000HD === ENV.model) {
                            if (this._chType === 'TV') {
                                num = 1;
                            }
                            else if (this._chType === 'RADIO') {
                                num = 5000;
                            }
                        }
                        else {
                            if (hx.config.useLcnUpdate) {
                                if (this._chType === 'TV') {
                                    num = (this._lastMajorChannelOfTvLcnList + 1);
                                }
                                else if (this._chType === 'RADIO') {
                                    num = (this._lastMajorChannelOfRadioLcnList + 1);
                                }
                            }
                            else {
                                if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                                    num = 100;
                                }
                            }
                        }
                        roopIdx = smallestLcnIdx;
                        roopCount = this._curCircularArray.length();
                        for (i = 0; i < roopCount; i += 1) {
                            if (this._curCircularArray._array[roopIdx].removed) {
                                roopIdx += 1;
                                if (roopIdx > (this._curCircularArray.length() - 1)) {
                                    roopIdx = 0;
                                }
                                continue;
                            }
                            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                                this._curCircularArray._array[roopIdx].majorChannel = num;
                            }
                            roopIdx += 1;
                            if (roopIdx > (this._curCircularArray.length() - 1)) {
                                roopIdx = 0;
                            }
                            num += 1;
                        }
                        for (i = 0; i < currentFocus; i += 1) {
                            this._curCircularArray.dec();
                            while (this._curCircularArray.curItem().removed === true) {
                                this._curCircularArray.dec();
                            }
                        }
                        //this._chList.$.remove();
                        this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
                        this.appendChList(true);
                        this._chList.setItemFocus(currentFocus);
                        for (i = 0; i < currentFocus; i += 1) {
                            this._curCircularArray.inc();
                            while (this._curCircularArray.curItem().removed === true) {
                                this._curCircularArray.inc();
                            }
                        }
                        //var item = this._chList.$.children('.mov');
                        //item.removeClass('mov');
                        //item.addClass('on');
                        this._selNum = 0;
                        this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
                        this._currentState = 'none';
                    }
                    break;
                case this._optList:
                    bConsumed = true;
                    switch (this._optList.$.children().eq(data.index).text()) {
                        case hx.l('LOC_DELETE_ID'):
                            this._deleteItem();
                            break;
                        case hx.l('LOC_LOCK_ID'):
                            this._lockItem();
                            break;
                        case hx.l('LOC_UNLOCK_ID'):
                            this._unlockItem();
                            break;
                        case hx.l('LOC_RENAME_ID'):
                            this._renameItem();
                            break;
                        case hx.l('LOC_MOVE_ID'):
                            this._moveItem();
                            break;
                        case hx.l('LOC_CHANGE_GROUP_ID'):
                            this._$opt.append(this._optGroupList.$);
                            this.append(this._optGroupList);
                            this._optGroupList.on();
                            this._optGroupList.setItemFocus(0);
                            this._optList.$.remove();
                            return;
                        case hx.l('LOC_SELECT_ALL_ID'):
                            this._selectAll();
                            break;
                        case hx.l('LOC_DESELECT_ALL_ID'):
                            this._deselectAll();
                            break;
                        case hx.l('LOC_CANCEL2_ID'):
                            this._moveCancel();
                            break;
                        case hx.l('LOC_OK_ID'):
                            this._moveOk();
                            break;
                    }
                    this._$opt.removeClass('on');
                    this._chList.on();
                    break;
                case this._optGroupList:
                    bConsumed = true;
                    this._changeGroup(data.index);
                    break;
            }
            return bConsumed;
        };
        CPgEditChannels.prototype._on_alScrollup = function (aParam) {
            var data = aParam.alData, listItem, itemList;
            var currentFocus, bConsumed = false;
            var curItem, curIndex;
            if (data) {
                var target = data.me;
                switch (target) {
                    case this._chList:
                        bConsumed = true;
                        if (this._currentState === 'none') {
                            if (data.index === 0 && this._allNum > 8) {
                                this.updateChListUp();
                            }
                            else {
                                if (data.index === 0) {
                                    this._chList.setItemFocus(this._chList.$.children('li').length - 1);
                                }
                                this._curCircularArray.dec();
                                if (this._curCircularArray.curItem()) {
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.dec();
                                    }
                                }
                            }
                        }
                        else if (this._currentState === 'mov') {
                            this._chList.$.children('.mov').removeClass('mov');
                            //change item
                            curItem = this._curCircularArray.curItem();
                            curIndex = this._curCircularArray._idxCur;
                            while (this._curCircularArray.getPrevItem().removed) {
                                this._curCircularArray.dec();
                            }
                            this._curCircularArray._array.splice(curIndex, 1, this._curCircularArray.getPrevItem());
                            //move PrevItem to curItem
                            this._curCircularArray.dec();
                            this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, curItem);
                            //move curItem to PrevItem
                            this._curCircularArray.inc();
                            while (this._curCircularArray.curItem().removed) {
                                this._curCircularArray.inc();
                            }
                            if (data.index === 0 && this._allNum > 8) {
                                this.updateChListUp();
                            }
                            else {
                                if (data.index === 0) {
                                    itemList = this._chList.$.children('li');
                                    listItem = itemList.eq(0)[0].innerHTML;
                                    itemList.eq(0)[0].innerHTML = itemList.eq(this._allNum - 1)[0].innerHTML;
                                    itemList.eq(this._allNum - 1)[0].innerHTML = listItem;
                                    this._chList.setItemFocus(itemList.length - 1);
                                }
                                else {
                                    currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                                    itemList = this._chList.$.children('li');
                                    listItem = itemList.eq(currentFocus)[0].innerHTML;
                                    itemList.eq(currentFocus)[0].innerHTML = itemList.eq(currentFocus + 1)[0].innerHTML;
                                    itemList.eq(currentFocus + 1)[0].innerHTML = listItem;
                                    this._chList.setItemFocus(currentFocus);
                                }
                                this._curCircularArray.dec();
                                while (this._curCircularArray.curItem().removed === true) {
                                    this._curCircularArray.dec();
                                }
                            }
                            this._chList.getFocusedItem().addClass('mov');
                        }
                        else if (this._currentState === 'multimov') {
                            this._chList.$.children('.mov.grp').removeClass('mov grp');
                            //change item
                            curItem = this._curCircularArray.curItem();
                            curIndex = this._curCircularArray._idxCur;
                            while (this._curCircularArray.getPrevItem().removed) {
                                this._curCircularArray.dec();
                            }
                            this._curCircularArray._array.splice(curIndex, 1, this._curCircularArray.getPrevItem());
                            //move PrevItem to curItem
                            this._curCircularArray.dec();
                            this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, curItem);
                            //move curItem to PrevItem
                            this._curCircularArray.inc();
                            while (this._curCircularArray.curItem().removed) {
                                this._curCircularArray.inc();
                            }
                            if (data.index === 0 && (this._allNum - this._selNum) >= 8) {
                                this.updateChListUp();
                            }
                            else {
                                if (data.index === 0) {
                                    itemList = this._chList.$.children('li');
                                    listItem = itemList.eq(0)[0].innerHTML;
                                    itemList.eq(0)[0].innerHTML = itemList.eq(this._allNum - this._selNum)[0].innerHTML;
                                    itemList.eq(this._allNum - this._selNum)[0].innerHTML = listItem;
                                    this._chList.setItemFocus(itemList.length - 1);
                                }
                                else {
                                    currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                                    itemList = this._chList.$.children('li');
                                    listItem = itemList.eq(currentFocus)[0].innerHTML;
                                    itemList.eq(currentFocus)[0].innerHTML = itemList.eq(currentFocus + 1)[0].innerHTML;
                                    itemList.eq(currentFocus + 1)[0].innerHTML = listItem;
                                    this._chList.setItemFocus(currentFocus);
                                }
                                this._curCircularArray.dec();
                                while (this._curCircularArray.curItem().removed === true) {
                                    this._curCircularArray.dec();
                                }
                            }
                            this._chList.getFocusedItem().addClass('mov grp');
                        }
                        break;
                    case this._optList:
                        bConsumed = true;
                        alert('up - opt');
                        break;
                }
            }
            return bConsumed;
        };
        CPgEditChannels.prototype._on_alScrolldown = function (aParam) {
            var data = aParam.alData, bConsumed = false;
            var itemList, listItem, currentFocus;
            if (data) {
                var target = data.me;
                switch (target) {
                    case this._chList:
                        bConsumed = true;
                        if (this._currentState === 'none') {
                            if (data.index === 7 && this._allNum > 8) {
                                this.updateChListDown();
                            }
                            else {
                                if (data.index === this._allNum - 1) {
                                    this._chList.setItemFocus(0);
                                }
                                this._curCircularArray.inc();
                                if (this._curCircularArray.curItem()) {
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.inc();
                                    }
                                }
                            }
                        }
                        else if (this._currentState === 'mov') {
                            this._chList.$.children('.mov').removeClass('mov');
                            //change item
                            var curItem = this._curCircularArray.curItem();
                            var curIndex = this._curCircularArray._idxCur;
                            while (this._curCircularArray.getNextItem().removed) {
                                this._curCircularArray.inc();
                            }
                            this._curCircularArray._array.splice(curIndex, 1, this._curCircularArray.getNextItem());
                            //move NextItem to curItem
                            this._curCircularArray.inc();
                            this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, curItem);
                            //move curItem to NextItem
                            this._curCircularArray.dec();
                            while (this._curCircularArray.curItem().removed) {
                                this._curCircularArray.dec();
                            }
                            if (data.index === 7 && this._allNum > 8) {
                                this.updateChListDown();
                            }
                            else {
                                if (data.index === this._allNum - 1) {
                                    itemList = this._chList.$.children('li');
                                    listItem = itemList.eq(this._allNum - 1)[0].innerHTML;
                                    itemList.eq(this._allNum - 1)[0].innerHTML = itemList.eq(0)[0].innerHTML;
                                    itemList.eq(0)[0].innerHTML = listItem;
                                    this._chList.setItemFocus(0);
                                }
                                else {
                                    currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                                    itemList = this._chList.$.children('li');
                                    listItem = itemList.eq(currentFocus)[0].innerHTML;
                                    itemList.eq(currentFocus)[0].innerHTML = itemList.eq(currentFocus - 1)[0].innerHTML;
                                    itemList.eq(currentFocus - 1)[0].innerHTML = listItem;
                                    this._chList.setItemFocus(currentFocus);
                                }
                                this._curCircularArray.inc();
                                while (this._curCircularArray.curItem().removed === true) {
                                    this._curCircularArray.inc();
                                }
                            }
                            this._chList.getFocusedItem().addClass('mov');
                        }
                        else if (this._currentState === 'multimov') {
                            this._chList.$.children('.mov.grp').removeClass('mov grp');
                            //change item
                            curItem = this._curCircularArray.curItem();
                            curIndex = this._curCircularArray._idxCur;
                            while (this._curCircularArray.getNextItem().removed) {
                                this._curCircularArray.inc();
                            }
                            this._curCircularArray._array.splice(curIndex, 1, this._curCircularArray.getNextItem());
                            //move NextItem to curItem
                            this._curCircularArray.inc();
                            this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, curItem);
                            //move curItem to NextItem
                            this._curCircularArray.dec();
                            while (this._curCircularArray.curItem().removed) {
                                this._curCircularArray.dec();
                            }
                            if (data.index === 7 && (this._allNum - this._selNum) >= 8) {
                                this.updateChListDown();
                            }
                            else {
                                if (data.index === (this._allNum - this._selNum)) {
                                    itemList = this._chList.$.children('li');
                                    listItem = itemList.eq(this._allNum - this._selNum)[0].innerHTML;
                                    itemList.eq(this._allNum - this._selNum)[0].innerHTML = itemList.eq(0)[0].innerHTML;
                                    itemList.eq(0)[0].innerHTML = listItem;
                                    this._chList.setItemFocus(0);
                                }
                                else {
                                    currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                                    itemList = this._chList.$.children('li');
                                    listItem = itemList.eq(currentFocus)[0].innerHTML;
                                    itemList.eq(currentFocus)[0].innerHTML = itemList.eq(currentFocus - 1)[0].innerHTML;
                                    itemList.eq(currentFocus - 1)[0].innerHTML = listItem;
                                    this._chList.setItemFocus(currentFocus);
                                }
                                this._curCircularArray.inc();
                                while (this._curCircularArray.curItem().removed === true) {
                                    this._curCircularArray.inc();
                                }
                            }
                            this._chList.getFocusedItem().addClass('mov grp');
                        }
                        break;
                    case this._optList:
                        bConsumed = true;
                        alert('down - opt');
                        break;
                }
            }
            return bConsumed;
        };
        CPgEditChannels.prototype._on_alExit = function (aParam) {
            var that = this, data = aParam.alData, bConsumed = false, i;
            var fn = function (_status) {
                if (_status === 'key_power' || _status === hx.l('LOC_NO_ID')) {
                    if (that._isChange) {
                        hx.svc.CH_LIST.forceChListUpdate();
                    }
                    that._gotoSettingsPage();
                }
                else if (_status === hx.l('LOC_YES_ID')) {
                    that._updateEditedChList('setting');
                }
                else if (_status === 'key_back' || _status === 'key_esc') {
                    that._chList.on();
                }
            };
            if (data) {
                var target = data.me;
                switch (target) {
                    case this._chList:
                        bConsumed = true;
                        if (this._currentState === 'mov' || this._currentState === 'multimov') {
                            this._curCircularArray = this._moveBackupList.clone();
                            for (i = 0; i < this._moveBackupFocus; i += 1) {
                                this._curCircularArray.dec();
                                while (this._curCircularArray.curItem().removed === true) {
                                    this._curCircularArray.dec();
                                }
                            }
                            //this._chList.$.remove();
                            this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
                            this.appendChList(true);
                            this._chList.setItemFocus(this._moveBackupFocus);
                            for (i = 0; i < this._moveBackupFocus; i += 1) {
                                this._curCircularArray.inc();
                                while (this._curCircularArray.curItem().removed === true) {
                                    this._curCircularArray.inc();
                                }
                            }
                            this._selNum = 0;
                            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
                            this._currentState = 'none';
                        }
                        else if (this._isChange) {
                            this.confirm = hx.msg('confirm', {
                                text: hx.l('LOC_SAVE_THE_CHANGE_ID'),
                                auto_close: false,
                                //timeout : 5000,
                                btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                                dia_class: 'osd',
                                callback_fn: fn
                            });
                        }
                        else {
                            this._gotoSettingsPage();
                        }
                        break;
                    case this._optList:
                        bConsumed = true;
                        if (this._$opt.hasClass('on')) {
                            this._$opt.removeClass('on');
                            this._chList.on();
                        }
                        else {
                            if (this._isChange) {
                                this.confirm = hx.msg('confirm', {
                                    text: hx.l('LOC_SAVE_THE_CHANGE_ID'),
                                    auto_close: false,
                                    //timeout : 5000,
                                    btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                                    dia_class: 'osd',
                                    callback_fn: fn
                                });
                            }
                            else {
                                this._gotoSettingsPage();
                            }
                        }
                        break;
                    case this._optGroupList:
                        bConsumed = true;
                        if (this._$opt.hasClass('on')) {
                            this._$opt.removeClass('on');
                            this._optGroupList.$.remove();
                            this._$opt.append(this._optList.$);
                            this.append(this._optList);
                            this._chList.on();
                        }
                        else {
                            if (this._isChange) {
                                this.confirm = hx.msg('confirm', {
                                    text: hx.l('LOC_SAVE_THE_CHANGE_ID'),
                                    auto_close: false,
                                    //timeout : 5000,
                                    btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                                    dia_class: 'osd',
                                    callback_fn: fn
                                });
                            }
                            else {
                                this._gotoSettingsPage();
                            }
                        }
                        break;
                }
            }
            return bConsumed;
        };
        CPgEditChannels.prototype._on_alKey = function (aParam) {
            var that = this, data = aParam.alData, i, bConsumed = false, currentFocus, curItem;
            if (data) {
                var target = data.me;
            }
            var fn = function (_status) {
                if (_status === 'key_power' || _status === hx.l('LOC_NO_ID')) {
                    if (that._isChange) {
                        hx.svc.CH_LIST.forceChListUpdate();
                    }
                    that._goExit('live');
                }
                else if (_status === hx.l('LOC_YES_ID')) {
                    that._updateEditedChList('live');
                }
                else if (_status === 'key_back' || _status === 'key_esc') {
                    that._chList.on();
                }
            };
            switch (aParam.alKey) {
                case hx.key.KEY_ESCAPE:
                    if (data) {
                        bConsumed = true;
                        if (this._isChange) {
                            this.confirm = hx.msg('confirm', {
                                text: hx.l('LOC_SAVE_THE_CHANGE_ID'),
                                auto_close: false,
                                //timeout : 5000,
                                btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                                dia_class: 'osd',
                                callback_fn: fn
                            });
                        }
                        else {
                            this._goExit('live');
                        }
                    }
                    break;
                case hx.key.KEY_OPTION:
                    if (data) {
                        bConsumed = true;
                        if (this._$opt.hasClass('on')) {
                            this._$opt.removeClass('on');
                            this._optList.$.remove();
                            this._optGroupList.$.remove();
                            this._chList.on();
                        }
                        else {
                            if (target === this._chList) {
                                this._optList.$.remove();
                                if (this._selNum >= this._allNum) {
                                    // after deleting and select all times then this._selNum > this._allNum
                                    this._optList = CPgEditChannels.optListMenu(['LOC_DELETE_ID', 'LOC_LOCK_ID', 'LOC_CHANGE_GROUP_ID', 'LOC_DESELECT_ALL_ID'], 'menu');
                                }
                                else if ((this._selNum > 1) && !((this._currentList === 'mov') || (this._currentState === 'multimov'))) {
                                    if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT)
                                        this._optList = CPgEditChannels.optListMenu(['LOC_DELETE_ID', 'LOC_LOCK_ID', 'LOC_CHANGE_GROUP_ID', 'LOC_SELECT_ALL_ID', 'LOC_DESELECT_ALL_ID'], 'menu');
                                    else
                                        this._optList = CPgEditChannels.optListMenu(['LOC_DELETE_ID', 'LOC_LOCK_ID', 'LOC_MOVE_ID', 'LOC_CHANGE_GROUP_ID', 'LOC_SELECT_ALL_ID', 'LOC_DESELECT_ALL_ID'], 'menu');
                                }
                                else if ((this._currentState === 'mov') || (this._currentState === 'multimov')) {
                                    this._optList = CPgEditChannels.optListMenu(['LOC_OK_ID', 'LOC_CANCEL2_ID'], 'menu');
                                }
                                else {
                                    this._optList = CPgEditChannels.optListMenu(this._contextListMap[0], 'menu');
                                }
                                this._$opt.append(this._optList.$);
                                this._islock = this._checkLockItem();
                                if (this._islock) {
                                    $('#opt .menu').children('li').eq(1).html(hx.l('LOC_UNLOCK_ID'));
                                }
                                else if (!((this._currentState === 'mov') || (this._currentState === 'multimov'))) {
                                    $('#opt .menu').children('li').eq(1).html(hx.l('LOC_LOCK_ID'));
                                }
                                this.append(this._optList);
                                this._$opt.addClass('on');
                                this._optList.on();
                                this._optList.setItemFocus(0);
                            }
                            else {
                                this._optList.$.remove();
                                this._optList = CPgEditChannels.optListMenu(this._contextListMap[0], 'menu');
                                this._$opt.append(this._optList.$);
                                this.append(this._optList);
                                this._$opt.addClass('on');
                                this._optList.on();
                                this._optList.setItemFocus(4);
                            }
                        }
                    }
                    break;
                case hx.key.KEY_CHANNEL_UP:
                    if (data) {
                        bConsumed = true;
                        if (target === this._chList) {
                            if (this._currentState === 'none') {
                                if (this._allNum > 8) {
                                    currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                                    for (i = 0; i < (currentFocus + 8); i += 1) {
                                        this._curCircularArray.dec();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.dec();
                                        }
                                    }
                                    //this._chList.$.remove();
                                    this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                    this.appendChList(true);
                                    for (i = 0; i < currentFocus; i += 1) {
                                        this._curCircularArray.inc();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.inc();
                                        }
                                    }
                                    this._chList.setItemFocus(currentFocus);
                                }
                            }
                            else if (this._currentState === 'mov') {
                                if (this._allNum > 8) {
                                    currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;
                                    this._chList.$.children('.on').removeClass('on');
                                    this._chList.$.children('.mov').removeClass('mov');
                                    for (i = 0; i < 8; i += 1) {
                                        curItem = this._curCircularArray.curItem();
                                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._curCircularArray.getPrevItem());
                                        this._curCircularArray.dec();
                                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, curItem);
                                        this._curCircularArray.inc();
                                        this._curCircularArray.dec();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.dec();
                                        }
                                    }
                                    for (i = 0; i < currentFocus; i += 1) {
                                        this._curCircularArray.dec();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.dec();
                                        }
                                    }
                                    //this._chList.$.remove();
                                    this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                    this.appendChList(true);
                                    for (i = 0; i < currentFocus; i += 1) {
                                        this._curCircularArray.inc();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.inc();
                                        }
                                    }
                                    this._chList.setItemFocus(currentFocus);
                                    this._chList.getItem(currentFocus).addClass('mov');
                                }
                            }
                            else if (this._currentState === 'multimov') {
                                if ((this._allNum - this._selNum + 1) > 8) {
                                    currentFocus = this._chList.$.children('.mov.grp').prevAll('.focusable').length;
                                    this._chList.$.children('.on').removeClass('on');
                                    this._chList.$.children('.mov.grp').removeClass('mov grp');
                                    for (i = 0; i < 8; i += 1) {
                                        curItem = this._curCircularArray.curItem();
                                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._curCircularArray.getPrevItem());
                                        this._curCircularArray.dec();
                                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, curItem);
                                        this._curCircularArray.inc();
                                        this._curCircularArray.dec();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.dec();
                                        }
                                    }
                                    for (i = 0; i < currentFocus; i += 1) {
                                        this._curCircularArray.dec();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.dec();
                                        }
                                    }
                                    //this._chList.$.remove();
                                    this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                    this.appendChList(true);
                                    for (i = 0; i < currentFocus; i += 1) {
                                        this._curCircularArray.inc();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.inc();
                                        }
                                    }
                                    this._chList.setItemFocus(currentFocus);
                                    this._chList.getItem(currentFocus).addClass('mov grp');
                                }
                            }
                        }
                    }
                    break;
                case hx.key.KEY_CHANNEL_DOWN:
                    if (data) {
                        bConsumed = true;
                        if (target === this._chList) {
                            if (this._currentState === 'none') {
                                if (this._allNum > 8) {
                                    currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                                    for (i = 0; i < (8 - currentFocus); i += 1) {
                                        this._curCircularArray.inc();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.inc();
                                        }
                                    }
                                    //this._chList.$.remove();
                                    this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                    this.appendChList(true);
                                    for (i = 0; i < currentFocus; i += 1) {
                                        this._curCircularArray.inc();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.inc();
                                        }
                                    }
                                    this._chList.setItemFocus(currentFocus);
                                }
                            }
                            else if (this._currentState === 'mov') {
                                if (this._allNum > 8) {
                                    currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;
                                    this._chList.$.children('.on').removeClass('on');
                                    this._chList.$.children('.mov').removeClass('mov');
                                    for (i = 0; i < 8; i += 1) {
                                        curItem = this._curCircularArray.curItem();
                                        //change item
                                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._curCircularArray.getNextItem());
                                        this._curCircularArray.inc();
                                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, curItem);
                                        this._curCircularArray.dec();
                                        this._curCircularArray.inc();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.inc();
                                        }
                                    }
                                    for (i = 0; i < currentFocus; i += 1) {
                                        this._curCircularArray.dec();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.dec();
                                        }
                                    }
                                    //this._chList.$.remove();
                                    this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                    this.appendChList(true);
                                    for (i = 0; i < currentFocus; i += 1) {
                                        this._curCircularArray.inc();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.inc();
                                        }
                                    }
                                    this._chList.setItemFocus(currentFocus);
                                    this._chList.getItem(currentFocus).addClass('mov');
                                }
                            }
                            else if (this._currentState === 'multimov') {
                                if ((this._allNum - this._selNum + 1) > 8) {
                                    currentFocus = this._chList.$.children('.mov.grp').prevAll('.focusable').length;
                                    this._chList.$.children('.on').removeClass('on');
                                    this._chList.$.children('.mov.grp').removeClass('mov grp');
                                    for (i = 0; i < 8; i += 1) {
                                        curItem = this._curCircularArray.curItem();
                                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._curCircularArray.getNextItem());
                                        this._curCircularArray.inc();
                                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, curItem);
                                        this._curCircularArray.dec();
                                        this._curCircularArray.inc();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.inc();
                                        }
                                    }
                                    for (i = 0; i < currentFocus; i += 1) {
                                        this._curCircularArray.dec();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.dec();
                                        }
                                    }
                                    //this._chList.$.remove();
                                    this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                    this.appendChList(true);
                                    for (i = 0; i < currentFocus; i += 1) {
                                        this._curCircularArray.inc();
                                        while (this._curCircularArray.curItem().removed === true) {
                                            this._curCircularArray.inc();
                                        }
                                    }
                                    this._chList.setItemFocus(currentFocus);
                                    this._chList.getItem(currentFocus).addClass('mov grp');
                                }
                            }
                        }
                    }
                    break;
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
            }
            return bConsumed;
        };
        CPgEditChannels.prototype._updateEditedChList = function (aTargetPage) {
            var _this = this;
            var that = this;
            this.progressMsg = hx.msg('progress', {
                text: hx.l('LOC_SAVING_ID'),
                _use_backkey: false,
                auto_close: false
            });
            function _updateChList(cicularChList, type) {
                var idx, len_of_list, currentItem, backupCur, backupCircular = [];
                var j, item;
                var deleteChLists = [], updatedChLists = [];
                backupCircular.length = 0;
                if (type === 'TV') {
                    backupCircular = that._circularTVArrayBackup;
                }
                else if (type === 'RADIO') {
                    backupCircular = that._circularRADIOArrayBackup;
                }
                backupCircular.setCur(0);
                cicularChList.setCur(0);
                if (hx.config.useFavouriteCollection) {
                    hx.svc.CH_LIST.editChListDoCommit({
                        group: type
                    });
                }
                len_of_list = cicularChList.length();
                for (idx = 0; idx < len_of_list; idx += 1) {
                    // GET Current
                    currentItem = cicularChList.curItem();
                    backupCur = backupCircular.curItem();
                    if (currentItem.removed) {
                        var bDeleted = (currentItem.removed) ? true : false;
                        for (j = 0; j < that._recentDataList.length; j += 1) {
                            if (that._recentDataList[j] === currentItem.ccid) {
                                that.sendEventToEntity({
                                    alEvent: 'alMessageToPage',
                                    target: 'CPgHubProxy/updateChannel?ccid=' + currentItem.ccid + ',bDeleted=' + bDeleted,
                                    me: that
                                });
                            }
                        }
                        item = {
                            group: type,
                            ccid: currentItem.ccid
                        };
                        deleteChLists.push(item);
                        if (that._currentChObj) {
                            if (that._currentChObj.result) {
                                if (that._currentChObj.channel) {
                                    if (that._currentChObj.channel.ccid === item.ccid) {
                                        hx.svc.AV_CTRL.stopCurChannel({
                                            stopAV: 1,
                                            stopTsr: 0
                                        });
                                        that._isAvBlock = true;
                                    }
                                }
                            }
                        }
                    }
                    else if ((currentItem.manualBlock !== backupCur.manualBlock) || (currentItem.name !== backupCur.name)) {
                        if (currentItem.majorChannel !== backupCur.majorChannel) {
                            that._ismoveChannel = true;
                        }
                        item = {
                            group: type,
                            ccid: currentItem.ccid,
                            isLockChange: currentItem.isBlockChange,
                            lock: currentItem.manualBlock,
                            isNameChange: currentItem.isNameChange,
                            name: currentItem.name,
                            isRemoved: currentItem.removed,
                            majorChannel: currentItem.majorChannel,
                            isLcnChange: that._ismoveChannel
                        };
                        updatedChLists.push(item);
                    }
                    // GOTO NEXT
                    cicularChList.inc();
                    backupCircular.inc();
                }
                hx.log('error', '\tdeleteChLists.length : ' + deleteChLists.length);
                hx.log('error', '\tupdatedChLists.length : ' + updatedChLists.length);
                hx.svc.CH_LIST.removeChannels({
                    'deleteChLists': deleteChLists,
                    'group': type
                });
                hx.svc.CH_LIST.updateChannels({
                    'updatedChLists': updatedChLists,
                    'group': type
                });
                hx.svc.HISTORY.deleteHistory(deleteChLists);
                hx.svc.HISTORY.updateHistory2(updatedChLists);
                deleteChLists = [];
                updatedChLists = [];
                backupCircular = [];
            }
            //animation
            var $target = $('#DIA .vaTop');
            hx.il.ani.action({
                type: 'in',
                targets: $target,
                cb: function () {
                    switch (that._chType) {
                        case 'TV':
                            that._circularTVArray = that._curCircularArray;
                            break;
                        case 'RADIO':
                            that._circularRADIOArray = that._curCircularArray;
                            break;
                    }
                    _updateChList(that._circularTVArray, 'TV');
                    _updateChList(that._circularRADIOArray, 'RADIO');
                    hx.svc.CH_LIST.forceChListUpdate();
                    var dsrvRecordings = hx.svc.RECORDINGS;
                    dsrvRecordings.refreshScheduleList();
                    setTimeout(function (time) {
                        hx.msg.close('progress', that.progressMsg);
                        hx.log("warn", "timer: " + time);
                        _this._changeToAvailCh(_this._currentChObj.group);
                        if (aTargetPage === 'live') {
                            that._goExit('live');
                        }
                        else if (aTargetPage === 'setting') {
                            that._gotoSettingsPage();
                        }
                    }, 1000);
                }
            });
        };
        CPgEditChannels.prototype._gotoSettingsPage = function () {
            this._goExit('setting');
        };
        CPgEditChannels.prototype._getCircularItem = function (aLength, aOffset) {
            var i, curIndex, showList = [];
            curIndex = this._curCircularArray.cur();
            for (i = 0; i < aOffset; i += 1) {
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
            for (i = 0; i < this._curCircularArray.length(); i += 1) {
                if (this._curCircularArray.curItem().removed !== true) {
                    showList.push(this._curCircularArray.curItem());
                }
                this._curCircularArray.inc();
                if (showList.length === aLength) {
                    break;
                }
            }
            this._curCircularArray.setCur(curIndex);
            return showList;
        };
        CPgEditChannels.prototype._deleteItem = function () {
            var i, currentFocus, changedccidList = [], num = 0;
            if (this._allNum <= 0) {
                return;
            }
            changedccidList.length = 0;
            this._isChange = true;
            currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
            if (this._selNum === 0 && this._allNum > 0) {
                this._curCircularArray.curItem().removed = true;
                changedccidList.push(this._curCircularArray.curItem().ccid);
                this._allNum -= 1;
                this._curCircularArray.inc();
            }
            else {
                for (i = 0; i < this._curCircularArray.length(); i += 1) {
                    if (this._curCircularArray.curItem().removed === true) {
                        this._curCircularArray.inc();
                    }
                    else {
                        if (this._curCircularArray.curItem().select) {
                            this._curCircularArray.curItem().removed = true;
                            this._curCircularArray.curItem().select = false;
                            changedccidList.push(this._curCircularArray.curItem().ccid);
                            this._allNum -= 1;
                        }
                        this._curCircularArray.inc();
                    }
                }
            }
            if (this._allNum !== 0) {
                for (i = 0; i < currentFocus; i += 1) {
                    this._curCircularArray.dec();
                    while (this._curCircularArray.curItem().removed === true) {
                        this._curCircularArray.dec();
                    }
                }
            }
            if (ENV.listModel.IR4000HD === ENV.model) {
                if (this._chType === 'TV') {
                    num = 1;
                }
                else if (this._chType === 'RADIO') {
                    num = 5000;
                }
            }
            else {
                if (hx.config.useLcnUpdate) {
                    if (this._chType === 'TV') {
                        num = (this._lastMajorChannelOfTvLcnList + 1);
                    }
                    else if (this._chType === 'RADIO') {
                        num = (this._lastMajorChannelOfRadioLcnList + 1);
                    }
                }
                else {
                    if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                        num = 100;
                    }
                }
            }
            for (i = 0; i < this._curCircularArray.length(); i += 1) {
                if (this._curCircularArray._array[i].removed) {
                    continue;
                }
                if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                    this._curCircularArray._array[i].majorChannel = num;
                }
                num += 1;
            }
            //this._chList.$.remove();
            if (this._allNum !== 0) {
                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                this.appendChList(true);
                var chlength = this._chList.$.children('li').length;
                if (chlength - 1 < currentFocus) {
                    this._chList.setItemFocus(chlength - 1);
                }
                else {
                    this._chList.setItemFocus(currentFocus);
                }
            }
            else {
                this._currentList.length = 0;
                this.appendChList(true);
            }
            if (this._allNum !== 0) {
                for (i = 0; i < currentFocus; i += 1) {
                    this._curCircularArray.inc();
                    while (this._curCircularArray.curItem().removed === true) {
                        this._curCircularArray.inc();
                    }
                }
            }
            this._selNum = 0;
            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
        };
        CPgEditChannels.prototype._checkLockItem = function () {
            var i, isLock = false;
            if (this._selNum === 0 && this._allNum > 0) {
                if (this._curCircularArray.curItem().manualBlock) {
                    isLock = true;
                }
            }
            else {
                for (i = 0; i < this._curCircularArray.length(); i += 1) {
                    if (this._curCircularArray.curItem().select) {
                        if (this._curCircularArray.curItem().manualBlock) {
                            isLock = true;
                        }
                    }
                    this._curCircularArray.inc();
                }
            }
            return isLock;
        };
        CPgEditChannels.prototype._lockItem = function () {
            var i, changedccidList = [], currentFocus;
            if (this._allNum <= 0) {
                return;
            }
            changedccidList.length = 0;
            this._isChange = true;
            currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
            if (this._selNum === 0 && this._allNum > 0) {
                this._curCircularArray.curItem().manualBlock = true;
                this._curCircularArray.curItem().isBlockChange = true;
                changedccidList.push(this._curCircularArray.curItem().ccid);
            }
            else {
                for (i = 0; i < this._curCircularArray.length(); i += 1) {
                    if (this._curCircularArray.curItem().select) {
                        this._curCircularArray.curItem().select = false;
                        this._curCircularArray.curItem().manualBlock = true;
                        this._curCircularArray.curItem().isBlockChange = true;
                        changedccidList.push(this._curCircularArray.curItem().ccid);
                    }
                    this._curCircularArray.inc();
                }
            }
            for (i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
            //this._chList.$.remove();
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
            this.appendChList(true);
            this._chList.setItemFocus(currentFocus);
            for (i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
            this._selNum = 0;
            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
        };
        CPgEditChannels.prototype._unlockItem = function () {
            var i, changedccidList = [], currentFocus;
            if (this._allNum <= 0) {
                return;
            }
            changedccidList.length = 0;
            this._isChange = true;
            currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
            if (this._selNum === 0 && this._allNum > 0) {
                this._curCircularArray.curItem().manualBlock = false;
                this._curCircularArray.curItem().isBlockChange = true;
                changedccidList.push(this._curCircularArray.curItem().ccid);
            }
            else {
                for (i = 0; i < this._curCircularArray.length(); i += 1) {
                    if (this._curCircularArray.curItem().select) {
                        this._curCircularArray.curItem().select = false;
                        this._curCircularArray.curItem().manualBlock = false;
                        this._curCircularArray.curItem().isBlockChange = true;
                        changedccidList.push(this._curCircularArray.curItem().ccid);
                    }
                    this._curCircularArray.inc();
                }
            }
            for (i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
            //this._chList.$.remove();
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
            this.appendChList(true);
            this._chList.setItemFocus(currentFocus);
            for (i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
            this._selNum = 0;
            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
        };
        CPgEditChannels.prototype._renameItem = function () {
            var that = this;
            var i, changedName, changedccidList = [], currentFocus;
            if (this._allNum <= 0) {
                return;
            }
            changedccidList.length = 0;
            currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
            var _setRename = function () {
                for (i = 0; i < currentFocus; i += 1) {
                    that._curCircularArray.dec();
                    while (that._curCircularArray.curItem().removed === true) {
                        that._curCircularArray.dec();
                    }
                }
                //that._chList.$.remove();
                that._currentList = that.makeCurrentShowList(that._getCircularItem(8));
                that.appendChList(true);
                that._chList.setItemFocus(currentFocus);
                for (i = 0; i < currentFocus; i += 1) {
                    that._curCircularArray.inc();
                    while (that._curCircularArray.curItem().removed === true) {
                        that._curCircularArray.inc();
                    }
                }
                that._selNum = 0;
                that._hgroup.$.find('p').html(that._selNum + ' / ' + that._allNum);
            };
            var passwdTmp = hx.msg('passwd', {
                text: hx.l('LOC_ENTER_CHANNEL_NAME_ID'),
                responseText: hx.l('LOC_TYPE_NAME_PRESS_ENTER_ID'),
                wifi: 'Name',
                string: hx.l('LOC_CHANNEL_NAME_ID'),
                auto_close: false,
                dia_class: 'osd',
                callback_fn: fn
            });
            function fn(_status, _input) {
                if (_status === 'key_back' || _status === 'key_esc') {
                    for (i = 0; i < that._curCircularArray.length(); i += 1) {
                        if (that._curCircularArray.curItem().select) {
                            that._curCircularArray.curItem().select = false;
                        }
                        that._curCircularArray.inc();
                    }
                    _setRename();
                }
                else if (_status === hx.l('LOC_OK_ID')) {
                    if (_input) {
                        if (_input.password !== "") {
                            that._isChange = true;
                            if (that._selNum === 0 && that._allNum > 0) {
                                changedName = _input.password;
                                that._curCircularArray.curItem().name = changedName;
                                that._curCircularArray.curItem().isNameChange = true;
                                that._curCircularArray.curItem().select = false;
                                changedccidList.push(that._curCircularArray.curItem().ccid);
                            }
                            else {
                                for (i = 0; i < that._curCircularArray.length(); i += 1) {
                                    if (that._curCircularArray.curItem().select) {
                                        changedName = _input.password;
                                        that._curCircularArray.curItem().name = changedName;
                                        that._curCircularArray.curItem().isNameChange = true;
                                        that._curCircularArray.curItem().select = false;
                                        changedccidList.push(that._curCircularArray.curItem().ccid);
                                    }
                                    that._curCircularArray.inc();
                                }
                            }
                            _setRename();
                        }
                        else {
                            for (i = 0; i < that._curCircularArray.length(); i += 1) {
                                if (that._curCircularArray.curItem().select) {
                                    that._curCircularArray.curItem().select = false;
                                }
                                that._curCircularArray.inc();
                            }
                            _setRename();
                        }
                    }
                }
                else if (_status === 'close') {
                    for (i = 0; i < that._curCircularArray.length(); i += 1) {
                        if (that._curCircularArray.curItem().select) {
                            that._curCircularArray.curItem().select = false;
                        }
                        that._curCircularArray.inc();
                    }
                    _setRename();
                }
            }
        };
        CPgEditChannels.prototype._moveItem = function () {
            var i, currentFocus;
            if (this._allNum <= 0) {
                return;
            }
            if (this._selNum === 0) {
                currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                this._moveBackupList.length = 0;
                this._moveBackupList = this._curCircularArray.clone();
                this._moveBackupFocus = currentFocus;
                this._selNum += 1;
                this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
                this._currentState = 'mov';
                this._currentMoveItemList.length = 0;
                this._currentMoveItemList.push(this._curCircularArray.curItem());
                this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, {
                    ccid: false,
                    name: this._curCircularArray.curItem().name,
                    majorChannel: this._curCircularArray.curItem().majorChannel,
                    isHD: this._curCircularArray.curItem().isHD
                });
                for (i = 0; i < currentFocus; i += 1) {
                    this._curCircularArray.dec();
                    while (this._curCircularArray.curItem().removed === true) {
                        this._curCircularArray.dec();
                    }
                }
                //this._chList.$.remove();
                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                this.appendChList(true);
                for (i = 0; i < currentFocus; i += 1) {
                    this._curCircularArray.inc();
                    while (this._curCircularArray.curItem().removed === true) {
                        this._curCircularArray.inc();
                    }
                }
                this._chList.setItemFocus(currentFocus);
                this._chList.getFocusedItem().addClass('mov');
            }
            else if (this._selNum === 1) {
                currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                this._moveBackupList.length = 0;
                this._moveBackupList = this._curCircularArray.clone();
                this._moveBackupFocus = currentFocus;
                this._currentState = 'mov';
                this._currentMoveItemList.length = 0;
                for (i = 0; i < this._curCircularArray.length(); i += 1) {
                    if (this._curCircularArray.curItem().select) {
                        this._curCircularArray.curItem().select = false;
                        break;
                    }
                    this._curCircularArray.inc();
                }
                this._currentMoveItemList.push(this._curCircularArray.curItem());
                this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, {
                    ccid: false,
                    name: this._curCircularArray.curItem().name,
                    majorChannel: this._curCircularArray.curItem().majorChannel,
                    isHD: this._curCircularArray.curItem().isHD
                });
                for (i = 0; i < currentFocus; i += 1) {
                    this._curCircularArray.dec();
                    while (this._curCircularArray.curItem().removed === true) {
                        this._curCircularArray.dec();
                    }
                }
                //this._chList.$.remove();
                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                this.appendChList(true);
                for (i = 0; i < currentFocus; i += 1) {
                    this._curCircularArray.inc();
                    while (this._curCircularArray.curItem().removed === true) {
                        this._curCircularArray.inc();
                    }
                }
                this._chList.setItemFocus(currentFocus);
                this._chList.getFocusedItem().addClass('mov');
            }
            else if (this._selNum > 1) {
                currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                this._moveBackupList.length = 0;
                this._moveBackupList = this._curCircularArray.clone();
                this._moveBackupFocus = currentFocus;
                this._currentState = 'multimov';
                this._currentMoveItemList.length = 0;
                for (i = 0; i < this._curCircularArray.length(); i += 1) {
                    if (this._curCircularArray.curItem().select) {
                        this._curCircularArray.curItem().select = false;
                        this._currentMoveItemList.push(this._curCircularArray.curItem());
                        this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1);
                        this._curCircularArray._idxLast = this._curCircularArray.length() - 1;
                        if (this._curCircularArray._idxCur > this._curCircularArray._idxLast) {
                            this._curCircularArray._idxCur = 0;
                        }
                        i -= 1;
                    }
                    else {
                        this._curCircularArray.inc();
                    }
                }
                if (currentFocus + 1 > this._allNum - this._selNum) {
                    currentFocus = 0;
                }
                this._curCircularArray._array.splice(this._curCircularArray._idxCur, 0, {
                    name: '',
                    majorChannel: ''
                });
                for (i = 0; i < currentFocus; i += 1) {
                    this._curCircularArray.dec();
                    while (this._curCircularArray.curItem().removed === true) {
                        this._curCircularArray.dec();
                    }
                }
                //this._chList.$.remove();
                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                this.appendChList(true);
                for (i = 0; i < currentFocus; i += 1) {
                    this._curCircularArray.inc();
                    while (this._curCircularArray.curItem().removed === true) {
                        this._curCircularArray.inc();
                    }
                }
                this._chList.setItemFocus(currentFocus);
                this._chList.getFocusedItem().addClass('mov grp');
            }
        };
        CPgEditChannels.prototype._changeGroup = function (aIndex) {
            var i, channelLength;
            var chTypeStr = '';
            if ((this._chType === 'TV' && aIndex === 0) || (this._chType === 'RADIO' && aIndex === 1)) {
                this._$opt.removeClass('on');
                this._optGroupList.$.remove();
                this._$opt.append(this._optList.$);
                this.append(this._optList);
                this._chList.on();
                return;
            }
            switch (this._chType) {
                case 'TV':
                    this._circularTVArray = this._curCircularArray;
                    break;
                case 'RADIO':
                    this._circularRADIOArray = this._curCircularArray;
                    break;
            }
            switch (aIndex) {
                case 0:
                    //TV
                    channelLength = 0;
                    for (i = 0; i < this._circularTVArray.length(); i += 1) {
                        if (!this._circularTVArray.curItem().removed) {
                            channelLength += 1;
                        }
                        this._circularTVArray.inc();
                    }
                    this._curCircularArray = this._circularTVArray;
                    this._allNum = channelLength;
                    this._chType = 'TV';
                    break;
                case 1:
                    //RADIO
                    channelLength = 0;
                    for (i = 0; i < this._circularRADIOArray.length(); i += 1) {
                        if (!this._circularRADIOArray.curItem().removed) {
                            channelLength += 1;
                        }
                        this._circularRADIOArray.inc();
                    }
                    this._curCircularArray = this._circularRADIOArray;
                    this._allNum = channelLength;
                    this._chType = 'RADIO';
                    break;
            }
            if (this._chType === 'TV') {
                chTypeStr = hx.l('LOC_TV_ID');
            }
            else if (this._chType === 'RADIO') {
                chTypeStr = hx.l('LOC_RADIO_ID');
            }
            this._hgroup.$.find('h1').html(chTypeStr);
            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
            //this._chList.$.remove();
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
            this.appendChList(true);
            this._$opt.removeClass('on');
            this._optGroupList.$.remove();
            this._$opt.append(this._optList.$);
            this.append(this._optList);
        };
        CPgEditChannels.prototype._select_unselect_Item = function (aIndex) {
            if (this._curCircularArray.curItem().select) {
                this._curCircularArray.curItem().select = false;
                this._chList.getItem(aIndex).removeClass('sel');
                this._selNum -= 1;
                this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
            }
            else {
                this._curCircularArray.curItem().select = true;
                this._chList.getItem(aIndex).addClass('sel');
                this._selNum += 1;
                this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
            }
            if (aIndex === 7 && this._allNum > 8) {
                this.updateChListDown();
            }
            else {
                if (aIndex === this._allNum - 1) {
                    this._chList.setItemFocus(0);
                }
                else {
                    this._chList.getItem(aIndex + 1).addClass('on');
                }
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
        };
        CPgEditChannels.prototype._selectAll = function () {
            var i;
            if (this._allNum <= 0) {
                return;
            }
            var currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
            for (i = 0; i < this._curCircularArray.length(); i += 1) {
                if (!this._curCircularArray.curItem().select) {
                    this._curCircularArray.curItem().select = true;
                }
                this._curCircularArray.inc();
            }
            for (i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
            //this._chList.$.remove();
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
            this.appendChList(true);
            this._chList.setItemFocus(currentFocus);
            for (i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
            this._selNum = this._curCircularArray.length();
            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
        };
        CPgEditChannels.prototype._deselectAll = function () {
            var i;
            if (this._allNum <= 0) {
                return;
            }
            var currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
            for (i = 0; i < this._curCircularArray.length(); i += 1) {
                if (this._curCircularArray.curItem().select) {
                    this._curCircularArray.curItem().select = false;
                }
                this._curCircularArray.inc();
            }
            for (i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
            //this._chList.$.remove();
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
            this.appendChList(true);
            this._chList.setItemFocus(currentFocus);
            for (i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
            this._selNum = 0;
            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
        };
        CPgEditChannels.prototype._moveCancel = function () {
            var i, bConsumed = false;
            this._curCircularArray = this._moveBackupList.clone();
            for (i = 0; i < this._moveBackupFocus; i += 1) {
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
            this.appendChList(true);
            this._chList.setItemFocus(this._moveBackupFocus);
            for (i = 0; i < this._moveBackupFocus; i += 1) {
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
            this._selNum = 0;
            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
            this._currentState = 'none';
        };
        CPgEditChannels.prototype._moveOk = function () {
            var i;
            var num = 0, smallestLcn, smallestLcnIdx, roopIdx, roopCount, currentFocus;
            if (this._currentState === 'mov') {
                this._isChange = true;
                if (hx.config.useFavouriteCollection) {
                    hx.svc.CH_LIST.moveChListItem({
                        favtype: this._chType,
                        ccid: this._currentMoveItemList[0].ccid,
                        insertTargetccid: this._curCircularArray.getNextItem().ccid
                    });
                }
                this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._currentMoveItemList[0]);
                //insert
                currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;
                smallestLcn = this._curCircularArray._array[0].majorChannel;
                //find smallestLcnIdx
                smallestLcnIdx = 0;
                for (i = 1; i < this._curCircularArray.length(); i += 1) {
                    if (smallestLcn > this._curCircularArray._array[i].majorChannel) {
                        smallestLcn = this._curCircularArray._array[i].majorChannel;
                        smallestLcnIdx = i;
                    }
                }
                var tempSmallestLcnIdx = smallestLcnIdx;
                //if you move smallestLcnItem, find next smallestLcnItem
                if (this._curCircularArray._array[tempSmallestLcnIdx].ccid === this._currentMoveItemList[0].ccid) {
                    smallestLcn = this._curCircularArray._array[0].majorChannel;
                    smallestLcnIdx = 0;
                    for (i = 1; i < this._curCircularArray.length(); i += 1) {
                        if (smallestLcn > this._curCircularArray._array[i].majorChannel) {
                            if (tempSmallestLcnIdx === i) {
                                continue;
                            }
                            smallestLcn = this._curCircularArray._array[i].majorChannel;
                            smallestLcnIdx = i;
                        }
                    }
                }
                //if insertTargetIdx is just before smallestLcnIdx, set smallestLcnIdx with moveItem
                if (this._curCircularArray.getNextItem().ccid === this._curCircularArray._array[smallestLcnIdx].ccid) {
                    for (i = 0; i < this._curCircularArray.length(); i += 1) {
                        if (this._currentMoveItemList[0].ccid === this._curCircularArray._array[i].ccid) {
                            smallestLcnIdx = i;
                            break;
                        }
                    }
                }
            }
            else if (this._currentState === 'multimov') {
                this._isChange = true;
                if (hx.config.useFavouriteCollection) {
                    var targetChIndex = hx.svc.CH_LIST.moveChListItem({
                        favtype: this._chType,
                        ccid: this._currentMoveItemList[0].ccid,
                        insertTargetccid: this._curCircularArray.getNextItem().ccid
                    });
                    for (i = 1; i < this._currentMoveItemList.length; i += 1) {
                        if (hx.config.useFavouriteCollection) {
                            hx.svc.CH_LIST.moveChListItemWithTargetIdx({
                                favtype: this._chType,
                                ccid: this._currentMoveItemList[i].ccid,
                                targetIdx: targetChIndex,
                                offset: i
                            });
                        }
                    }
                }
                this._currentMoveItemList[0].majorChannel = 100000;
                //remove previous number
                this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._currentMoveItemList[0]);
                for (i = 1; i < this._currentMoveItemList.length; i += 1) {
                    this._currentMoveItemList[i].majorChannel = 100000;
                    //remove previous number
                    this._curCircularArray._array.splice(this._curCircularArray._idxCur + i, 0, this._currentMoveItemList[i]);
                }
                currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;
                var lastMoveItemIdx;
                //find smallestLcnIdx
                smallestLcn = this._curCircularArray._array[0].majorChannel;
                smallestLcnIdx = 0;
                for (i = 1; i < this._curCircularArray.length(); i += 1) {
                    if (smallestLcn > this._curCircularArray._array[i].majorChannel) {
                        smallestLcn = this._curCircularArray._array[i].majorChannel;
                        smallestLcnIdx = i;
                    }
                }
                for (i = 0; i < this._curCircularArray.length(); i += 1) {
                    if (this._currentMoveItemList[this._currentMoveItemList.length - 1].ccid === this._curCircularArray._array[i].ccid) {
                        lastMoveItemIdx = i;
                        break;
                    }
                }
                //if insertTargetIdx is just before smallestLcnIdx, set smallestLcnIdx with moveItem
                if (this._curCircularArray.getNextItemWithCurIdx(lastMoveItemIdx).ccid === this._curCircularArray._array[smallestLcnIdx].ccid) {
                    for (i = 0; i < this._curCircularArray.length(); i += 1) {
                        if (this._currentMoveItemList[0].ccid === this._curCircularArray._array[i].ccid) {
                            smallestLcnIdx = i;
                            break;
                        }
                    }
                }
            }
            if (ENV.listModel.IR4000HD === ENV.model) {
                if (this._chType === 'TV') {
                    num = 1;
                }
                else if (this._chType === 'RADIO') {
                    num = 5000;
                }
            }
            else {
                if (hx.config.useLcnUpdate) {
                    if (this._chType === 'TV') {
                        num = (this._lastMajorChannelOfTvLcnList + 1);
                    }
                    else if (this._chType === 'RADIO') {
                        num = (this._lastMajorChannelOfRadioLcnList + 1);
                    }
                }
                else {
                    if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                        num = 100;
                    }
                }
            }
            roopIdx = smallestLcnIdx;
            roopCount = this._curCircularArray.length();
            for (i = 0; i < roopCount; i += 1) {
                if (this._curCircularArray._array[roopIdx].removed) {
                    roopIdx += 1;
                    if (roopIdx > (this._curCircularArray.length() - 1)) {
                        roopIdx = 0;
                    }
                    continue;
                }
                if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                    this._curCircularArray._array[roopIdx].majorChannel = num;
                }
                roopIdx += 1;
                if (roopIdx > (this._curCircularArray.length() - 1)) {
                    roopIdx = 0;
                }
                num += 1;
            }
            for (i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
            this.appendChList(true);
            this._chList.setItemFocus(currentFocus);
            for (i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
            this._selNum = 0;
            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
            this._currentState = 'none';
        };
        CPgEditChannels.prototype.updateChListDown = function () {
            if (this._allNum === 0) {
                return;
            }
            this._curCircularArray.inc();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.inc();
            }
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 7));
            //this._chList.$.remove();
            this.appendChList(false);
            this._chList.setItemFocus(7);
            this._chList.on();
        };
        CPgEditChannels.prototype.updateChListUp = function () {
            if (this._allNum === 0) {
                return;
            }
            this._curCircularArray.dec();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.dec();
            }
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
            //this._chList.$.remove();
            this.appendChList(true);
        };
        CPgEditChannels.prototype.getChlist = function () {
            var retObj;
            retObj = hx.svc.CH_LIST.getChlist({
                group: 'TV'
            });
            this._tvList = retObj.chList;
            retObj = hx.svc.CH_LIST.getChlist({
                group: 'RADIO'
            });
            this._radioList = retObj.chList;
            if (undefined !== this._chType) {
                this.getSrvlist(this._chType);
            }
        };
        CPgEditChannels.prototype.getSrvlist = function (aChType) {
            var _this = this;
            var svcList;
            var ulOrderingNumber = 0;
            if ((hx.config.useDefaultChannels === 1) && (ENV.listModel.HMS1000S === ENV.model)) {
                ulOrderingNumber = 99;
            }
            else {
                ulOrderingNumber = 0;
            }
            var _makeCircularArray = function (svcList, type) {
                var i, svcListLength = svcList.length, _svcList = [], lastLcnOfLcnList = 99;
                for (i = 0; i < svcListLength; i += 1) {
                    if (hx.config.useLcnUpdate) {
                        if (svcList[i].svcSection === _this.SVC_SECTION.SECTION_LCN) {
                            if (svcList[i].majorChannel > lastLcnOfLcnList) {
                                lastLcnOfLcnList = svcList[i].majorChannel;
                            }
                        }
                        if (svcList[i].svcSection === _this.SVC_SECTION.SECTION_NONE_LCN) {
                            insertWrappingChObj(svcList[i], _svcList);
                        }
                    }
                    else {
                        if (svcList[i].majorChannel > ulOrderingNumber) {
                            insertWrappingChObj(svcList[i], _svcList);
                        }
                    }
                }
                switch (type) {
                    case 'TV':
                        if (hx.config.useLcnUpdate) {
                            _this._lastMajorChannelOfTvLcnList = lastLcnOfLcnList;
                        }
                        _this._circularTVArray = new hx.il.circularArray(_svcList);
                        break;
                    case 'RADIO':
                        if (hx.config.useLcnUpdate) {
                            _this._lastMajorChannelOfRadioLcnList = lastLcnOfLcnList;
                        }
                        _this._circularRADIOArray = new hx.il.circularArray(_svcList);
                        break;
                    case 'TVBACKUP':
                        _this._circularTVArrayBackup = new hx.il.circularArray(_svcList);
                        break;
                    case 'RADIOBACKUP':
                        _this._circularRADIOArrayBackup = new hx.il.circularArray(_svcList);
                        break;
                }
            };
            var insertWrappingChObj = function (chObj, _svcList) {
                var wrapChObj = {
                    ccid: '',
                    select: false,
                    manualBlock: false,
                    majorChannel: 0,
                    isHD: false,
                    removed: false,
                    name: '',
                    casInfo: '',
                    isNameChange: false,
                    isBlockChange: false
                };
                wrapChObj.ccid = chObj.ccid;
                wrapChObj.select = chObj.select;
                wrapChObj.manualBlock = chObj.manualBlock;
                wrapChObj.majorChannel = chObj.majorChannel;
                wrapChObj.isHD = chObj.isHD;
                wrapChObj.name = chObj.name;
                wrapChObj.isNameChange = false;
                wrapChObj.isBlockChange = false;
                wrapChObj.casInfo = _this.getCasInformation(chObj);
                _svcList.push(wrapChObj);
            };
            _makeCircularArray(this._tvList, 'TV');
            _makeCircularArray(this._radioList, 'RADIO');
            _makeCircularArray(this._tvList, 'TVBACKUP');
            _makeCircularArray(this._radioList, 'RADIOBACKUP');
            switch (aChType) {
                case 'TV':
                    this._curCircularArray = this._circularTVArray;
                    this._allNum = this._circularTVArray.length();
                    break;
                case 'RADIO':
                    this._curCircularArray = this._circularRADIOArray;
                    this._allNum = this._circularRADIOArray.length();
                    break;
                default:
                    break;
            }
        };
        CPgEditChannels.prototype.appendChList = function (aIsListOn) {
            this._chList = CPgEditChannels.chListMenu(this._currentList);
            var $ulChList = $('#set .chItem ul');
            if ($ulChList.length >= 1) {
                $ulChList.detach();
            }
            this._hgroup.$.after(this._chList.$);
            if (this._currentList.length !== 0) {
                this.append(this._chList);
                if (aIsListOn) {
                    this._chList.on();
                }
            }
        };
        CPgEditChannels.prototype.makeCurrentShowList = function (aChlist, aMoveType) {
            var chArray = [], chInfo, chTempInfo, chLength;
            var notcheckPoint = true, i;
            chArray.length = 0;
            chLength = aChlist.length;
            chTempInfo = {
                chName: '',
                manualBlock: false,
                hd: false,
                cas: false,
                select: false
            };
            switch (aMoveType) {
                case 'multiFirst':
                    chTempInfo.chName = '';
                    this._currentMovePoint = 0;
                    chLength += 1;
                    break;
                case 'multiSecond':
                    chTempInfo.chName = '';
                    this._currentMovePoint = 1;
                    chLength += 1;
                    break;
                case 'multiThird':
                    chTempInfo.chName = '';
                    this._currentMovePoint = 2;
                    chLength += 1;
                    break;
                case 'multiFourth':
                    chTempInfo.chName = '';
                    this._currentMovePoint = 3;
                    chLength += 1;
                    break;
                case 'multiFive':
                    chTempInfo.chName = '';
                    this._currentMovePoint = 4;
                    chLength += 1;
                    break;
                case 'multiSix':
                    chTempInfo.chName = '';
                    this._currentMovePoint = 5;
                    chLength += 1;
                    break;
                case 'multiSeven':
                    chTempInfo.chName = '';
                    this._currentMovePoint = 6;
                    chLength += 1;
                    break;
                case 'multiEight':
                    chTempInfo.chName = '';
                    this._currentMovePoint = 7;
                    chLength += 1;
                    break;
            }
            for (i = 0; i < chLength; i += 1) {
                if (i === this._currentMovePoint && notcheckPoint) {
                    chArray.push(chTempInfo);
                    i -= 1;
                    notcheckPoint = false;
                }
                else {
                    if (aChlist.length === i) {
                        continue;
                    }
                    chInfo = {
                        chName: '',
                        manualBlock: false,
                        hd: false,
                        cas: false,
                        select: false
                    };
                    chInfo.chName = aChlist[i].majorChannel + '\t\t\t' + aChlist[i].name;
                    chInfo.manualBlock = aChlist[i].manualBlock;
                    chInfo.hd = aChlist[i].isHD;
                    chInfo.select = aChlist[i].select;
                    chInfo.cas = aChlist[i].casInfo;
                    chArray.push(chInfo);
                }
            }
            return chArray;
        };
        CPgEditChannels.prototype.getCasInformation = function (channel) {
            var i, casArr = [], length, casIDs;
            if (channel.ccid) {
                if (!!channel.casIDs) {
                    length = channel.casIDs.length;
                    for (i = 0; i < length; i += 1) {
                        casArr.push(channel.casIDs[i]);
                    }
                }
            }
            if (casArr.length > 0) {
                casIDs = 'cas ';
                if (casArr.length > 1) {
                    casIDs += 'multi';
                }
                else {
                    for (i = 0, length = casArr.length; i < length; i += 1) {
                        if (casArr[i] === this._strCasID.indexOf('CAS_NAGRA')) {
                            casIDs += 'na ';
                        }
                        else if (casArr[i] === this._strCasID.indexOf('CAS_IRDETO')) {
                            casIDs += 'ir ';
                        }
                        else if (casArr[i] === this._strCasID.indexOf('CAS_NDS')) {
                            casIDs += 'nd ';
                        }
                        else if (casArr[i] === this._strCasID.indexOf('CAS_VIACCESS')) {
                            casIDs += 'va ';
                        }
                        else if (casArr[i] === this._strCasID.indexOf('CAS_CONAX')) {
                            casIDs += 'cx ';
                        }
                        else if (casArr[i] === this._strCasID.indexOf('CAS_MEDIAGUARD')) {
                            casIDs += 'mg ';
                        }
                        else if (casArr[i] === this._strCasID.indexOf('CAS_CRYPTOWORKS')) {
                            casIDs += 'cr ';
                        }
                    }
                }
            }
            return casIDs;
        };
        CPgEditChannels.targetPage = ['CPgSettings', 'CPgLiveController'];
        CPgEditChannels.chListMenu = function (aParam) {
            var chListMenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': ''
                },
                data: aParam,
                editChList: true,
                count: aParam.length,
                fx: function (i, o) {
                    o.html(this.data[i].chName).append($('<em>', {
                        'class': 'ico'
                    }));
                    if (this.data[i].select) {
                        o.addClass('sel');
                    }
                    if (this.data[i].manualBlock) {
                        o.find('em').append($('<span>', {
                            'class': 'lock'
                        }));
                    }
                    if (this.data[i].hd) {
                        o.find('em').append($('<span>', {
                            'class': 'hd'
                        }));
                    }
                    if (this.data[i].cas) {
                        o.find('em').append($('<span>', {
                            'class': this.data[i].cas
                        }));
                    }
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return chListMenu;
        };
        CPgEditChannels.optListMenu = function (aParam, aParam2) {
            var optListMenu;
            optListMenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': aParam2
                },
                data: aParam,
                count: aParam.length,
                fx: function (i, o) {
                    o.html(hx.l(this.data[i]));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return optListMenu;
        };
        CPgEditChannels.hgroup = function (aParam, aParam2) {
            var hgroup = hx.al.component('hgroup', {
                selectItem: aParam,
                allItem: aParam2
            });
            return hgroup;
        };
        return CPgEditChannels;
    })(__KERNEL_SETTINGS__);
    return CPgEditChannels;
});
