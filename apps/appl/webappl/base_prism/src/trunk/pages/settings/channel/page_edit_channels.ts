///<reference path="../../../def/sugar.d.ts" />
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx;
// file name : pages/settings/channel/edit_channels.js
/*jslint sloppy:true, nomen:true, vars:true */
class CPgEditChannels extends __KERNEL_SETTINGS__{
    static targetPage = ['CPgSettings', 'CPgLiveController'];
    static chListMenu = function(aParam) {
        var chListMenu : any = hx.al.component('settingsList', {
            itemAttr : {},
            attr : {
                'class' : ''
            },
            data : aParam,
            editChList : true,
            count : aParam.length,

            fx : function (i, o) {
                o.html(this.data[i].chName).append($('<em>', {
                    'class' : 'ico'
                }));

                if (this.data[i].select) {
                    o.addClass('sel');
                }
                if (this.data[i].manualBlock) {
                    o.find('em').append($('<span>', {
                        'class' : 'lock'
                    }));
                }
                if (this.data[i].hd) {
                    o.find('em').append($('<span>', {
                        'class' : 'hd'
                    }));
                }
                if (this.data[i].cas) {
                    o.find('em').append($('<span>', {
                        'class' : this.data[i].cas
                    }));
                }
            },
            keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return chListMenu;
    };
    static optListMenu = function(aParam, aParam2) {
        var optListMenu;

        optListMenu = hx.al.component('settingsList', {
            itemAttr : {},
            attr : {
                'class' : aParam2
            },
            data : aParam,
            count : aParam.length,
            fx : function (i, o) {
                o.html(hx.l(this.data[i]));
            },
            keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return optListMenu;
    };
    static hgroup = function(aParam, aParam2) {
        var hgroup = hx.al.component('hgroup', {
            selectItem : aParam,
            allItem : aParam2
        });
        return hgroup;
    };
    _contextListMap = [['LOC_DELETE_ID', 'LOC_LOCK_ID', 'LOC_RENAME_ID', 'LOC_CHANGE_GROUP_ID', 'LOC_SELECT_ALL_ID', 'LOC_DESELECT_ALL_ID'], ['LOC_TV_ID', 'LOC_RADIO_ID']];
    aniConfig = {
        fadeIn : {
            start : {
                'opacity' : 0
            },
            end : {
                'opacity' : 1
            }
        },
        channelMenu : {
            fadeIn : {
                start : {
                    'opacity' : 0,
                    'top' : '20px'
                },
                end : {
                    'opacity' : 1,
                    'top' : 0
                }
            },
            fadeOut : {
                start : {
                    'opacity' : 1,
                    'top' : 0
                },
                end : {
                    'opacity' : 0,
                    'top' : '20px'
                }
            }
        }
    };


    SVC_SECTION : any = {
        SECTION_DEFAULT : 1,
        SECTION_LCN : 2,
        SECTION_NONE_LCN : 16
    };

    confirm : any;
    progressMsg : any;

    _strCasID : any = ['CAS_FTA', 'CAS_NAGRA', 'CAS_IRDETO', 'CAS_NDS', 'CAS_VIACCESS', 'CAS_CONAX', 'CAS_MEDIAGUARD', 'CAS_CRYPTOWORKS'];
    _circularTVArray : any = [];
    _circularRADIOArray : any = [];
    _circularTVArrayBackup : any = [];
    _circularRADIOArrayBackup : any = [];
    _curCircularArray : any = [];
    _availableGroup : boolean = false;
    _currentList : any = [];
    _currentMoveItemList : any = [];
    _moveBackupList : any = [];
    _moveBackupFocus : number = 0;
    _svcList : any = [];
    _allNum : number = 0;
    _selNum : number = 0;
    _currentState : string = 'none';
    _currentMovePoint : number = 100;
    _isChange : boolean = false;
    _recentDataList : any = [];
    _isChangePageInSetting : boolean = false;

    _chList : any;
    _chType : string;
    _optList : any;
    _hgroup : any;
    _islock : boolean;
    _$opt : any;
    _optGroupList : any;
    _ismoveChannel : boolean;
    _lastMajorChannelOfRadioLcnList : number;
    _lastMajorChannelOfTvLcnList : number;
    _tvList : any;
    _radioList : any;
    _isAvBlock : boolean;
    _currentChObj : any;
    _$chItem : any;
    _$optTit : any;

    constructor() {
        super();
    }

    create () {
        super.create();
        var ccid : any;
        var i : number, currentChIdx : number = 0;
        var chTypeStr : string = '';

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

        var set_title : any = $('#set .title');
        if (set_title[0]) {
            set_title[0].children[0].innerHTML = hx.l('LOC_EDIT_CHANNEL_LIST_ID');
        }
        set_title.ani({
            style : this.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });

        this.progressMsg = hx.msg('progress', {
            text : hx.l('LOC_LOADING_DOT_ID'),
            _use_backkey : false,
            auto_close : false
            //timeout : 5000,
        });

        $.extend(this, {
            _$chItem : $('<article>', {
                'class' : 'chItem'
            }),

            _$opt : $('<section>', {
                'id' : 'opt'
            }),
            _$optTit : $('<p>', {
                'class' : 'tit',
                'html' : hx.l('LOC_EDIT_ID')
            }),
            _optList : CPgEditChannels.optListMenu(this._contextListMap[0], 'menu'),
            _optGroupList : CPgEditChannels.optListMenu(this._contextListMap[1], 'menu')
        });
        hx.al.compose(this).append(this._optList);

        //animation
        var $target = $('#DIA .vaTop');
        hx.il.ani.action({
            type : 'in',
            targets : $target,
            cb : () => {
                $('#set').append(this._$chItem);
                $('#set').after(this._$opt.append(this._$optTit).append(this._optList.$));

                this._recentDataList.length = 0;
                var recentHistory = hx.svc.WEB_STORAGE.getRecentItems();
                if (recentHistory) {
                    if (recentHistory.recentData) {
                        this._recentDataList = recentHistory.recentData.split(/\s*,\s*/);
                    }
                }

                this._isChangePageInSetting = false;

                //this._chList.on();
                var chObj = hx.svc.CH_LIST.getCurrentChannel();
                this._currentChObj = chObj;

                if (chObj) {
                    if (chObj.result) {
                        if (chObj.channel) {
                            ccid = chObj.channel.ccid;
                            if (chObj.group) {
                                this._chType = chObj.group;
                                if (this._chType === 'TV' || this._chType === 'HDTV') {
                                    this._chType = 'TV';
                                } else if (this._chType === 'RADIO') {
                                    this._chType = 'RADIO';
                                } else {
                                    this._chType = (chObj.channel.channelType === 1) ? 'RADIO' : 'TV';
                                }
                            } else {//if(chObj.group)
                                this._chType = 'TV';
                            }
                        } else {//if(chObj.channel)
                            ccid = undefined;
                            this._chType = 'TV';
                        }
                    } else {//if(chObj.result)
                        ccid = undefined;
                        this._chType = 'TV';
                    }
                } else {//if(chObj)
                    ccid = undefined;
                    this._chType = 'TV';
                }

                this.getChlist();
                //getChlist with group
                this._hgroup = CPgEditChannels.hgroup(this._selNum, this._allNum);

                if (this._chType === 'TV') {
                    chTypeStr = hx.l('LOC_TV_ID');
                } else if (this._chType === 'RADIO') {
                    chTypeStr = hx.l('LOC_RADIO_ID');
                }
                this._hgroup.$.find('h1').html(chTypeStr);

                for ( i = 0; i < this._curCircularArray._array.length; i += 1) {//find currentChannel Idx
                    if (this._curCircularArray._array[i].ccid === ccid) {
                        currentChIdx = i;
                        break;
                    }
                }

                for ( i = 0; i < currentChIdx; i += 1) {//goto currentChannel Idx
                    this._curCircularArray.inc();
                    while (this._curCircularArray.curItem().removed === true) {
                        this._curCircularArray.inc();
                    }
                }

                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                this._chList = CPgEditChannels.chListMenu(this._currentList);
                this._$chItem.append(this._hgroup.$).append(this._chList.$);

                hx.msg.close('progress', this.progressMsg);

                $('#set .chItem').ani({
                    style : this.aniConfig.channelMenu.fadeIn,
                    trans : {
                        dur : 200,
                        delay : 400
                    },
                    cb : () => {
                        this.append(this._chList);

                        var dsrvRecordings = hx.svc.RECORDINGS;
                        dsrvRecordings.addEventCb('RecordingItemAdded', this, (param) => {
                            if (this.progressMsg) {
                                hx.msg.close('progress', this.progressMsg);
                            }
                            if (this.confirm) {
                                hx.msg.close('confirm', this.confirm);
                            }
                            hx.svc.SETTING_UTIL.setFrontPanelString({
                                text : ''
                            });
                            this.sendEventToEntity({
                                alEvent : 'alChangePage',
                                target : 'CPgLiveController',
                                me : this
                            });
                        });

                        this._chList.on();
                    }
                });
            }
        });
    }
    destroy () {
        super.destroy();
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
        } else {
            this._isChangePageInSetting = false;
        }
    }
    _changeToAvailCh (group) {
        // tune to available channel
        if (this._isAvBlock) {
            hx.svc.AV_CTRL.availableChannel({
                group: group
            });
        }
    }
    _goExit (aTargetPage : string) {
        var nextTargetPage : string;
        if (aTargetPage === 'setting') {
            this._isChangePageInSetting = true;
            nextTargetPage = CPgEditChannels.targetPage[0];
            hx.al.decompose(this);
            $('#set .chItem').ani({
                style : this.aniConfig.channelMenu.fadeOut,
                trans : {
                    dur : 200,
                    delay : 100
                },
                cb : ()=> {
                    this.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : nextTargetPage,
                        me : this
                    });
                }
            });
        } else if (aTargetPage === 'live') {
            hx.svc.SETTING_UTIL.setFrontPanelString({
                text : ''
            });
            nextTargetPage = CPgEditChannels.targetPage[1];
            this.sendEventToEntity({
                alEvent : 'alChangePage',
                target : nextTargetPage,
                me : this
            });
        }
    }
    _on_alClicked (aParam) {
        var data = aParam.alData, target = data.me, i : number, currentFocus : number, bConsumed : boolean = false;
        var num : number = 0, smallestLcn : number, smallestLcnIdx : number, roopIdx : number, roopCount : number;

        switch (target) {
            case this._chList:
                bConsumed = true;
                if (this._allNum === 0) {//have no item
                    return;
                }

                if (this._currentState === 'none') {
                    this._select_unselect_Item(data.index);
                } else if (this._currentState === 'mov') {
                    this._isChange = true;
                    if (hx.config.useFavouriteCollection) {
                        hx.svc.CH_LIST.moveChListItem({
                            favtype : this._chType,
                            ccid : this._currentMoveItemList[0].ccid,
                            insertTargetccid : this._curCircularArray.getNextItem().ccid
                        });
                    }

                    this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._currentMoveItemList[0]);
                    //insert
                    currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;

                    smallestLcn = this._curCircularArray._array[0].majorChannel;
                    //find smallestLcnIdx
                    smallestLcnIdx = 0;
                    for ( i = 1; i < this._curCircularArray.length(); i += 1) {
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
                        for ( i = 1; i < this._curCircularArray.length(); i += 1) {
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
                        for ( i = 0; i < this._curCircularArray.length(); i += 1) {
                            if (this._currentMoveItemList[0].ccid === this._curCircularArray._array[i].ccid) {
                                smallestLcnIdx = i;
                                break;
                            }
                        }
                    }
                    if (ENV.listModel.IR4000HD === ENV.model) {
                        if (this._chType === 'TV') {
                            num = 1;
                        } else if (this._chType === 'RADIO') {
                            num = 5000;
                        }
                    } else {
                        if (hx.config.useLcnUpdate) {
                            if (this._chType === 'TV') {
                                num = (this._lastMajorChannelOfTvLcnList + 1);
                            } else if (this._chType === 'RADIO') {
                                num = (this._lastMajorChannelOfRadioLcnList + 1);
                            }
                        } else {
                            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                                num = 100;
                            }
                        }
                    }
                    roopIdx = smallestLcnIdx;
                    roopCount = this._curCircularArray.length();
                    for ( i = 0; i < roopCount; i += 1) {
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

                    for ( i = 0; i < currentFocus; i += 1) {
                        this._curCircularArray.dec();
                        while (this._curCircularArray.curItem().removed === true) {
                            this._curCircularArray.dec();
                        }
                    }

                    //this._chList.$.remove();
                    this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
                    this.appendChList(true);
                    this._chList.setItemFocus(currentFocus);

                    for ( i = 0; i < currentFocus; i += 1) {
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
                } else if (this._currentState === 'multimov') {
                    this._isChange = true;

                    if (hx.config.useFavouriteCollection) {
                        var targetChIndex = hx.svc.CH_LIST.moveChListItem({
                            favtype : this._chType,
                            ccid : this._currentMoveItemList[0].ccid,
                            insertTargetccid : this._curCircularArray.getNextItem().ccid
                        });

                        for ( i = 1; i < this._currentMoveItemList.length; i += 1) {
                            if (hx.config.useFavouriteCollection) {
                                hx.svc.CH_LIST.moveChListItemWithTargetIdx({
                                    favtype : this._chType,
                                    ccid : this._currentMoveItemList[i].ccid,
                                    targetIdx : targetChIndex,
                                    offset : i
                                });
                            }
                        }
                    }

                    this._currentMoveItemList[0].majorChannel = 100000;
                    //remove previous number
                    this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._currentMoveItemList[0]);
                    //insert

                    for ( i = 1; i < this._currentMoveItemList.length; i += 1) {
                        this._currentMoveItemList[i].majorChannel = 100000;
                        //remove previous number
                        this._curCircularArray._array.splice(this._curCircularArray._idxCur + i, 0, this._currentMoveItemList[i]);
                        //insert
                    }

                    currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;

                    var lastMoveItemIdx;
                    //find smallestLcnIdx
                    smallestLcn = this._curCircularArray._array[0].majorChannel;
                    smallestLcnIdx = 0;
                    for ( i = 1; i < this._curCircularArray.length(); i += 1) {
                        if (smallestLcn > this._curCircularArray._array[i].majorChannel) {
                            smallestLcn = this._curCircularArray._array[i].majorChannel;
                            smallestLcnIdx = i;
                        }
                    }

                    for ( i = 0; i < this._curCircularArray.length(); i += 1) {
                        if (this._currentMoveItemList[this._currentMoveItemList.length - 1].ccid === this._curCircularArray._array[i].ccid) {
                            lastMoveItemIdx = i;
                            break;
                        }
                    }

                    //if insertTargetIdx is just before smallestLcnIdx, set smallestLcnIdx with moveItem
                    if (this._curCircularArray.getNextItemWithCurIdx(lastMoveItemIdx).ccid === this._curCircularArray._array[smallestLcnIdx].ccid) {
                        for ( i = 0; i < this._curCircularArray.length(); i += 1) {
                            if (this._currentMoveItemList[0].ccid === this._curCircularArray._array[i].ccid) {
                                smallestLcnIdx = i;
                                break;
                            }
                        }
                    }

                    if (ENV.listModel.IR4000HD === ENV.model) {
                        if (this._chType === 'TV') {
                            num = 1;
                        } else if (this._chType === 'RADIO') {
                            num = 5000;
                        }
                    } else {
                        if (hx.config.useLcnUpdate) {
                            if (this._chType === 'TV') {
                                num = (this._lastMajorChannelOfTvLcnList + 1);
                            } else if (this._chType === 'RADIO') {
                                num = (this._lastMajorChannelOfRadioLcnList + 1);
                            }
                        } else {
                            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                                num = 100;
                            }
                        }
                    }
                    roopIdx = smallestLcnIdx;
                    roopCount = this._curCircularArray.length();
                    for ( i = 0; i < roopCount; i += 1) {
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

                    for ( i = 0; i < currentFocus; i += 1) {
                        this._curCircularArray.dec();
                        while (this._curCircularArray.curItem().removed === true) {
                            this._curCircularArray.dec();
                        }
                    }

                    //this._chList.$.remove();
                    this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
                    this.appendChList(true);
                    this._chList.setItemFocus(currentFocus);

                    for ( i = 0; i < currentFocus; i += 1) {
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
                switch(this._optList.$.children().eq(data.index).text()) {
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
    }
    _on_alScrollup (aParam) : boolean {
        var data = aParam.alData, listItem : any, itemList : any;
        var currentFocus : number, bConsumed : boolean = false;
        var curItem : any, curIndex : number;
        if (data) {
            var target = data.me;

            switch (target) {
                case this._chList:
                    bConsumed = true;
                    if (this._currentState === 'none') {
                        if (data.index === 0 && this._allNum > 8) {
                            this.updateChListUp();
                        } else {
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
                    } else if (this._currentState === 'mov') {
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
                        } else {
                            if (data.index === 0) {
                                itemList = this._chList.$.children('li');
                                listItem = itemList.eq(0)[0].innerHTML;
                                itemList.eq(0)[0].innerHTML = itemList.eq(this._allNum - 1)[0].innerHTML;
                                itemList.eq(this._allNum - 1)[0].innerHTML = listItem;
                                this._chList.setItemFocus(itemList.length - 1);
                            } else {
                                currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                                itemList = this._chList.$.children('li');
                                listItem = itemList.eq(currentFocus)[0].innerHTML;
                                itemList.eq(currentFocus)[0].innerHTML = itemList.eq(currentFocus+1)[0].innerHTML;
                                itemList.eq(currentFocus+1)[0].innerHTML = listItem;
                                this._chList.setItemFocus(currentFocus);
                            }
                            this._curCircularArray.dec();
                            while (this._curCircularArray.curItem().removed === true) {
                                this._curCircularArray.dec();
                            }
                        }
                        this._chList.getFocusedItem().addClass('mov');
                    } else if (this._currentState === 'multimov') {
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
                        } else {
                            if (data.index === 0) {
                                itemList = this._chList.$.children('li');
                                listItem = itemList.eq(0)[0].innerHTML;
                                itemList.eq(0)[0].innerHTML = itemList.eq(this._allNum-this._selNum)[0].innerHTML;
                                itemList.eq(this._allNum-this._selNum)[0].innerHTML = listItem;
                                this._chList.setItemFocus(itemList.length - 1);
                            } else {
                                currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                                itemList = this._chList.$.children('li');
                                listItem = itemList.eq(currentFocus)[0].innerHTML;
                                itemList.eq(currentFocus)[0].innerHTML = itemList.eq(currentFocus+1)[0].innerHTML;
                                itemList.eq(currentFocus+1)[0].innerHTML = listItem;
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
    }
    _on_alScrolldown (aParam) : boolean {
        var data : any = aParam.alData, bConsumed : boolean = false;
        var itemList : any, listItem : any, currentFocus : number;
        if (data) {
            var target = data.me;
            switch (target) {
                case this._chList:
                    bConsumed = true;
                    if (this._currentState === 'none') {
                        if (data.index === 7 && this._allNum > 8) {
                            this.updateChListDown();
                        } else {
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
                    } else if (this._currentState === 'mov') {
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
                        } else {
                            if (data.index === this._allNum - 1) {
                                itemList = this._chList.$.children('li');
                                listItem = itemList.eq(this._allNum - 1)[0].innerHTML;
                                itemList.eq(this._allNum - 1)[0].innerHTML = itemList.eq(0)[0].innerHTML;
                                itemList.eq(0)[0].innerHTML = listItem;
                                this._chList.setItemFocus(0);
                            } else {
                                currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                                itemList = this._chList.$.children('li');
                                listItem = itemList.eq(currentFocus)[0].innerHTML;
                                itemList.eq(currentFocus)[0].innerHTML = itemList.eq(currentFocus-1)[0].innerHTML;
                                itemList.eq(currentFocus-1)[0].innerHTML = listItem;
                                this._chList.setItemFocus(currentFocus);
                            }
                            this._curCircularArray.inc();
                            while (this._curCircularArray.curItem().removed === true) {
                                this._curCircularArray.inc();
                            }
                        }
                        this._chList.getFocusedItem().addClass('mov');
                    } else if (this._currentState === 'multimov') {
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
                        } else {
                            if (data.index === (this._allNum - this._selNum)) {
                                itemList = this._chList.$.children('li');
                                listItem = itemList.eq(this._allNum-this._selNum)[0].innerHTML;
                                itemList.eq(this._allNum-this._selNum)[0].innerHTML = itemList.eq(0)[0].innerHTML;
                                itemList.eq(0)[0].innerHTML = listItem;
                                this._chList.setItemFocus(0);
                            } else {
                                currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
                                itemList = this._chList.$.children('li');
                                listItem = itemList.eq(currentFocus)[0].innerHTML;
                                itemList.eq(currentFocus)[0].innerHTML = itemList.eq(currentFocus-1)[0].innerHTML;
                                itemList.eq(currentFocus-1)[0].innerHTML = listItem;
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
    }
    _on_alExit (aParam) : boolean {
        var that = this, data = aParam.alData, bConsumed : boolean = false, i : number;

        var fn = function(_status) {
            if (_status === 'key_power' || _status === hx.l('LOC_NO_ID')) {
                if (that._isChange) {
                    hx.svc.CH_LIST.forceChListUpdate();
                }
                that._gotoSettingsPage();
            } else if (_status === hx.l('LOC_YES_ID')) {
                that._updateEditedChList('setting');
            } else if (_status === 'key_back' || _status === 'key_esc') {
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

                        for ( i = 0; i < this._moveBackupFocus; i += 1) {
                            this._curCircularArray.dec();
                            while (this._curCircularArray.curItem().removed === true) {
                                this._curCircularArray.dec();
                            }
                        }
                        //this._chList.$.remove();
                        this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
                        this.appendChList(true);
                        this._chList.setItemFocus(this._moveBackupFocus);
                        for ( i = 0; i < this._moveBackupFocus; i += 1) {
                            this._curCircularArray.inc();
                            while (this._curCircularArray.curItem().removed === true) {
                                this._curCircularArray.inc();
                            }
                        }

                        this._selNum = 0;
                        this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
                        this._currentState = 'none';
                    } else if (this._isChange) {
                        this.confirm = hx.msg('confirm', {
                            text : hx.l('LOC_SAVE_THE_CHANGE_ID'),
                            auto_close : false,
                            //timeout : 5000,
                            btn_title_arr : [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                            dia_class : 'osd',
                            callback_fn : fn
                        });
                    } else {
                        this._gotoSettingsPage();
                    }
                    break;
                case this._optList:
                    bConsumed = true;
                    if (this._$opt.hasClass('on')) {
                        this._$opt.removeClass('on');
                        this._chList.on();
                    } else {//no channel
                        if (this._isChange) {
                            this.confirm = hx.msg('confirm', {
                                text : hx.l('LOC_SAVE_THE_CHANGE_ID'),
                                auto_close : false,
                                //timeout : 5000,
                                btn_title_arr : [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                                dia_class : 'osd',
                                callback_fn : fn
                            });
                        } else {
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
                    } else {
                        if (this._isChange) {
                            this.confirm = hx.msg('confirm', {
                                text : hx.l('LOC_SAVE_THE_CHANGE_ID'),
                                auto_close : false,
                                //timeout : 5000,
                                btn_title_arr : [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                                dia_class : 'osd',
                                callback_fn : fn
                            });
                        } else {
                            this._gotoSettingsPage();
                        }
                    }
                    break;
            }
        }
        return bConsumed;
    }
    _on_alKey (aParam) : boolean {
        var that = this, data = aParam.alData, i : number, bConsumed : boolean = false, currentFocus : number, curItem;
        if (data) {
            var target = data.me;
        }

        var fn = function(_status) {
            if (_status === 'key_power' || _status === hx.l('LOC_NO_ID')) {
                if (that._isChange) {
                    hx.svc.CH_LIST.forceChListUpdate();
                }
                that._goExit('live');
            } else if (_status === hx.l('LOC_YES_ID')) {
                that._updateEditedChList('live');
            } else if (_status === 'key_back' || _status === 'key_esc') {
                that._chList.on();
            }
        };

        switch (aParam.alKey) {
            case hx.key.KEY_ESCAPE:
                if (data) {
                    bConsumed = true;
                    if (this._isChange) {
                        this.confirm = hx.msg('confirm', {
                            text : hx.l('LOC_SAVE_THE_CHANGE_ID'),
                            auto_close : false,
                            //timeout : 5000,
                            btn_title_arr : [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                            dia_class : 'osd',
                            callback_fn : fn
                        });
                    } else {
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
                    } else {
                        if (target === this._chList) {
                            this._optList.$.remove();
                            if (this._selNum >= this._allNum) {// After selecting all, move and select all items are blocked. And
                                // after deleting and select all times then this._selNum > this._allNum
                                this._optList = CPgEditChannels.optListMenu(['LOC_DELETE_ID', 'LOC_LOCK_ID', 'LOC_CHANGE_GROUP_ID', 'LOC_DESELECT_ALL_ID'], 'menu');
                            } else if ((this._selNum > 1) && !((this._currentList === 'mov') || (this._currentState === 'multimov'))) {
                                if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT)
                                    this._optList = CPgEditChannels.optListMenu(['LOC_DELETE_ID', 'LOC_LOCK_ID', 'LOC_CHANGE_GROUP_ID', 'LOC_SELECT_ALL_ID', 'LOC_DESELECT_ALL_ID'], 'menu');
                                else
                                    this._optList = CPgEditChannels.optListMenu(['LOC_DELETE_ID', 'LOC_LOCK_ID', 'LOC_MOVE_ID', 'LOC_CHANGE_GROUP_ID', 'LOC_SELECT_ALL_ID', 'LOC_DESELECT_ALL_ID'], 'menu');
                            } else if ((this._currentState === 'mov') || (this._currentState === 'multimov')) {
                                this._optList = CPgEditChannels.optListMenu(['LOC_OK_ID', 'LOC_CANCEL2_ID'], 'menu');
                            } else {
                                this._optList = CPgEditChannels.optListMenu(this._contextListMap[0], 'menu');
                            }
                            this._$opt.append(this._optList.$);

                            this._islock = this._checkLockItem();
                            if (this._islock) {
                                $('#opt .menu').children('li').eq(1).html(hx.l('LOC_UNLOCK_ID'));
                            } else if (!((this._currentState === 'mov') || (this._currentState === 'multimov'))) {
                                $('#opt .menu').children('li').eq(1).html(hx.l('LOC_LOCK_ID'));
                            }
                            this.append(this._optList);
                            this._$opt.addClass('on');
                            this._optList.on();
                            this._optList.setItemFocus(0);
                        } else {
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
                                for ( i = 0; i < (currentFocus + 8); i += 1) {
                                    this._curCircularArray.dec();
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.dec();
                                    }
                                }

                                //this._chList.$.remove();
                                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                this.appendChList(true);

                                for ( i = 0; i < currentFocus; i += 1) {
                                    this._curCircularArray.inc();
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.inc();
                                    }
                                }
                                this._chList.setItemFocus(currentFocus);
                            }
                        } else if (this._currentState === 'mov') {
                            if (this._allNum > 8) {
                                currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;
                                this._chList.$.children('.on').removeClass('on');
                                this._chList.$.children('.mov').removeClass('mov');

                                for ( i = 0; i < 8; i += 1) {//change item
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

                                for ( i = 0; i < currentFocus; i += 1) {
                                    this._curCircularArray.dec();
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.dec();
                                    }
                                }

                                //this._chList.$.remove();
                                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                this.appendChList(true);

                                for ( i = 0; i < currentFocus; i += 1) {
                                    this._curCircularArray.inc();
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.inc();
                                    }
                                }
                                this._chList.setItemFocus(currentFocus);
                                this._chList.getItem(currentFocus).addClass('mov');
                            }
                        } else if (this._currentState === 'multimov') {
                            if ((this._allNum - this._selNum + 1) > 8) {
                                currentFocus = this._chList.$.children('.mov.grp').prevAll('.focusable').length;
                                this._chList.$.children('.on').removeClass('on');
                                this._chList.$.children('.mov.grp').removeClass('mov grp');

                                //change item
                                for ( i = 0; i < 8; i += 1) {//change item
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

                                for ( i = 0; i < currentFocus; i += 1) {
                                    this._curCircularArray.dec();
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.dec();
                                    }
                                }

                                //this._chList.$.remove();
                                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                this.appendChList(true);

                                for ( i = 0; i < currentFocus; i += 1) {
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
                                for ( i = 0; i < (8 - currentFocus); i += 1) {
                                    this._curCircularArray.inc();
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.inc();
                                    }
                                }

                                //this._chList.$.remove();
                                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                this.appendChList(true);

                                for ( i = 0; i < currentFocus; i += 1) {
                                    this._curCircularArray.inc();
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.inc();
                                    }
                                }
                                this._chList.setItemFocus(currentFocus);
                            }
                        } else if (this._currentState === 'mov') {
                            if (this._allNum > 8) {
                                currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;
                                this._chList.$.children('.on').removeClass('on');
                                this._chList.$.children('.mov').removeClass('mov');

                                for ( i = 0; i < 8; i += 1) {//change item
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

                                for ( i = 0; i < currentFocus; i += 1) {
                                    this._curCircularArray.dec();
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.dec();
                                    }
                                }

                                //this._chList.$.remove();
                                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                this.appendChList(true);

                                for ( i = 0; i < currentFocus; i += 1) {
                                    this._curCircularArray.inc();
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.inc();
                                    }
                                }
                                this._chList.setItemFocus(currentFocus);
                                this._chList.getItem(currentFocus).addClass('mov');
                            }
                        } else if (this._currentState === 'multimov') {
                            if ((this._allNum - this._selNum + 1) > 8) {
                                currentFocus = this._chList.$.children('.mov.grp').prevAll('.focusable').length;
                                this._chList.$.children('.on').removeClass('on');
                                this._chList.$.children('.mov.grp').removeClass('mov grp');

                                //change item
                                for ( i = 0; i < 8; i += 1) {//change item
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

                                for ( i = 0; i < currentFocus; i += 1) {
                                    this._curCircularArray.dec();
                                    while (this._curCircularArray.curItem().removed === true) {
                                        this._curCircularArray.dec();
                                    }
                                }

                                //this._chList.$.remove();
                                this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
                                this.appendChList(true);

                                for ( i = 0; i < currentFocus; i += 1) {
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
    }
    _updateEditedChList (aTargetPage) {
        var that = this;
        this.progressMsg = hx.msg('progress', {
            text : hx.l('LOC_SAVING_ID'),
            _use_backkey : false,
            auto_close : false
            //timeout : 5000,
        });

        function _updateChList(cicularChList, type) {
            var idx : number, len_of_list : number, currentItem : any, backupCur : any, backupCircular : any = [];
            var j : number, item : any;
            var deleteChLists : any = [], updatedChLists : any = [];

            backupCircular.length = 0;
            if (type === 'TV') {
                backupCircular = that._circularTVArrayBackup;
            } else if (type === 'RADIO') {
                backupCircular = that._circularRADIOArrayBackup;
            }
            backupCircular.setCur(0);
            cicularChList.setCur(0);

            if (hx.config.useFavouriteCollection) {
                hx.svc.CH_LIST.editChListDoCommit({
                    group : type
                });
            }

            len_of_list = cicularChList.length();
            for ( idx = 0; idx < len_of_list; idx += 1) {
                // GET Current
                currentItem = cicularChList.curItem();
                backupCur = backupCircular.curItem();
                if (currentItem.removed) {

                    var bDeleted = (currentItem.removed) ? true : false;
                    for ( j = 0; j < that._recentDataList.length; j += 1) {
                        if (that._recentDataList[j] === currentItem.ccid) {
                            that.sendEventToEntity({
                                alEvent : 'alMessageToPage',
                                target : 'CPgHubProxy/updateChannel?ccid=' + currentItem.ccid + ',bDeleted=' + bDeleted,
                                me : that
                            });
                        }
                    }
                    item = {
                        group : type,
                        ccid : currentItem.ccid
                    };
                    deleteChLists.push(item);
                    if (that._currentChObj) {
                        if (that._currentChObj.result) {
                            if (that._currentChObj.channel) {
                                if (that._currentChObj.channel.ccid === item.ccid) {
                                    hx.svc.AV_CTRL.stopCurChannel({
                                        stopAV : 1,
                                        stopTsr : 0
                                    });
                                    that._isAvBlock = true;
                                }
                            }
                        }
                    }
                } else if ((currentItem.manualBlock !== backupCur.manualBlock) || //
                    (currentItem.name !== backupCur.name)) {
                    if (currentItem.majorChannel !== backupCur.majorChannel) {
                        that._ismoveChannel = true;
                    }
                    item = {
                        group : type,
                        ccid : currentItem.ccid,
                        isLockChange : currentItem.isBlockChange,
                        lock : currentItem.manualBlock,
                        isNameChange : currentItem.isNameChange,
                        name : currentItem.name,
                        isRemoved : currentItem.removed,
                        majorChannel : currentItem.majorChannel,
                        isLcnChange : that._ismoveChannel
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
                'deleteChLists' : deleteChLists,
                'group' : type
            });
            hx.svc.CH_LIST.updateChannels({
                'updatedChLists' : updatedChLists,
                'group' : type
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
            type : 'in',
            targets : $target,
            cb : () => {
                switch(that._chType) {
                    case 'TV' :
                        that._circularTVArray = that._curCircularArray;
                        break;
                    case 'RADIO' :
                        that._circularRADIOArray = that._curCircularArray;
                        break;
                }
                _updateChList(that._circularTVArray, 'TV');
                _updateChList(that._circularRADIOArray, 'RADIO');

                hx.svc.CH_LIST.forceChListUpdate();
                var dsrvRecordings = hx.svc.RECORDINGS;
                dsrvRecordings.refreshScheduleList();
                setTimeout((time) => {
                    hx.msg.close('progress', that.progressMsg);
                    hx.log("warn", "timer: " + time);
                    this._changeToAvailCh(this._currentChObj.group);
                    if (aTargetPage === 'live') {
                        that._goExit('live');
                    } else if (aTargetPage === 'setting') {
                        that._gotoSettingsPage();
                    }
                }, 1000);
            }
        });
    }
    _gotoSettingsPage () {
        this._goExit('setting');
    }
    _getCircularItem (aLength : number, aOffset? : number) {
        var i, curIndex, showList = [];
        curIndex = this._curCircularArray.cur();

        for ( i = 0; i < aOffset; i += 1) {
            this._curCircularArray.dec();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.dec();
            }
        }

        for ( i = 0; i < this._curCircularArray.length(); i += 1) {
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
    }
    _deleteItem () {
        var i : number, currentFocus : number, changedccidList : any = [], num : number = 0;

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
        } else {
            for ( i = 0; i < this._curCircularArray.length(); i += 1) {//set remove Flag true
                if (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                } else {
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
            for ( i = 0; i < currentFocus; i += 1) {//back to FirstIndexItem to show List
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
        }

        if (ENV.listModel.IR4000HD === ENV.model) {
            if (this._chType === 'TV') {
                num = 1;
            } else if (this._chType === 'RADIO') {
                num = 5000;
            }
        } else {
            if (hx.config.useLcnUpdate) {
                if (this._chType === 'TV') {
                    num = (this._lastMajorChannelOfTvLcnList + 1);
                } else if (this._chType === 'RADIO') {
                    num = (this._lastMajorChannelOfRadioLcnList + 1);
                }
            } else {
                if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                    num = 100;
                }
            }
        }

        for ( i = 0; i < this._curCircularArray.length(); i += 1) {//reArrange MajorChannel
            if (this._curCircularArray._array[i].removed) {
                continue;
            }
            if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                this._curCircularArray._array[i].majorChannel = num;
            }

            num += 1;
        }

        //this._chList.$.remove();

        if (this._allNum !== 0) {//make list to show
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
            this.appendChList(true);
            var chlength : number = this._chList.$.children('li').length;
            if (chlength - 1 < currentFocus) {
                this._chList.setItemFocus(chlength - 1);
            } else {
                this._chList.setItemFocus(currentFocus);
            }
        } else {
            this._currentList.length = 0;
            this.appendChList(true);
        }

        if (this._allNum !== 0) {//go to focusedItem
            for ( i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
        }

        this._selNum = 0;
        this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
    }
    _checkLockItem () {
        var i : number, isLock : boolean = false;

        if (this._selNum === 0 && this._allNum > 0) {
            if (this._curCircularArray.curItem().manualBlock) {
                isLock = true;
            }
        } else {
            for ( i = 0; i < this._curCircularArray.length(); i += 1) {
                if (this._curCircularArray.curItem().select) {
                    if (this._curCircularArray.curItem().manualBlock) {
                        isLock = true;
                    }
                }
                this._curCircularArray.inc();
            }
        }

        return isLock;
    }
    _lockItem () {
        var i : number, changedccidList = [], currentFocus : number;

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
        } else {
            for ( i = 0; i < this._curCircularArray.length(); i += 1) {
                if (this._curCircularArray.curItem().select) {
                    this._curCircularArray.curItem().select = false;
                    this._curCircularArray.curItem().manualBlock = true;
                    this._curCircularArray.curItem().isBlockChange = true;
                    changedccidList.push(this._curCircularArray.curItem().ccid);
                }
                this._curCircularArray.inc();
            }
        }

        for ( i = 0; i < currentFocus; i += 1) {
            this._curCircularArray.dec();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.dec();
            }
        }

        //this._chList.$.remove();
        this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
        this.appendChList(true);
        this._chList.setItemFocus(currentFocus);

        for ( i = 0; i < currentFocus; i += 1) {
            this._curCircularArray.inc();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.inc();
            }
        }

        this._selNum = 0;
        this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
    }
    _unlockItem () {
        var i : number, changedccidList = [], currentFocus : number;

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
        } else {
            for ( i = 0; i < this._curCircularArray.length(); i += 1) {
                if (this._curCircularArray.curItem().select) {
                    this._curCircularArray.curItem().select = false;
                    this._curCircularArray.curItem().manualBlock = false;
                    this._curCircularArray.curItem().isBlockChange = true;
                    changedccidList.push(this._curCircularArray.curItem().ccid);
                }
                this._curCircularArray.inc();
            }
        }

        for ( i = 0; i < currentFocus; i += 1) {
            this._curCircularArray.dec();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.dec();
            }
        }

        //this._chList.$.remove();
        this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
        this.appendChList(true);
        this._chList.setItemFocus(currentFocus);

        for ( i = 0; i < currentFocus; i += 1) {
            this._curCircularArray.inc();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.inc();
            }
        }

        this._selNum = 0;
        this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
    }
    _renameItem () {
        var that = this;
        var i : number, changedName : string, changedccidList : any = [], currentFocus : number;

        if (this._allNum <= 0) {
            return;
        }

        changedccidList.length = 0;
        currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;

        var _setRename : Function = ()=>{
            for ( i = 0; i < currentFocus; i += 1) {//back to FirstIndexItem to show List
                that._curCircularArray.dec();
                while (that._curCircularArray.curItem().removed === true) {
                    that._curCircularArray.dec();
                }
            }

            //that._chList.$.remove();
            that._currentList = that.makeCurrentShowList(that._getCircularItem(8));
            that.appendChList(true);
            that._chList.setItemFocus(currentFocus);

            for ( i = 0; i < currentFocus; i += 1) {//go to focusItem
                that._curCircularArray.inc();
                while (that._curCircularArray.curItem().removed === true) {
                    that._curCircularArray.inc();
                }
            }

            that._selNum = 0;
            that._hgroup.$.find('p').html(that._selNum + ' / ' + that._allNum);
        }

        var passwdTmp : any = hx.msg('passwd', {
            text : hx.l('LOC_ENTER_CHANNEL_NAME_ID'),
            responseText : hx.l('LOC_TYPE_NAME_PRESS_ENTER_ID'),
            wifi : 'Name', //wifi
            string : hx.l('LOC_CHANNEL_NAME_ID'),
            auto_close : false,
            dia_class : 'osd',
            callback_fn : fn
        });

        function fn(_status, _input) {
            if (_status === 'key_back' || _status === 'key_esc') {
                //hx.msg.close('passwd', passwdTmp);
                for ( i = 0; i < that._curCircularArray.length(); i += 1) {//Deselect
                    if (that._curCircularArray.curItem().select) {
                        that._curCircularArray.curItem().select = false;
                    }
                    that._curCircularArray.inc();
                }
                _setRename();
            } else if (_status === hx.l('LOC_OK_ID')) {
                if (_input) {
                    if (_input.password !== "") {
                        that._isChange = true;
                        if (that._selNum === 0 && that._allNum > 0) {
                            changedName = _input.password;
                            that._curCircularArray.curItem().name = changedName;
                            that._curCircularArray.curItem().isNameChange = true;
                            that._curCircularArray.curItem().select = false;
                            changedccidList.push(that._curCircularArray.curItem().ccid);
                        } else {
                            for ( i = 0; i < that._curCircularArray.length(); i += 1) {
                                if (that._curCircularArray.curItem().select) {//set Name that is entered by user
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
                    } else {
                        for ( i = 0; i < that._curCircularArray.length(); i += 1) {//deselect
                            if (that._curCircularArray.curItem().select) {
                                that._curCircularArray.curItem().select = false;
                            }
                            that._curCircularArray.inc();
                        }
                        _setRename();
                    }
                }
            } else if (_status === 'close') {
                for ( i = 0; i < that._curCircularArray.length(); i += 1) {//deselect
                    if (that._curCircularArray.curItem().select) {
                        that._curCircularArray.curItem().select = false;
                    }
                    that._curCircularArray.inc();
                }
                _setRename();
            }
        }

    }
    _moveItem () {
        var i : number, currentFocus : number;

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
                ccid : false,
                name : this._curCircularArray.curItem().name,
                majorChannel : this._curCircularArray.curItem().majorChannel,
                isHD : this._curCircularArray.curItem().isHD
            });
            for ( i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
            //this._chList.$.remove();
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
            this.appendChList(true);
            for ( i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
            this._chList.setItemFocus(currentFocus);
            this._chList.getFocusedItem().addClass('mov');
        } else if (this._selNum === 1) {
            currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
            this._moveBackupList.length = 0;
            this._moveBackupList = this._curCircularArray.clone();
            this._moveBackupFocus = currentFocus;

            this._currentState = 'mov';
            this._currentMoveItemList.length = 0;
            for ( i = 0; i < this._curCircularArray.length(); i += 1) {
                if (this._curCircularArray.curItem().select) {
                    this._curCircularArray.curItem().select = false;
                    break;
                }
                this._curCircularArray.inc();
            }

            this._currentMoveItemList.push(this._curCircularArray.curItem());
            this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, {
                ccid : false,
                name : this._curCircularArray.curItem().name,
                majorChannel : this._curCircularArray.curItem().majorChannel,
                isHD : this._curCircularArray.curItem().isHD
            });
            for ( i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
            //this._chList.$.remove();
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
            this.appendChList(true);
            for ( i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
            this._chList.setItemFocus(currentFocus);
            this._chList.getFocusedItem().addClass('mov');
        } else if (this._selNum > 1) {
            currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
            this._moveBackupList.length = 0;
            this._moveBackupList = this._curCircularArray.clone();
            this._moveBackupFocus = currentFocus;

            this._currentState = 'multimov';
            this._currentMoveItemList.length = 0;
            for ( i = 0; i < this._curCircularArray.length(); i += 1) {
                if (this._curCircularArray.curItem().select) {
                    this._curCircularArray.curItem().select = false;
                    this._currentMoveItemList.push(this._curCircularArray.curItem());
                    this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1);
                    this._curCircularArray._idxLast = this._curCircularArray.length() - 1;
                    if (this._curCircularArray._idxCur > this._curCircularArray._idxLast) {
                        this._curCircularArray._idxCur = 0;
                    }
                    i -= 1;
                    //i += 1;
                    //this._curCircularArray.indexing(this._curCircularArray._idxCur);
                } else {
                    this._curCircularArray.inc();
                }
            }

            if (currentFocus + 1 > this._allNum - this._selNum) {
                currentFocus = 0;
            }

            this._curCircularArray._array.splice(this._curCircularArray._idxCur, 0, {
                name : '',
                majorChannel : ''
            });
            for ( i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.dec();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.dec();
                }
            }
            //this._chList.$.remove();
            this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
            this.appendChList(true);
            for ( i = 0; i < currentFocus; i += 1) {
                this._curCircularArray.inc();
                while (this._curCircularArray.curItem().removed === true) {
                    this._curCircularArray.inc();
                }
            }
            this._chList.setItemFocus(currentFocus);
            this._chList.getFocusedItem().addClass('mov grp');
            //.removeClass('sel');
        }
    }
    _changeGroup (aIndex) {
        var i : number, channelLength : number;
        var chTypeStr : string = '';

        if ((this._chType === 'TV' && aIndex === 0) || (this._chType === 'RADIO' && aIndex === 1)) {
            this._$opt.removeClass('on');
            this._optGroupList.$.remove();
            this._$opt.append(this._optList.$);
            this.append(this._optList);
            this._chList.on();
            return;
        }

        switch(this._chType) {
            case 'TV' :
                this._circularTVArray = this._curCircularArray;
                break;
            case 'RADIO' :
                this._circularRADIOArray = this._curCircularArray;
                break;
        }

        switch(aIndex) {
            case 0:
                //TV
                channelLength = 0;
                for ( i = 0; i < this._circularTVArray.length(); i += 1) {
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
                for ( i = 0; i < this._circularRADIOArray.length(); i += 1) {
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
        } else if (this._chType === 'RADIO') {
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
    }
    _select_unselect_Item (aIndex) {
        if (this._curCircularArray.curItem().select) {
            this._curCircularArray.curItem().select = false;
            this._chList.getItem(aIndex).removeClass('sel');
            this._selNum -= 1;
            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
        } else {
            this._curCircularArray.curItem().select = true;
            this._chList.getItem(aIndex).addClass('sel');
            this._selNum += 1;
            this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
        }
        if (aIndex === 7 && this._allNum > 8) {
            this.updateChListDown();
        } else {
            if (aIndex === this._allNum - 1) {
                this._chList.setItemFocus(0);
            } else {
                this._chList.getItem(aIndex + 1).addClass('on');
            }
            this._curCircularArray.inc();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.inc();
            }
        }
    }
    _selectAll () {
        var i : number;

        if (this._allNum <= 0) {
            return;
        }

        var currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
        for ( i = 0; i < this._curCircularArray.length(); i += 1) {
            if (!this._curCircularArray.curItem().select) {
                this._curCircularArray.curItem().select = true;
            }
            this._curCircularArray.inc();
        }

        for ( i = 0; i < currentFocus; i += 1) {
            this._curCircularArray.dec();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.dec();
            }
        }
        //this._chList.$.remove();
        this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
        this.appendChList(true);
        this._chList.setItemFocus(currentFocus);
        for ( i = 0; i < currentFocus; i += 1) {
            this._curCircularArray.inc();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.inc();
            }
        }

        this._selNum = this._curCircularArray.length();
        this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
    }
    _deselectAll () {
        var i : number;

        if (this._allNum <= 0) {
            return;
        }

        var currentFocus = this._chList.$.children('.on').prevAll('.focusable').length;
        for ( i = 0; i < this._curCircularArray.length(); i += 1) {
            if (this._curCircularArray.curItem().select) {
                this._curCircularArray.curItem().select = false;
            }
            this._curCircularArray.inc();
        }

        for ( i = 0; i < currentFocus; i += 1) {
            this._curCircularArray.dec();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.dec();
            }
        }
        //this._chList.$.remove();
        this._currentList = this.makeCurrentShowList(this._getCircularItem(8, 0));
        this.appendChList(true);
        this._chList.setItemFocus(currentFocus);
        for ( i = 0; i < currentFocus; i += 1) {
            this._curCircularArray.inc();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.inc();
            }
        }

        this._selNum = 0;
        this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
    }
    _moveCancel () {
        var i : number, bConsumed = false;

        this._curCircularArray = this._moveBackupList.clone();

        for ( i = 0; i < this._moveBackupFocus; i += 1) {
            this._curCircularArray.dec();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.dec();
            }
        }
        this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
        this.appendChList(true);
        this._chList.setItemFocus(this._moveBackupFocus);
        for ( i = 0; i < this._moveBackupFocus; i += 1) {
            this._curCircularArray.inc();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.inc();
            }
        }
        this._selNum = 0;
        this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
        this._currentState = 'none';
    }
    _moveOk () {
        var i : number;
        var num : number = 0, smallestLcn : number, smallestLcnIdx : number, roopIdx : number, roopCount : number, currentFocus : number;

        if (this._currentState === 'mov') {
            this._isChange = true;

            if (hx.config.useFavouriteCollection) {
                hx.svc.CH_LIST.moveChListItem({
                    favtype : this._chType,
                    ccid : this._currentMoveItemList[0].ccid,
                    insertTargetccid : this._curCircularArray.getNextItem().ccid
                });
            }

            this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._currentMoveItemList[0]);
            //insert
            currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;

            smallestLcn = this._curCircularArray._array[0].majorChannel;
            //find smallestLcnIdx
            smallestLcnIdx = 0;
            for ( i = 1; i < this._curCircularArray.length(); i += 1) {
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
                for ( i = 1; i < this._curCircularArray.length(); i += 1) {
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
                for ( i = 0; i < this._curCircularArray.length(); i += 1) {
                    if (this._currentMoveItemList[0].ccid === this._curCircularArray._array[i].ccid) {
                        smallestLcnIdx = i;
                        break;
                    }
                }
            }
        } else if (this._currentState === 'multimov') {
            this._isChange = true;

            if (hx.config.useFavouriteCollection) {
                var targetChIndex = hx.svc.CH_LIST.moveChListItem({
                    favtype : this._chType,
                    ccid : this._currentMoveItemList[0].ccid,
                    insertTargetccid : this._curCircularArray.getNextItem().ccid
                });

                for ( i = 1; i < this._currentMoveItemList.length; i += 1) {
                    if (hx.config.useFavouriteCollection) {
                        hx.svc.CH_LIST.moveChListItemWithTargetIdx({
                            favtype : this._chType,
                            ccid : this._currentMoveItemList[i].ccid,
                            targetIdx : targetChIndex,
                            offset : i
                        });
                    }
                }
            }

            this._currentMoveItemList[0].majorChannel = 100000;
            //remove previous number
            this._curCircularArray._array.splice(this._curCircularArray._idxCur, 1, this._currentMoveItemList[0]);
            //insert

            for ( i = 1; i < this._currentMoveItemList.length; i += 1) {
                this._currentMoveItemList[i].majorChannel = 100000;
                //remove previous number
                this._curCircularArray._array.splice(this._curCircularArray._idxCur + i, 0, this._currentMoveItemList[i]);
                //insert
            }

            currentFocus = this._chList.$.children('.mov').prevAll('.focusable').length;

            var lastMoveItemIdx;
            //find smallestLcnIdx
            smallestLcn = this._curCircularArray._array[0].majorChannel;
            smallestLcnIdx = 0;
            for ( i = 1; i < this._curCircularArray.length(); i += 1) {
                if (smallestLcn > this._curCircularArray._array[i].majorChannel) {
                    smallestLcn = this._curCircularArray._array[i].majorChannel;
                    smallestLcnIdx = i;
                }
            }

            for ( i = 0; i < this._curCircularArray.length(); i += 1) {
                if (this._currentMoveItemList[this._currentMoveItemList.length - 1].ccid === this._curCircularArray._array[i].ccid) {
                    lastMoveItemIdx = i;
                    break;
                }
            }

            //if insertTargetIdx is just before smallestLcnIdx, set smallestLcnIdx with moveItem
            if (this._curCircularArray.getNextItemWithCurIdx(lastMoveItemIdx).ccid === this._curCircularArray._array[smallestLcnIdx].ccid) {
                for ( i = 0; i < this._curCircularArray.length(); i += 1) {
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
            } else if (this._chType === 'RADIO') {
                num = 5000;
            }
        } else {
            if (hx.config.useLcnUpdate) {
                if (this._chType === 'TV') {
                    num = (this._lastMajorChannelOfTvLcnList + 1);
                } else if (this._chType === 'RADIO') {
                    num = (this._lastMajorChannelOfRadioLcnList + 1);
                }
            } else {
                if (ENV.op !== ENV.listOP.AUSDTT && ENV.op !== ENV.listOP.UKDTT) {
                    num = 100;
                }
            }
        }
        roopIdx = smallestLcnIdx;
        roopCount = this._curCircularArray.length();
        for ( i = 0; i < roopCount; i += 1) {
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

        for ( i = 0; i < currentFocus; i += 1) {
            this._curCircularArray.dec();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.dec();
            }
        }
        this._currentList = this.makeCurrentShowList(this._getCircularItem(8));
        this.appendChList(true);
        this._chList.setItemFocus(currentFocus);

        for ( i = 0; i < currentFocus; i += 1) {
            this._curCircularArray.inc();
            while (this._curCircularArray.curItem().removed === true) {
                this._curCircularArray.inc();
            }
        }
        this._selNum = 0;
        this._hgroup.$.find('p').html(this._selNum + ' / ' + this._allNum);
        this._currentState = 'none';
    }
    updateChListDown () {
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
    }
    updateChListUp () {
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
    }
    getChlist () {
        var retObj : any;

        retObj = hx.svc.CH_LIST.getChlist({
            group : 'TV'
        });
        this._tvList = retObj.chList;
        retObj = hx.svc.CH_LIST.getChlist({
            group : 'RADIO'
        });
        this._radioList = retObj.chList;
        if (undefined !== this._chType) {
            this.getSrvlist(this._chType);
        }
    }
    getSrvlist (aChType) {
        var svcList : any;
        var ulOrderingNumber : number = 0;

        if ((hx.config.useDefaultChannels === 1) && (ENV.listModel.HMS1000S === ENV.model)) {
            ulOrderingNumber = 99;
        } else {
            ulOrderingNumber = 0;
        }

        var _makeCircularArray = (svcList, type) => {
            var i, svcListLength = svcList.length, _svcList = [], lastLcnOfLcnList = 99;
            for ( i = 0; i < svcListLength; i += 1) {
                if (hx.config.useLcnUpdate) {
                    if (svcList[i].svcSection === this.SVC_SECTION.SECTION_LCN) {//find lastMajorChannelOfLcnList
                        if (svcList[i].majorChannel > lastLcnOfLcnList) {
                            lastLcnOfLcnList = svcList[i].majorChannel;
                        }
                    }
                    if (svcList[i].svcSection === this.SVC_SECTION.SECTION_NONE_LCN) {//filtering defaultCh and LcnCh
                        insertWrappingChObj(svcList[i], _svcList);
                    }
                } else {
                    if (svcList[i].majorChannel > ulOrderingNumber) {//filtering default channel.. it will be changed.
                        insertWrappingChObj(svcList[i], _svcList);
                    }
                }
            }
            switch(type) {
                case 'TV' :
                    if (hx.config.useLcnUpdate) {
                        this._lastMajorChannelOfTvLcnList = lastLcnOfLcnList;
                    }
                    this._circularTVArray = new hx.il.circularArray(_svcList);
                    break;
                case 'RADIO' :
                    if (hx.config.useLcnUpdate) {
                        this._lastMajorChannelOfRadioLcnList = lastLcnOfLcnList;
                    }
                    this._circularRADIOArray = new hx.il.circularArray(_svcList);
                    break;
                case 'TVBACKUP' :
                    this._circularTVArrayBackup = new hx.il.circularArray(_svcList);
                    break;
                case 'RADIOBACKUP' :
                    this._circularRADIOArrayBackup = new hx.il.circularArray(_svcList);
                    break;
            }
        }

        var insertWrappingChObj = (chObj, _svcList) => {
            var wrapChObj = {
                ccid : '',
                select : false,
                manualBlock : false,
                majorChannel : 0,
                isHD : false,
                removed : false,
                name : '',
                casInfo : '',
                isNameChange : false,
                isBlockChange : false
            };
            wrapChObj.ccid = chObj.ccid;
            wrapChObj.select = chObj.select;
            wrapChObj.manualBlock = chObj.manualBlock;
            wrapChObj.majorChannel = chObj.majorChannel;
            wrapChObj.isHD = chObj.isHD;
            wrapChObj.name = chObj.name;
            wrapChObj.isNameChange = false;
            wrapChObj.isBlockChange = false;
            wrapChObj.casInfo = this.getCasInformation(chObj);
            _svcList.push(wrapChObj);
        }

        _makeCircularArray(this._tvList, 'TV');
        _makeCircularArray(this._radioList, 'RADIO');
        _makeCircularArray(this._tvList, 'TVBACKUP');
        _makeCircularArray(this._radioList, 'RADIOBACKUP');

        switch (aChType) {
            case 'TV' :
                this._curCircularArray = this._circularTVArray;
                this._allNum = this._circularTVArray.length();
                break;
            case 'RADIO' :
                this._curCircularArray = this._circularRADIOArray;
                this._allNum = this._circularRADIOArray.length();
                break;
            default :
                break;
        }
    }
    appendChList (aIsListOn : boolean) {
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
    }
    makeCurrentShowList (aChlist, aMoveType? : string) {
        var chArray : any = [], chInfo : any, chTempInfo : any, chLength : number;
        var notcheckPoint : boolean = true, i : number;

        chArray.length = 0;
        chLength = aChlist.length;

        chTempInfo = {
            chName : '',
            manualBlock : false,
            hd : false,
            cas : false,
            select : false
        };

        switch(aMoveType) {
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

        for ( i = 0; i < chLength; i += 1) {
            if (i === this._currentMovePoint && notcheckPoint) {
                chArray.push(chTempInfo);
                i -= 1;
                notcheckPoint = false;
            } else {
                if (aChlist.length === i) {
                    continue;
                }
                chInfo = {
                    chName : '',
                    manualBlock : false,
                    hd : false,
                    cas : false,
                    select : false
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
    }
    getCasInformation (channel) {
        var i : number, casArr : any = [], length : number, casIDs : string;

        if (channel.ccid) {
            if (!!channel.casIDs) {
                length = channel.casIDs.length;
                for ( i = 0; i < length; i += 1) {
                    casArr.push(channel.casIDs[i]);
                }
            }
        }

        if (casArr.length > 0) {
            casIDs = 'cas ';
            if (casArr.length > 1) {
                casIDs += 'multi';
            } else {
                for ( i = 0, length = casArr.length; i < length; i += 1) {
                    if (casArr[i] === this._strCasID.indexOf('CAS_NAGRA')) {
                        casIDs += 'na ';
                    } else if (casArr[i] === this._strCasID.indexOf('CAS_IRDETO')) {
                        casIDs += 'ir ';
                    } else if (casArr[i] === this._strCasID.indexOf('CAS_NDS')) {
                        casIDs += 'nd ';
                    } else if (casArr[i] === this._strCasID.indexOf('CAS_VIACCESS')) {
                        casIDs += 'va ';
                    } else if (casArr[i] === this._strCasID.indexOf('CAS_CONAX')) {
                        casIDs += 'cx ';
                    } else if (casArr[i] === this._strCasID.indexOf('CAS_MEDIAGUARD')) {
                        casIDs += 'mg ';
                    } else if (casArr[i] === this._strCasID.indexOf('CAS_CRYPTOWORKS')) {
                        casIDs += 'cr ';
                    }
                }
            }
        }
        return casIDs;
    }
}


export = CPgEditChannels;