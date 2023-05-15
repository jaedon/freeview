var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    // file name : pages/settings/channel/fav_channels.js
    /*jslint sloppy:true, nomen:true, vars:true*/
    var cCPgFavoriteChannels = (function (_super) {
        __extends(cCPgFavoriteChannels, _super);
        function cCPgFavoriteChannels() {
            _super.call(this);
            this.OPTIONMENU = {
                REMOVE: 0,
                CHANGE_FAVOURITE: 1,
                RENAME_FAVOURITE: 2,
                SELECT_ALL: 3,
                DESELECT_ALL: 4
            };
            this.FAVOURITELISTLENGTH = 5;
            this.debug = false;
            this.$selectionEl = null;
            this.$titleEl = null;
            this.$loaderEl = null;
            this._mainChannelList = null;
            this._favChannelList = null;
            this._datas = [];
            this._dateTime = null;
            this._dlgMsg = null;
            this._isActive = false;
            this._isChangePageInSetting = false;
            this._mainFocusedIndex = 0;
            this._favFocusedIndex = 0;
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
                    fadeIn2: {
                        start: {
                            'opacity': 0,
                            'top': '20px'
                        },
                        end: {
                            'opacity': 0.5,
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
            this.init();
        }
        cCPgFavoriteChannels.prototype.init = function () {
            hx.log("info", "EditChannels - init");
        };
        cCPgFavoriteChannels.prototype.create = function (aTarget, aArgObject) {
            _super.prototype.create.call(this);
            var that = this;
            that._mainFocusedIndex = 0;
            that._favFocusedIndex = 0;
            that._datas = [];
            hx.log("info", "EditChannels - create");
            that._mainOptList = [{
                'name': hx.l('LOC_ADDTOFAV_ID'),
                'cb': 'addToFav'
            }, {
                'name': hx.l('LOC_CHANGE_GROUP_ID'),
                'sub_menu': [{
                    'name': hx.l('LOC_TV_ID'),
                    'cb': 'changeGroup'
                }, {
                    'name': hx.l('LOC_RADIO_ID'),
                    'cb': 'changeGroup'
                }, {
                    'name': hx.l('LOC_HDTV_ID'),
                    'cb': 'changeGroup'
                }]
            }, {
                'name': hx.l('LOC_SELECT_ALL_ID'),
                'cb': 'selectAll'
            }, {
                'name': hx.l('LOC_DESELECT_ALL_ID'),
                'cb': 'deselectAll'
            }];
            that._subOptList = [{
                'name': hx.l('LOC_REMOVE_ID'),
                'cb': 'removeItem'
            }, {
                'name': hx.l('LOC_MOVE_ID'),
                'cb': 'moveItem'
            }, {
                'name': hx.l('LOC_CHANGE_FAVOURITE_ID'),
                'sub_menu': [{
                    'name': "Favourite1",
                    'cb': 'changeFavourite'
                }, {
                    'name': "Favourite2",
                    'cb': 'changeFavourite'
                }, {
                    'name': "Favourite3",
                    'cb': 'changeFavourite'
                }, {
                    'name': "Favourite4",
                    'cb': 'changeFavourite'
                }, {
                    'name': "Favourite5",
                    'cb': 'changeFavourite'
                }]
            }, {
                'name': hx.l('LOC_RENAME_FAVOURITE_ID'),
                'cb': 'renameFavourite'
            }, {
                'name': hx.l('LOC_SELECT_ALL_ID'),
                'cb': 'selectAll'
            }, {
                'name': hx.l('LOC_DESELECT_ALL_ID'),
                'cb': 'deselectAll'
            }];
            that._movesubOptList = [{
                'name': hx.l('LOC_OK_ID'),
                'cb': 'moveOkItem'
            }, {
                'name': hx.l('LOC_CANCEL2_ID'),
                'cb': 'moveCancelItem'
            }];
            that._selAllOptList = [{
                'name': hx.l('LOC_REMOVE_ID'),
                'cb': 'removeItem'
            }, {
                'name': hx.l('LOC_CHANGE_FAVOURITE_ID'),
                'sub_menu': [{
                    'name': "Favourite1",
                    'cb': 'changeFavourite'
                }, {
                    'name': "Favourite2",
                    'cb': 'changeFavourite'
                }, {
                    'name': "Favourite3",
                    'cb': 'changeFavourite'
                }, {
                    'name': "Favourite4",
                    'cb': 'changeFavourite'
                }, {
                    'name': "Favourite5",
                    'cb': 'changeFavourite'
                }]
            }, {
                'name': hx.l('LOC_RENAME_FAVOURITE_ID'),
                'cb': 'renameFavourite'
            }, {
                'name': hx.l('LOC_SELECT_ALL_ID'),
                'cb': 'selectAll'
            }, {
                'name': hx.l('LOC_DESELECT_ALL_ID'),
                'cb': 'deselectAll'
            }];
            that.$section = $(document.getElementById('set'));
            that.$bg = $('.prism-page.setting');
            that.$section[0].querySelector('h1').innerHTML = hx.l('LOC_FAVOURITE_CHANNELS_ID');
            $('#set .title').ani({
                style: that.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            that.progressMsg = hx.msg('progress', {
                text: hx.l('LOC_LOADING_DOT_ID'),
                _use_backkey: false,
                auto_close: false
            });
            //compose
            hx.al.compose(that);
            //animation
            var $target = $('#DIA .vaTop');
            hx.il.ani.action({
                type: 'in',
                targets: $target,
                cb: function () {
                    that.$wrap = $(document.getElementById('wrap'));
                    that._initFavChannelList();
                    that._initOptionList();
                    that._initMainChannelList();
                    //rendering
                    that._render();
                    hx.msg.close('progress', that.progressMsg);
                    $('#set .chItem').eq(0).ani({
                        style: that.aniConfig.channelMenu.fadeIn,
                        trans: {
                            dur: 200,
                            delay: 400
                        },
                        cb: function () {
                            $('#set .chItem').eq(0).css('opacity', '');
                        }
                    });
                    $('#set .chItem').eq(1).ani({
                        style: that.aniConfig.channelMenu.fadeIn2,
                        trans: {
                            dur: 200,
                            delay: 400
                        },
                        cb: function () {
                            $('#set .chItem').eq(1).css('opacity', '');
                            //compose
                            that.append(that.$mainChannelList).append(that.$favChannelList).append(that.$sectionOpt);
                            that.$mainChannelList.setFocus(that._mainFocusedIndex);
                            that.$mainChannelList.on();
                            that._isChangePageInSetting = false;
                        }
                    });
                }
            });
        };
        cCPgFavoriteChannels.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            this._mainFocusedIndex = undefined;
            this._favFocusedIndex = undefined;
            this._mainChannelList = undefined;
            this._favChannelList = undefined;
            this._datas = undefined;
            this.$sectionOpt.$.remove();
            //$('#opt').remove();
            this.$mainChannelList.$.remove();
            this.$mainChannelList = undefined;
            this.$favChannelList.$.remove();
            this.$favChannelList = undefined;
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
        };
        cCPgFavoriteChannels.prototype._render = function () {
            this.$section[0].appendChild(this.$mainChannelList.$[0]);
            this.$section[0].appendChild(this.$favChannelList.$[0]);
            this.$bg[0].appendChild(this.$sectionOpt.$[0]);
        };
        cCPgFavoriteChannels.prototype._initMainChannelList = function () {
            this.$mainChannelList = hx.al.component('ChannelList', {
                'parent': this,
                'type': 'main',
                'className': 'chItem duel'
            });
        };
        cCPgFavoriteChannels.prototype._initFavChannelList = function () {
            this.$favChannelList = hx.al.component('ChannelList', {
                'parent': this,
                'type': 'fav',
                'className': 'chItem duel dis'
            });
        };
        cCPgFavoriteChannels.prototype._initOptionList = function () {
            this.$sectionOpt = hx.al.component('editChannelOptList', {
                'id': 'opt',
                'tit': hx.l('LOC_EDIT_ID')
            });
        };
        cCPgFavoriteChannels.prototype._on_alKey = function (aParam) {
            var that = this, bConsumed = false, k = hx.key, kc = aParam.alKey;
            switch (kc) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case k.KEY_LEFT:
                case k.KEY_RIGHT:
                    if (aParam.alData) {
                        bConsumed = true;
                        if (aParam.alData.me.mode !== 'moveItem' && aParam.alData.me.mode !== 'moveGroup') {
                            this._doToggle(aParam.alData.me);
                        }
                    }
                    break;
                case k.KEY_BACK:
                    if (aParam.alData) {
                        bConsumed = true;
                        if (aParam.alData.me === that.$sectionOpt) {
                            if (that.$sectionOpt._optList[1].sub_menu && that.$sectionOpt._totalCount === 3) {
                                that.$sectionOpt.fillOptList(that.$sectionOpt._optList);
                                that.$sectionOpt._setFocus(1);
                            }
                            else if (that.$sectionOpt._optList[2].sub_menu && that.$sectionOpt._totalCount === 5) {
                                that.$sectionOpt.fillOptList(that.$sectionOpt._optList);
                                that.$sectionOpt._setFocus(2);
                            }
                            else {
                                that._optBack();
                            }
                        }
                        else if (aParam.alData.me === that.$mainChannelList || aParam.alData.me === that.$favChannelList) {
                            if (aParam.alData.me === that.$favChannelList && (that.$favChannelList.mode === 'moveItem' || that.$favChannelList.mode === 'moveGroup')) {
                                that.$favChannelList.returnToOriginList();
                            }
                            else {
                                if (that.$favChannelList.isChange) {
                                    var confirmBack = hx.msg('confirm', {
                                        text: hx.l('LOC_SAVE_THE_CHANGE_ID'),
                                        auto_close: false,
                                        //timeout : 5000,
                                        btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                                        dia_class: 'osd',
                                        callback_fn: backFn
                                    });
                                    function backFn(_status) {
                                        if (_status === 'key_power' || _status === hx.l('LOC_NO_ID')) {
                                            that._isChangePageInSetting = true;
                                            hx.al.decompose(that);
                                            $('#set .chItem').eq(0).ani({
                                                style: that.aniConfig.channelMenu.fadeOut,
                                                trans: {
                                                    dur: 200,
                                                    delay: 100
                                                }
                                            });
                                            $('#set .chItem').eq(1).ani({
                                                style: that.aniConfig.channelMenu.fadeOut,
                                                trans: {
                                                    dur: 200,
                                                    delay: 100
                                                },
                                                cb: function () {
                                                    that.sendEventToEntity({
                                                        alEvent: 'alChangePage',
                                                        target: 'CPgSettings',
                                                        me: that
                                                    });
                                                }
                                            });
                                        }
                                        else if (_status === hx.l('LOC_YES_ID')) {
                                            that._renewFav('setting');
                                        }
                                        else if (_status === 'key_back' || _status === 'key_esc') {
                                            var noWork;
                                        }
                                    }
                                }
                                else {
                                    that._isChangePageInSetting = true;
                                    hx.al.decompose(that);
                                    $('#set .chItem').eq(0).ani({
                                        style: that.aniConfig.channelMenu.fadeOut,
                                        trans: {
                                            dur: 200,
                                            delay: 100
                                        }
                                    });
                                    $('#set .chItem').eq(1).ani({
                                        style: that.aniConfig.channelMenu.fadeOut,
                                        trans: {
                                            dur: 200,
                                            delay: 100
                                        },
                                        cb: function () {
                                            that.sendEventToEntity({
                                                alEvent: 'alChangePage',
                                                target: 'CPgSettings',
                                                me: that
                                            });
                                        }
                                    });
                                }
                            }
                        }
                    }
                    break;
                case k.KEY_OPTION:
                    if (aParam.alData) {
                        bConsumed = true;
                        if (that.$sectionOpt.$.hasClass('on')) {
                            that._optBack();
                        }
                        else if ((aParam.alData.me.mode === 'moveItem' || aParam.alData.me.mode === 'moveGroup')) {
                            that._domoveOption(aParam.alData.me);
                        }
                        else if (aParam.alData.me.totalCount === aParam.alData.me.checkedCount) {
                            that._doselAllOption(aParam.alData.me);
                        }
                        else {
                            that._doOption(aParam.alData.me);
                        }
                    }
                    break;
                case hx.key.KEY_ESCAPE:
                    if (aParam.alData) {
                        bConsumed = true;
                        if (aParam.alData.me === that.$sectionOpt) {
                            that._optBack();
                            break;
                        }
                        if (that.$favChannelList.isChange) {
                            var confirmEscape = hx.msg('confirm', {
                                text: hx.l('LOC_SAVE_THE_CHANGE_ID'),
                                auto_close: false,
                                //timeout : 5000,
                                btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                                dia_class: 'osd',
                                callback_fn: escapeFn
                            });
                            function escapeFn(_status) {
                                //$('#set').remove();
                                //$('#opt').remove();
                                //$('#DIA').remove();
                                if (_status === 'key_back' || _status === 'key_power' || _status === 'key_esc' || _status === hx.l('LOC_NO_ID')) {
                                    hx.svc.SETTING_UTIL.setFrontPanelString({
                                        text: ''
                                    });
                                    that.sendEventToEntity({
                                        alEvent: 'alChangePage',
                                        target: 'CPgLiveController',
                                        me: that
                                    });
                                }
                                else if (_status === hx.l('LOC_YES_ID')) {
                                    that._renewFav('live');
                                }
                            }
                        }
                        else {
                            hx.svc.SETTING_UTIL.setFrontPanelString({
                                text: ''
                            });
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgLiveController',
                                me: that
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgFavoriteChannels.prototype._doToggle = function (aChannelList) {
            var that = this, i, liList, focusObj, focusedIdx;
            liList = aChannelList.$[0].querySelectorAll('li');
            focusObj = aChannelList.$[0].querySelector('.on');
            for (i = 0; i < liList.length; i += 1) {
                if (liList[i] === focusObj) {
                    focusedIdx = i;
                    break;
                }
            }
            if (focusedIdx === undefined) {
                focusedIdx = 0;
            }
            if (aChannelList === that.$mainChannelList) {
                that.$mainChannelList.$[0].classList.add('dis');
                that.$favChannelList.$[0].classList.remove('dis');
                if (this.$favChannelList.totalCount !== 0) {
                    this.$favChannelList.setFocus(this._mainFocusedIndex);
                }
                if (focusObj) {
                    this._favFocusedIndex = focusedIdx;
                }
                that.$favChannelList.on();
            }
            else if (aChannelList === that.$favChannelList) {
                that.$favChannelList.$[0].classList.add('dis');
                that.$mainChannelList.$[0].classList.remove('dis');
                this.$mainChannelList.setFocus(this._favFocusedIndex);
                if (focusObj) {
                    this._mainFocusedIndex = focusedIdx;
                }
                that.$mainChannelList.on();
            }
        };
        cCPgFavoriteChannels.prototype._doOption = function (aChannelList) {
            //var opt : HTMLElement  = document.getElementById('opt');
            var opt = this.$sectionOpt.$[0];
            if (aChannelList === this.$mainChannelList) {
                this.$sectionOpt.parent = this.$mainChannelList;
                this.$sectionOpt.setOptList(this._mainOptList, this.$mainChannelList);
            }
            else if (aChannelList === this.$favChannelList) {
                this.$sectionOpt.parent = this.$favChannelList;
                this.$sectionOpt.setOptList(this._subOptList, this.$favChannelList);
            }
            opt.classList.add('on');
            this.$sectionOpt.on();
        };
        cCPgFavoriteChannels.prototype._domoveOption = function (aChannelList) {
            //var opt: HTMLElement  = document.getElementById('opt');
            var opt = this.$sectionOpt.$[0];
            if (aChannelList === this.$mainChannelList) {
                this.$sectionOpt.parent = this.$mainChannelList;
                this.$sectionOpt.setOptList(this._mainOptList, this.$mainChannelList);
            }
            else if (aChannelList === this.$favChannelList) {
                this.$sectionOpt.parent = this.$favChannelList;
                this.$sectionOpt.setOptList(this._movesubOptList, this.$favChannelList);
            }
            opt.classList.add('on');
            this.$sectionOpt.on();
        };
        cCPgFavoriteChannels.prototype._doselAllOption = function (aChannelList) {
            var opt = document.getElementById('opt');
            var opt = this.$sectionOpt.$[0];
            if (aChannelList === this.$mainChannelList) {
                this.$sectionOpt.parent = this.$mainChannelList;
                this.$sectionOpt.setOptList(this._mainOptList, this.$mainChannelList);
            }
            else if (aChannelList === this.$favChannelList) {
                this.$sectionOpt.parent = this.$favChannelList;
                this._selAllOptList[this.OPTIONMENU.CHANGE_FAVOURITE]['sub_menu'] = [{
                    'name': this.$favChannelList.favList1Name,
                    'cb': 'changeFavourite'
                }, {
                    'name': this.$favChannelList.favList2Name,
                    'cb': 'changeFavourite'
                }, {
                    'name': this.$favChannelList.favList3Name,
                    'cb': 'changeFavourite'
                }, {
                    'name': this.$favChannelList.favList4Name,
                    'cb': 'changeFavourite'
                }, {
                    'name': this.$favChannelList.favList5Name,
                    'cb': 'changeFavourite'
                }];
                this.$sectionOpt.setOptList(this._selAllOptList, this.$favChannelList);
            }
            opt.classList.add('on');
            this.$sectionOpt.on();
        };
        cCPgFavoriteChannels.prototype._optBack = function () {
            var opt = document.getElementById('opt');
            opt.classList.remove('on');
            if (this.$sectionOpt.target === this.$mainChannelList) {
                this.$mainChannelList.on();
            }
            else {
                this.$favChannelList.on();
            }
        };
        cCPgFavoriteChannels.prototype._renewFav = function (targetPage) {
            var that = this;
            var progressMsg = hx.msg('progress', {
                text: hx.l('LOC_SAVING_ID'),
                _use_backkey: false,
                auto_close: false
            });
            //animation
            var $target = $('#DIA .vaTop');
            hx.il.ani.action({
                type: 'in',
                targets: $target,
                cb: function () {
                    that.updateFavList();
                    hx.msg.close('progress', progressMsg);
                    if (targetPage === 'live') {
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text: ''
                        });
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: that
                        });
                    }
                    else if (targetPage === 'setting') {
                        that._isChangePageInSetting = true;
                        hx.al.decompose(that);
                        $('#set .chItem').eq(0).ani({
                            style: that.aniConfig.channelMenu.fadeOut,
                            trans: {
                                dur: 200,
                                delay: 100
                            }
                        });
                        $('#set .chItem').eq(1).ani({
                            style: that.aniConfig.channelMenu.fadeOut,
                            trans: {
                                dur: 200,
                                delay: 100
                            },
                            cb: function () {
                                that.sendEventToEntity({
                                    alEvent: 'alChangePage',
                                    target: 'CPgSettings',
                                    me: that
                                });
                            }
                        });
                    }
                }
            });
        };
        cCPgFavoriteChannels.prototype.updateFavList = function () {
            var that = this, i, j, retObj = {}, listLength, favType, favListName, favListNameBackup, chList = [], backupList = [], favListNum = 5, favListComponent = that.$favChannelList, updateParamArray = [], isChange = false;
            var updateParam = {
                favType: '',
                isNameChange: false,
                favListName: '',
                isListChange: false,
                ccids: []
            };
            for (i = 0; i < favListNum; i += 1) {
                updateParam = {
                    favType: '',
                    isNameChange: false,
                    favListName: '',
                    isListChange: false,
                    ccids: []
                };
                if (i === 0) {
                    favType = 'Favourite1';
                    favListName = favListComponent.favList1Name;
                    favListNameBackup = favListComponent.favList1NameBackup;
                    chList = favListComponent._favList1.chList;
                    backupList = favListComponent._favList1Backup;
                }
                else if (i === 1) {
                    favType = 'Favourite2';
                    favListName = favListComponent.favList2Name;
                    favListNameBackup = favListComponent.favList2NameBackup;
                    chList = favListComponent._favList2.chList;
                    backupList = favListComponent._favList2Backup;
                }
                else if (i === 2) {
                    favType = 'Favourite3';
                    favListName = favListComponent.favList3Name;
                    favListNameBackup = favListComponent.favList3NameBackup;
                    chList = favListComponent._favList3.chList;
                    backupList = favListComponent._favList3Backup;
                }
                else if (i === 3) {
                    favType = 'Favourite4';
                    favListName = favListComponent.favList4Name;
                    favListNameBackup = favListComponent.favList4NameBackup;
                    chList = favListComponent._favList4.chList;
                    backupList = favListComponent._favList4Backup;
                }
                else if (i === 4) {
                    favType = 'Favourite5';
                    favListName = favListComponent.favList5Name;
                    favListNameBackup = favListComponent.favList5NameBackup;
                    chList = favListComponent._favList5.chList;
                    backupList = favListComponent._favList5Backup;
                }
                updateParam.favType = favType;
                //check nameChange
                if (favListName !== favListNameBackup) {
                    updateParam.isNameChange = true;
                    updateParam.favListName = favListName;
                    favListNameBackup = favListName;
                }
                //check listChange
                listLength = chList.length;
                if (listLength === backupList.length) {
                    for (j = 0; j < listLength; j += 1) {
                        if (chList[j].ccid !== backupList[j].ccid) {
                            updateParam.isListChange = true;
                        }
                    }
                }
                else {
                    updateParam.isListChange = true;
                }
                //renew List
                if (updateParam.isListChange) {
                    updateParam.ccids = [];
                    for (j = 0; j < chList.length; j += 1) {
                        updateParam.ccids.push(chList[j].ccid);
                    }
                }
                updateParamArray.push({
                    favType: updateParam.favType,
                    isNameChange: updateParam.isNameChange,
                    favListName: updateParam.favListName,
                    isListChange: updateParam.isListChange,
                    ccids: updateParam.ccids
                });
            }
            for (i = 0; i < favListNum; i += 1) {
                if (updateParamArray[i].isNameChange || updateParamArray[i].isListChange) {
                    isChange = true;
                    break;
                }
            }
            if (isChange) {
                hx.svc.CH_LIST.renewFavList({
                    'updateParamArray': updateParamArray
                });
            }
        };
        cCPgFavoriteChannels.prototype.addToFav = function () {
            var i, j, mainChannelList = this.$mainChannelList._$items;
            var targetFavList, remainList = [], mainLen = mainChannelList.length, favLen, isAdd = true;
            remainList.length = 0;
            switch (this.$favChannelList.group) {
                case "Favourite1":
                    targetFavList = this.$favChannelList._favList1.chList;
                    break;
                case "Favourite2":
                    targetFavList = this.$favChannelList._favList2.chList;
                    break;
                case "Favourite3":
                    targetFavList = this.$favChannelList._favList3.chList;
                    break;
                case "Favourite4":
                    targetFavList = this.$favChannelList._favList4.chList;
                    break;
                case "Favourite5":
                    targetFavList = this.$favChannelList._favList5.chList;
                    break;
            }
            favLen = targetFavList.length;
            if (favLen + this.$mainChannelList.checkedCount <= 200) {
                if (this.$mainChannelList.checkedCount === 0 && this.$mainChannelList.totalCount !== 0) {
                    for (i = 0; i < mainLen; i += 1) {
                        if ($('#set #editChannelList').eq(0).children('ul').children('.on')[0].innerHTML === (mainChannelList[i].majorChannel + ' ' + mainChannelList[i].name)) {
                            for (j = 0; j < favLen; j += 1) {
                                if (targetFavList[j].ccid === mainChannelList[i].ccid) {
                                    isAdd = false;
                                }
                            }
                            if (isAdd) {
                                if (targetFavList.length >= 200) {
                                    this.$mainChannelList.deselectAll();
                                    this.$favChannelList.isChange = true;
                                    this.$favChannelList.reload(targetFavList, 0);
                                    return;
                                }
                                targetFavList.push(mainChannelList[i]);
                            }
                            else {
                                remainList.push(mainChannelList[i]);
                            }
                            isAdd = true;
                        }
                        else {
                            remainList.push(mainChannelList[i]);
                        }
                    }
                }
                else {
                    for (i = 0; i < mainLen; i += 1) {
                        if (mainChannelList[i].checked) {
                            for (j = 0; j < favLen; j += 1) {
                                if (targetFavList[j].ccid === mainChannelList[i].ccid) {
                                    isAdd = false;
                                }
                            }
                            if (isAdd) {
                                if (targetFavList.length >= 200) {
                                    this.$mainChannelList.deselectAll();
                                    this.$favChannelList.isChange = true;
                                    this.$favChannelList.reload(targetFavList, 0);
                                    return;
                                }
                                targetFavList.push(mainChannelList[i]);
                            }
                            else {
                                remainList.push(mainChannelList[i]);
                            }
                            isAdd = true;
                        }
                        else {
                            remainList.push(mainChannelList[i]);
                        }
                    }
                }
                this.$mainChannelList.reload(remainList, 0);
                var ul = this.$mainChannelList.$.children('ul'), list;
                if (ul) {
                    list = ul.children();
                    if (list.length > 0) {
                        list.eq(0).addClass('on');
                    }
                }
            }
            else {
                hx.msg('response', {
                    text: hx.l('LOC_ADD_ONLY_200_CHANNEL_ID'),
                    auto_close: true,
                    timeout: 3000
                });
            }
            this.$mainChannelList.deselectAll();
            this.$favChannelList.isChange = true;
            this.$favChannelList.reload(targetFavList, 0);
        };
        return cCPgFavoriteChannels;
    })(__KERNEL_SETTINGS__);
    ;
    return cCPgFavoriteChannels;
});