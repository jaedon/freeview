var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    //file name : pages/settings/network_settings/fileShare.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgFileShare = (function (_super) {
        __extends(cCPgFileShare, _super);
        function cCPgFileShare() {
            _super.call(this);
            this.TMenuList = {
                SAMBASERVER: 0,
                FTPSERVER: 1,
                PASSWORD: 2,
                DOWNLOADFOLDER: 3
            };
            this.TSubOnOffMenuList = {
                ON: 0,
                OFF: 1
            };
            this.TSubSetMenuList = {
                OFF: 0,
                SETPASSWORD: 1
            };
            this._isSambaOn = false;
            this._isFtpOn = false;
            this._isPasswordSet = false;
            this._isDownloadFolderEnable = true;
            this._folderPathName = "/mnt/hd2/Media/Download";
            this._isChangePageInSetting = false;
            this._contextListMap = [['LOC_FILE_SHARE_ID'], ['LOC_SAMBA SERVER_ID', 'LOC_FTP_SERVCER_ID', 'LOC_STR_PASSWORD_ID', 'LOC_DOWNLOAD_FOLDER_ID'], ['LOC_ON_ID', 'LOC_OFF_ID',], ['LOC_OFF_ID', 'LOC_SET_PASSWORD_ID'], ['LOC_SAMBA_SET_ON_CAN_COPY_ID', 'LOC_FTP_SET_ON_CAN_USING_ID', 'LOC_PASSWORD_SET_CAN_ACCESS_ID', 'LOC_DOWNLOAD_FOLDER_SET_ENABLE_CAN_COPY_ID']];
        }
        cCPgFileShare.prototype.create = function () {
            var that = this;
            var i, passwdStr = '';
            _super.prototype.create.call(this);
            $.extend(that, {
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _$contextMenu: $('<div>', {
                    'class': 'dep2'
                }),
                _fileShareMainMenu: hx.al.CPgFileShare.fileShareMainMenu(that._contextListMap[1]),
                _$noti: $('<p>', {
                    'class': 'noti',
                    'html': hx.l(that._contextListMap[4][0])
                }),
                _$chItem: $('<article>', {
                    'class': 'chItem ul folder'
                }),
                _$setPasswordStge: $('<section>', {
                    'class': 'setStge'
                }),
                _$setDownloadStge: $('<section>', {
                    'id': 'downloadStatusStage',
                    'class': 'setStge'
                }),
                _$passwordStatus: $('<h1>', {
                    'html': hx.l('LOC_STR_PASSWORD_ID')
                }),
                _$downloadFolderStatus: $('<h1>', {
                    'html': hx.l('LOC_DOWNLOAD_FOLDER_ID')
                }),
                _$titlePasswordArea: $('<p>', {
                    'class': 'tit area1 point',
                    'html': ''
                }),
                _$titleDownloadArea: $('<p>', {
                    'class': 'tit area1 point',
                    'html': ''
                }),
                _$wrapBtn: $('<div>', {
                    'class': 'wrap_btn'
                }),
                _$satrecordRegistration: $('<h1>', {
                    'html': hx.l('LOC_PASSWORD_ID')
                }),
                _$titleArea1: $('<p>', {
                    'class': 'tit area1 point',
                    'html': ''
                }),
                _$wrapInput: $('<div>', {
                    'class': 'content-wrapper',
                }),
                _$inputList: $('<dl>'),
                _$tempTitle: $('<dt>', {
                    'style': 'text-align:center;',
                    'html': hx.l('LOC_USER_NAME_ID')
                }),
                _$tempArea: $('<dd>', {
                    'html': 'admin'
                }),
                _$inputTitle: $('<dt>', {
                    'style': 'text-align:center;',
                    'html': ''
                }),
                _$inputArea: $('<dd>', {
                    'class': 'point'
                }),
                _$form: $('<form>', {
                    'method': 'post',
                }),
                _$inputList2: $('<dl>'),
                _$inputTitle2: $('<dt>', {
                    'style': 'text-align:center;',
                    'html': hx.l('LOC_VERIFY_PASSWORD_ID')
                }),
                _$inputArea2: $('<dd>', {
                    'class': 'point'
                }),
                _$form2: $('<form>', {
                    'method': 'post',
                }),
                _$btnWrap: $('<div>', {
                    'class': 'wrap_btn'
                }),
                _passwdInput: hx.al.CPgFileShare.input({
                    id: 'satInput',
                    type: 'password',
                    style: 'full'
                }),
                _passwdInput2: hx.al.CPgFileShare.input({
                    id: 'satInput2',
                    type: 'password',
                    style: 'full'
                }),
                _okBtn: hx.al.CPgFileShare.button('LOC_OK_ID'),
                _cancelBtn: hx.al.CPgFileShare.button('LOC_CANCEL_ID'),
                _okBtn2: hx.al.CPgFileShare.button('LOC_OK_ID'),
                _cancelBtn2: hx.al.CPgFileShare.button('LOC_CANCEL_ID'),
                _folderPath: hx.al.CPgFileShare.chListMenu({
                    name: that._folderPathName,
                    length: 1,
                    select: ''
                }),
            });
            that._$setPasswordStge.append(that._$satrecordRegistration).append(that._$titleArea1).append(that._$wrapInput.append(that._$inputList.append(that._$tempTitle).append(that._$tempArea).append(that._$inputTitle).append(that._$inputArea.append(that._$form.append(that._passwdInput.$))).append(that._$inputTitle2).append(that._$inputArea2.append(that._$form2.append(that._passwdInput2.$))))).append(that._$btnWrap.append(that._okBtn.$).append(that._cancelBtn.$));
            that._$setDownloadStge.append(that._$downloadFolderStatus).append(that._$titleDownloadArea).append(that._$chItem.append(that._folderPath.$)).append(that._$wrapBtn.append(that._okBtn2.$).append(that._cancelBtn2.$));
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l(that._contextListMap[0]).toUpperCase();
            }
            if (that._isSambaOn === undefined || that._isFtpOn === undefined || that._isPasswordSet === undefined || that._isDownloadFolderEnable === undefined) {
                that._isSambaOn = false;
                that._isFtpOn = false;
                that._isPasswordSet = false;
                that._isDownloadFolderEnable = true;
            }
            else {
                that._isSambaOn = hx.svc.SETTING_UTIL.getEnableSambaServer();
                that._isFtpOn = hx.svc.SETTING_UTIL.getEnableFtpServer();
                that._isPasswordSet = hx.svc.SETTING_UTIL.getPasswordSet();
                that._isDownloadFolderEnable = hx.svc.SETTING_UTIL.getEnableDownloadFolder();
            }
            var $sambaOnMenu = that._fileShareMainMenu.getItem(0).find('em');
            var $ftpOnMenu = that._fileShareMainMenu.getItem(1).find('em');
            var $passwordSetMenu = that._fileShareMainMenu.getItem(2).find('em');
            $sambaOnMenu.html(hx.l(that._contextListMap[2][((that._isSambaOn == true) ? 0 : 1)]));
            $ftpOnMenu.html(hx.l(that._contextListMap[2][((that._isFtpOn == true) ? 0 : 1)]));
            if (that._isPasswordSet == false || that._isPasswordSet == undefined) {
                $passwordSetMenu.html(hx.l(that._contextListMap[3][0]));
            }
            else {
                for (i = 0; i < that._isPasswordSet.length; i++)
                    passwdStr += '*';
                $passwordSetMenu.html(passwdStr);
            }
            $('#set').append(that._$menu.append(that._$contextMenu.after(that._fileShareMainMenu.$)).after(that._$noti)).append(that._$setPasswordStge).append(that._$setDownloadStge);
            hx.al.compose(that);
            $title.ani({
                style: that.aniConfig.fadeIn,
                trans: { dur: 100, delay: 300 }
            });
            that._$noti.ani({
                style: that.aniConfig.menu.changeIn,
                trans: { dur: 200, delay: 400 }
            });
            that._$menu.ani({
                style: that.aniConfig.menu.changeIn,
                trans: { dur: 200, delay: 400 },
                cb: function () {
                    that.append(that._fileShareMainMenu).append(that._okBtn).append(that._cancelBtn).append(that._passwdInput).append(that._passwdInput2).append(that._folderPath).append(that._okBtn2).append(that._cancelBtn2);
                    that._fileShareMainMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
            that._inputText = '';
            that._passwdInput.$.val('');
            that._passwdInput.$.bind('input', function () {
                that._inputText = $(this).val();
                hx.log('FileShare', 'input TEXT : ' + that._inputText);
            });
            that._inputText2 = '';
            that._passwdInput2.$.val('');
            that._passwdInput2.$.bind('input', function () {
                that._inputText2 = $(this).val();
                hx.log('FileShare', 'input TEXT : ' + that._inputText2);
            });
        };
        cCPgFileShare.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            this._$menu.remove();
            this._$noti.remove();
            this._$setPasswordStge.remove();
            this._$setDownloadStge.remove();
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
            this._$menu = undefined;
            this._$contextMenu = undefined;
            this._fileShareMainMenu = undefined;
            this._fileShareOnOffMenu = undefined;
            this._$noti = undefined;
        };
        cCPgFileShare.prototype._on_alKey = function (aParam) {
            var _this = this;
            var bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    if (aParam.alData) {
                        bConsumed = true;
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text: ''
                        });
                        this.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: this
                        });
                    }
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_LEFT:
                    bConsumed = true;
                    if (this._passwdInput.$.hasClass('on') || this._passwdInput2.$.hasClass('on')) {
                        this._passwdInput.$.removeClass('on');
                        this._passwdInput2.$.removeClass('on');
                        this._passwdInput.$.blur();
                        this._passwdInput2.$.blur();
                        this._fileShareMainMenu.setItemFocus(this._currentFocusMenu);
                        this._$setPasswordStge.ani({
                            style: this.aniConfig.channelMenu.fadeOut,
                            trans: { dur: 200, delay: 100 },
                            cb: function () {
                                _this._$setPasswordStge.removeClass('active');
                                _this._fileShareMainMenu.on();
                            }
                        });
                    }
                    else if (this._$contextMenu.hasClass('active')) {
                        if (aParam.alData.me === this._subList) {
                            this._subList.off();
                            this._$contextMenu.ani({
                                style: this.aniConfig.dep2.fadeOut,
                                trans: { dur: 150, delay: 75 },
                                cb: function () {
                                    _this._subList.$.remove();
                                    _this._$contextMenu.removeClass('active');
                                    _this._fileShareMainMenu.getItem(_this._currentFocusMenu).removeClass('his');
                                    _this._fileShareMainMenu.setItemFocus(_this._currentFocusMenu);
                                    _this._fileShareMainMenu.on();
                                }
                            });
                        }
                    }
                    break;
                case hx.key.KEY_UP:
                    bConsumed = true;
                    if (this._passwdInput.$.hasClass('on')) {
                        this._passwdInput.$.removeClass('focusable');
                        this._passwdInput.$.removeClass('on');
                        this._okBtn.on();
                        this._okBtn.$.addClass('on');
                    }
                    else if (this._passwdInput2.$.hasClass('on')) {
                        this._passwdInput2.$.removeClass('focusable');
                        this._passwdInput2.$.removeClass('on');
                        this._passwdInput.$.addClass('focusable');
                        this._passwdInput.$.addClass('on');
                    }
                    break;
                case hx.key.KEY_DOWN:
                    bConsumed = true;
                    if (this._passwdInput.$.hasClass('on')) {
                        this._passwdInput.$.removeClass('focusable');
                        this._passwdInput.$.removeClass('on');
                        this._passwdInput2.$.addClass('focusable');
                        this._passwdInput2.$.addClass('on');
                    }
                    else if (this._passwdInput2.$.hasClass('on')) {
                        this._passwdInput2.$.removeClass('focusable');
                        this._passwdInput2.$.removeClass('on');
                        this._okBtn.on();
                        this._okBtn.$.addClass('on');
                    }
                    break;
                case hx.key.KEY_OK:
                    if (this._passwdInput.$.hasClass('on')) {
                        if (this._firstOK1) {
                            this._passwdInput.$[0].value = '';
                            this._passwdInput.$[0].type = 'password';
                            this._firstOK1 = false;
                        }
                        this._passwdInput.$.blur();
                        this._passwdInput.$.focus();
                        bConsumed = true;
                    }
                    else if (this._passwdInput2.$.hasClass('on')) {
                        if (this._firstOK2) {
                            this._passwdInput2.$[0].value = '';
                            this._passwdInput2.$[0].type = 'password';
                            this._firstOK2 = false;
                        }
                        this._passwdInput2.$.blur();
                        this._passwdInput2.$.focus();
                        bConsumed = true;
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgFileShare.prototype._on_alClicked = function (aParam) {
            var that = this, data = aParam.alData, target = data.me, bConsumed = false;
            var beforeValue, selectValue;
            var responseStr, i, passwdStr = '';
            switch (target) {
                case that._fileShareMainMenu:
                    bConsumed = true;
                    if (data.index === that.TMenuList.DOWNLOADFOLDER) {
                        // TODO get download folder enable function
                        that._$titleDownloadArea.html(hx.l('LOC_CHECK_DOWNLOAD_FOLDER_ID'));
                        that._fileShareMainMenu.off();
                        that._$setDownloadStge.addClass('active');
                        that._$setDownloadStge.ani({
                            style: that.aniConfig.channelMenu.fadeIn,
                            trans: { dur: 200, delay: 100 },
                            cb: function () {
                                that._isDownloadFolderEnable = hx.svc.SETTING_UTIL.getEnableDownloadFolder();
                                if (that._isDownloadFolderEnable) {
                                    that._folderPath.getFocusedItem().addClass('sel');
                                }
                                else {
                                    that._folderPath.getFocusedItem().removeClass('sel');
                                }
                                that._folderPath.on();
                                that._fileShareMainMenu.$.removeClass('active');
                            }
                        });
                    }
                    else {
                        that.fileShareSubMenu(data.index);
                        that._fileShareMainMenu.off();
                    }
                    that._currentFocusMenu = data.index;
                    break;
                case that._folderPath:
                    bConsumed = true;
                    if (that._isDownloadFolderEnable === true) {
                        that._isDownloadFolderEnable = false;
                        that._folderPath.getFocusedItem().removeClass('sel');
                        that._folderPath.on();
                    }
                    else {
                        that._isDownloadFolderEnable = true;
                        that._folderPath.getFocusedItem().addClass('sel');
                        that._folderPath.on();
                    }
                    break;
                case that._subList:
                    switch (that._currentFocusMenu) {
                        case that.TMenuList.SAMBASERVER:
                            if (that._isSambaOn === true) {
                                beforeValue = true;
                            }
                            else {
                                beforeValue = false;
                            }
                            if (data.index === that.TSubOnOffMenuList.ON) {
                                selectValue = true;
                                that._isSambaOn = true;
                            }
                            else {
                                selectValue = false;
                                that._isSambaOn = false;
                            }
                            break;
                        case that.TMenuList.FTPSERVER:
                            if (that._isFtpOn === true) {
                                beforeValue = true;
                            }
                            else {
                                beforeValue = false;
                            }
                            if (data.index === that.TSubOnOffMenuList.ON) {
                                selectValue = true;
                                that._isFtpOn = true;
                            }
                            else {
                                selectValue = false;
                                that._isFtpOn = false;
                            }
                            break;
                        case that.TMenuList.PASSWORD:
                            if (that._isPasswordSet === false) {
                                beforeValue = false;
                            }
                            else {
                                beforeValue = true;
                            }
                            if (data.index === that.TSubSetMenuList.OFF) {
                                selectValue = false;
                            }
                            else {
                                selectValue = true;
                            }
                            break;
                    }
                    if ((selectValue === true && beforeValue === false) || (selectValue === false && beforeValue === true)) {
                        that._subList.off();
                        that._$contextMenu.ani({
                            style: that.aniConfig.dep2.fadeOut,
                            trans: { dur: 150, delay: 75 },
                            cb: function () {
                                var $updateMenu = that._fileShareMainMenu.getItem(that._currentFocusMenu).find('em');
                                if (that._currentFocusMenu === that.TMenuList.PASSWORD) {
                                    if (selectValue === false) {
                                        var tmp = hx.msg('confirm', {
                                            text: hx.l('LOC_DO_YOU_WANT_CANCEL_PASSWORD_ID'),
                                            auto_close: false,
                                            //timeout : 5000,
                                            btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                                            dia_class: 'osd',
                                            callback_fn: fn
                                        });
                                        function fn(_status) {
                                            if (_status === hx.l('LOC_YES_ID')) {
                                                $updateMenu.html(hx.l(that._contextListMap[3][0]));
                                                that._isPasswordSet = false;
                                                hx.svc.SETTING_UTIL.setPasswordSet(that._isPasswordSet);
                                            }
                                            that._fileShareMainMenu.setItemFocus(that._currentFocusMenu);
                                            that._fileShareMainMenu.on();
                                        }
                                    }
                                    else {
                                        //TODO: Get & View 'Set Password' value.
                                        that._makePasswdDlg();
                                    }
                                }
                                else {
                                    if (that._currentFocusMenu === that.TMenuList.SAMBASERVER) {
                                        hx.svc.SETTING_UTIL.setEnableSambaServer(that._isSambaOn);
                                    }
                                    else if (that._currentFocusMenu === that.TMenuList.FTPSERVER) {
                                        hx.svc.SETTING_UTIL.setEnableFtpServer(that._isFtpOn);
                                    }
                                    $updateMenu.html(hx.l(that._contextListMap[2][selectValue === true ? 0 : 1]));
                                    that._fileShareMainMenu.setItemFocus(that._currentFocusMenu);
                                    that._fileShareMainMenu.on();
                                }
                                that._subList.$.remove();
                                that._$contextMenu.removeClass('active');
                                that._fileShareMainMenu.getItem(that._currentFocusMenu).removeClass('his');
                            }
                        });
                    }
                    else if ((selectValue === true && beforeValue === true) || (selectValue === false && beforeValue === false)) {
                        that._subList.off();
                        that._$contextMenu.ani({
                            style: that.aniConfig.dep2.fadeOut,
                            trans: { dur: 150, delay: 75 },
                            cb: function () {
                                if (that._currentFocusMenu === that.TMenuList.PASSWORD) {
                                    if (that._isPasswordSet !== false) {
                                        that._makePasswdDlg();
                                    }
                                    else {
                                        that._fileShareMainMenu.setItemFocus(that._currentFocusMenu);
                                        that._fileShareMainMenu.on();
                                    }
                                }
                                else {
                                    that._fileShareMainMenu.setItemFocus(that._currentFocusMenu);
                                    that._fileShareMainMenu.on();
                                }
                                that._subList.$.remove();
                                that._$contextMenu.removeClass('active');
                                that._fileShareMainMenu.getItem(that._currentFocusMenu).removeClass('his');
                            }
                        });
                    }
                    bConsumed = true;
                    break;
                case that._okBtn:
                case that._cancelBtn:
                    bConsumed = true;
                    var $updateMenu = that._fileShareMainMenu.getItem(that._currentFocusMenu).find('em');
                    if (target === that._okBtn) {
                        if (that._comparePasswd()) {
                            that._isPasswordSet = that._inputText;
                            hx.svc.SETTING_UTIL.setPasswordSet(that._isPasswordSet);
                            for (i = 0; i < that._isPasswordSet.length; i++)
                                passwdStr += '*';
                            $updateMenu.html(passwdStr);
                            that._okBtn.off();
                            that._okBtn.$.removeClass('on');
                        }
                        else {
                            var checkDefaultInputValue = function () {
                                var defaultString = hx.l('LOC_PRESS_OK_ID');
                                if (that._inputText === defaultString || that._inputText2 === defaultString || that._inputText.trim().length < 1 || that._inputText2.trim().length < 1) {
                                    return true;
                                }
                                return false;
                            };
                            if (checkDefaultInputValue()) {
                                responseStr = 'LOC_MSG_ADD_A_FRIEND_ERROR_04_ID';
                            }
                            else {
                                responseStr = 'LOC_PASSWROD_NOT_MATCH_ID';
                            }
                            hx.msg('response', {
                                text: hx.l(responseStr),
                                auto_close: true,
                                close_time: 3000,
                                dia_class: 'res vaTop'
                            });
                            break;
                        }
                    }
                    else if (target === that._cancelBtn) {
                        that._cancelBtn.off();
                        that._cancelBtn.$.removeClass('on');
                    }
                    that._passwdInput.$.blur();
                    that._passwdInput2.$.blur();
                    that._fileShareMainMenu.setItemFocus(that._currentFocusMenu);
                    that._$setPasswordStge.ani({
                        style: that.aniConfig.channelMenu.fadeOut,
                        trans: { dur: 200, delay: 100 },
                        cb: function () {
                            that._$setPasswordStge.removeClass('active');
                            that._fileShareMainMenu.on();
                        }
                    });
                    break;
                case that._okBtn2:
                case that._cancelBtn2:
                    bConsumed = true;
                    if (target === that._okBtn2) {
                        //TODO set download folder enable function
                        if (that._folderPath.getFocusedItem().hasClass('sel')) {
                            that._isDownloadFolderEnable = true;
                        }
                        else {
                            that._isDownloadFolderEnable = false;
                        }
                        hx.svc.SETTING_UTIL.setEnableDownloadFolder({
                            enable: that._isDownloadFolderEnable,
                            name: that._folderPathName
                        });
                        that._okBtn2.off();
                        that._okBtn2.$.removeClass('on');
                    }
                    else if (target === that._cancelBtn2) {
                        that._cancelBtn2.off();
                        that._cancelBtn2.$.removeClass('on');
                    }
                    that._fileShareMainMenu.setItemFocus(that._currentFocusMenu);
                    that._$setDownloadStge.ani({
                        style: that.aniConfig.channelMenu.fadeOut,
                        trans: { dur: 200, delay: 100 },
                        cb: function () {
                            that._$setDownloadStge.removeClass('active');
                            that._fileShareMainMenu.on();
                        }
                    });
                    break;
            }
            return bConsumed;
        };
        cCPgFileShare.prototype._on_alExit = function (aParam) {
            var _this = this;
            var data = aParam.alData, bConsumed = false;
            if (data) {
                switch (data.me) {
                    case this._fileShareMainMenu:
                        bConsumed = true;
                        this._gotoSettingPage();
                        break;
                    case this._fileShareOnOffMenu:
                        bConsumed = true;
                        this._fileShareOnOffMenu.off();
                        this._$contextMenu.ani({
                            style: this.aniConfig.dep2.fadeOut,
                            trans: { dur: 150, delay: 75 },
                            cb: function () {
                                _this._fileShareMainMenu.getItem(0).removeClass('his');
                                _this._$contextMenu.removeClass('active');
                                _this._fileShareMainMenu.on();
                            }
                        });
                        break;
                    case this._okBtn:
                    case this._cancelBtn:
                        bConsumed = true;
                        if (data.me === this._okBtn) {
                            this._okBtn.off();
                            this._okBtn.$.removeClass('on');
                        }
                        else if (data.me === this._cancelBtn) {
                            this._cancelBtn.off();
                            this._cancelBtn.$.removeClass('on');
                        }
                        this._passwdInput.$.blur();
                        this._passwdInput2.$.blur();
                        this._fileShareMainMenu.setItemFocus(this._currentFocusMenu);
                        this._$setPasswordStge.ani({
                            style: this.aniConfig.channelMenu.fadeOut,
                            trans: { dur: 200, delay: 100 },
                            cb: function () {
                                _this._$setPasswordStge.removeClass('active');
                                _this._fileShareMainMenu.on();
                            }
                        });
                        break;
                    case this._folderPath:
                    case this._okBtn2:
                    case this._cancelBtn2:
                        bConsumed = true;
                        if (data.me === this._folderPath) {
                            this._folderPath.$.children().removeClass('on');
                        }
                        else if (data.me === this._okBtn2) {
                            this._okBtn2.off();
                            this._okBtn2.$.removeClass('on');
                        }
                        else if (data.me === this._cancelBtn2) {
                            this._cancelBtn2.off();
                            this._cancelBtn2.$.removeClass('on');
                        }
                        this._fileShareMainMenu.setItemFocus(this._currentFocusMenu);
                        this._$setDownloadStge.ani({
                            style: this.aniConfig.channelMenu.fadeOut,
                            trans: { dur: 200, delay: 100 },
                            cb: function () {
                                _this._$setDownloadStge.removeClass('active');
                                _this._fileShareMainMenu.on();
                            }
                        });
                        break;
                }
            }
            return bConsumed;
        };
        cCPgFileShare.prototype._on_alChangeFocus = function (aParam) {
            var index = aParam.alData.index;
            if (aParam.alData.me === this._fileShareMainMenu)
                this._$noti.html(hx.l(this._contextListMap[4][index]));
        };
        cCPgFileShare.prototype._on_alScrollRight = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._okBtn:
                    bConsumed = true;
                    this._cancelBtn.on();
                    this._okBtn.$.removeClass('on');
                    this._cancelBtn.$.addClass('on');
                    break;
                case this._cancelBtn:
                    bConsumed = true;
                    this._okBtn.on();
                    this._okBtn.$.addClass('on');
                    this._cancelBtn.$.removeClass('on');
                    break;
                case this._okBtn2:
                    bConsumed = true;
                    this._cancelBtn2.on();
                    this._okBtn2.$.removeClass('on');
                    this._cancelBtn2.$.addClass('on');
                    break;
                case this._cancelBtn2:
                    bConsumed = true;
                    this._okBtn2.on();
                    this._okBtn2.$.addClass('on');
                    this._cancelBtn2.$.removeClass('on');
                    break;
            }
            return bConsumed;
        };
        cCPgFileShare.prototype._on_alScrollLeft = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._okBtn:
                    bConsumed = true;
                    this._cancelBtn.on();
                    this._okBtn.$.removeClass('on');
                    this._cancelBtn.$.addClass('on');
                    break;
                case this._cancelBtn:
                    bConsumed = true;
                    this._okBtn.on();
                    this._okBtn.$.addClass('on');
                    this._cancelBtn.$.removeClass('on');
                    break;
                case this._okBtn2:
                    bConsumed = true;
                    this._cancelBtn2.on();
                    this._okBtn2.$.removeClass('on');
                    this._cancelBtn2.$.addClass('on');
                    break;
                case this._cancelBtn2:
                    bConsumed = true;
                    this._okBtn2.on();
                    this._okBtn2.$.addClass('on');
                    this._cancelBtn2.$.removeClass('on');
                    break;
            }
            return bConsumed;
        };
        cCPgFileShare.prototype._on_alScrollup = function (aParam) {
            var bConsumed = false;
            if (aParam.alData.me === this._okBtn2 || aParam.alData.me === this._cancelBtn2) {
                bConsumed = true;
                if (aParam.alData.me === this._okBtn2) {
                    this._okBtn2.$.removeClass('on');
                }
                else if (aParam.alData.me === this._cancelBtn2) {
                    this._cancelBtn2.$.removeClass('on');
                }
                this._folderPath.on();
            }
            else if (aParam.alData.me === this._folderPath) {
                bConsumed = true;
                this._folderPath.$.children().removeClass('on');
                this._okBtn2.$.addClass('on');
                this._okBtn2.on();
            }
            else if (aParam.alData.me === this._okBtn || aParam.alData.me === this._cancelBtn) {
                bConsumed = true;
                this._okBtn.off();
                this._cancelBtn.off();
                this._okBtn.$.removeClass('on');
                this._cancelBtn.$.removeClass('on');
                this._passwdInput2.$.addClass('focusable');
                this._passwdInput2.$.addClass('on');
            }
            return bConsumed;
        };
        cCPgFileShare.prototype._on_alScrolldown = function (aParam) {
            var bConsumed = false;
            if (aParam.alData.me === this._folderPath) {
                bConsumed = true;
                this._folderPath.$.children().removeClass('on');
                this._okBtn2.$.addClass('on');
                this._okBtn2.on();
            }
            else if (aParam.alData.me === this._okBtn2 || aParam.alData.me === this._cancelBtn2) {
                bConsumed = true;
                if (aParam.alData.me === this._okBtn2) {
                    this._okBtn2.$.removeClass('on');
                }
                else if (aParam.alData.me === this._cancelBtn2) {
                    this._cancelBtn2.$.removeClass('on');
                }
                this._folderPath.on();
            }
            else if (aParam.alData.me === this._okBtn || aParam.alData.me === this._cancelBtn) {
                bConsumed = true;
                this._okBtn.off();
                this._cancelBtn.off();
                this._okBtn.$.removeClass('on');
                this._cancelBtn.$.removeClass('on');
                this._passwdInput.$.addClass('focusable');
                this._passwdInput.$.addClass('on');
            }
            return bConsumed;
        };
        cCPgFileShare.prototype._makePasswdDlg = function (aParam) {
            var that = this;
            if (that._isPasswordSet === false) {
                that._$titleArea1.html(hx.l('LOC_ENTER_PASSWORD_RESTRICT_ACCESS_ID'));
                that._$inputTitle.html(hx.l('LOC_NEW_PASSWORD_ID'));
            }
            else {
                that._$titleArea1.html(hx.l('LOC_PASSWORD_ALREADY_SET_ID'));
                that._$inputTitle.html(hx.l('LOC_CHANGE_PASSWORD_ID'));
            }
            that._fileShareMainMenu.off();
            that._$setPasswordStge.addClass('active');
            that._$setPasswordStge.ani({
                style: that.aniConfig.channelMenu.fadeIn,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    that._passwdInput.$.addClass('on');
                }
            });
            that._$form.ani({
                style: that.aniConfig.channelMenu.fadeIn,
                trans: { dur: 200, delay: 100 },
            });
            that._$form2.ani({
                style: that.aniConfig.channelMenu.fadeIn,
                trans: { dur: 200, delay: 100 },
            });
            that._inputText = hx.l('LOC_PRESS_OK_ID');
            that._inputText2 = hx.l('LOC_PRESS_OK_ID');
            var $input = $('#satInput');
            $input.val(that._inputText);
            $input[0].type = 'text';
            $input.bind('input', function () {
                that._inputText = $(this).val();
                hx.log('FileShare', 'input TEXT : ' + that._inputText);
            });
            var $input2 = $('#satInput2');
            $input2.val(that._inputText);
            $input2[0].type = 'text';
            $input2.bind('input', function () {
                that._inputText2 = $(this).val();
                hx.log('FileShare', 'input TEXT : ' + that._inputText2);
            });
            that._firstOK1 = true;
            that._firstOK2 = true;
        };
        cCPgFileShare.prototype._comparePasswd = function (aParam) {
            var defaultString = hx.l('LOC_PRESS_OK_ID');
            if (this._inputText === defaultString || this._inputText2 === defaultString || this._inputText.trim().length < 1 || this._inputText2.trim().length < 1) {
                return false;
            }
            if (this._inputText === this._inputText2) {
                return true;
            }
            else {
                return false;
            }
        };
        cCPgFileShare.prototype._gotoSettingPage = function () {
            var _this = this;
            this._isChangePageInSetting = true;
            hx.al.decompose(this);
            this._$noti.ani({
                style: this.aniConfig.menu.changeOut3,
                trans: { dur: 200, delay: 100 }
            });
            this._$menu.ani({
                style: this.aniConfig.menu.changeOut3,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    _this.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgSettings',
                        me: _this
                    });
                }
            });
        };
        cCPgFileShare.prototype.fileShareSubMenu = function (aParam) {
            var that = this, index, subFocusIndex, topPx;
            if (aParam === that.TMenuList.SAMBASERVER || aParam === that.TMenuList.FTPSERVER) {
                index = 2;
                switch (aParam) {
                    case that.TMenuList.SAMBASERVER:
                        if (that._isSambaOn === true)
                            subFocusIndex = 0;
                        else
                            subFocusIndex = 1;
                        break;
                    case that.TMenuList.FTPSERVER:
                        if (that._isFtpOn === true)
                            subFocusIndex = 0;
                        else
                            subFocusIndex = 1;
                        break;
                }
            }
            else if (aParam === that.TMenuList.PASSWORD) {
                index = 3;
                topPx = 185;
                if (that._isPasswordSet === false)
                    subFocusIndex = 0;
                else
                    subFocusIndex = 1;
            }
            var dataList = that._contextListMap[index];
            that._subList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length,
                top: topPx
            });
            that._$contextMenu.addClass('active');
            that._fileShareMainMenu.getItem(aParam).addClass('his');
            that._$contextMenu.append(that._subList.$);
            that._subList.bind(dataList);
            that._subList.draw();
            that._$contextMenu.ani({
                style: that.aniConfig.dep2.fadeIn,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    that.append(that._subList);
                    that._subList.on();
                }
            });
        };
        return cCPgFileShare;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgFileShare = cCPgFileShare;
    hx.al.CPgFileShare.prototype.aniConfig = {
        fadeIn: {
            start: {
                'opacity': 0
            },
            end: {
                'opacity': 1
            }
        },
        menu: {
            changeIn: {
                start: {
                    'opacity': 0,
                    'left': '20px'
                },
                end: {
                    'opacity': 1,
                    'left': 0
                }
            },
            changeOut3: {
                start: {
                    'opacity': 1,
                    'left': 0
                },
                end: {
                    'opacity': 0,
                    'left': '20px'
                }
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
        },
        dep2: {
            fadeIn: {
                start: {
                    'opacity': 0,
                    'left': '795px'
                },
                end: {
                    'opacity': 1,
                    'left': '775px'
                }
            },
            fadeOut: {
                start: {
                    'opacity': 1,
                    'left': '775px'
                },
                end: {
                    'opacity': 0,
                    'left': '795px'
                }
            }
        }
    };
    hx.al.CPgFileShare.fileShareMainMenu = function (aParam) {
        var fileShareMainMenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': 'dep1'
            },
            data: aParam,
            count: aParam.length,
            fx: function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>'));
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return fileShareMainMenu;
    };
    hx.al.CPgFileShare.button = function (aParam) {
        return (new hx.al.component('button', {
            attr: {
                'class': 'button',
                'data-langID': aParam
            },
            usePointerKey: true
        }));
    };
    hx.al.CPgFileShare.chListMenu = function (aParam) {
        var chListMenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': ''
            },
            data: aParam,
            editChList: true,
            count: aParam.length,
            fx: function (i, o) {
                o.html(this.data.name).append($('<em>', {
                    'class': 'ico'
                }));
                if (this.data.select) {
                    o.addClass('sel');
                }
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return chListMenu;
    };
    hx.al.CPgFileShare.input = function (aParam) {
        return hx.al.component('input', {
            attr: {
                'id': aParam.id,
                'type': aParam.type,
                'style': aParam.style
            },
        });
    };
    hx.al.CPgFileShare.passwordText = function () {
        var that = this;
        var passwdTmp = hx.msg('passwdMulti', {
            text: hx.l('LOC_ENTER_SERVER_NAME_ID'),
            responseText: hx.l('LOC_TYPE_NAME_PRESS_ENTER_ID'),
            wifi: 'Name',
            string: hx.l('LOC_STR_SERVER_NAME_01_ID'),
            auto_close: false,
            callback_fn: fn
        });
        function fn(_status, _input) {
            if (_status === 'key_back' || _status === 'key_esc') {
                hx.msg.close('passwdMulti', passwdTmp);
                that._fileShareMainMenu.setItemFocus(that._currentFocusMenu);
            }
            else if (_status === hx.l('LOC_OK_ID')) {
                if (_input) {
                    if (_input.password !== '') {
                        that._status = "connecting";
                        hx.svc.SETTING_UTIL.renameServer({
                            serverName: _input.password
                        });
                        that._networkMainMenu.getItem(1).find('em').html(_input.password);
                    }
                }
                hx.msg.close('passwdMulti', passwdTmp);
                that._fileShareMainMenu.setItemFocus(that._currentFocusMenu);
            }
            else if (_status === 'close') {
                that._fileShareMainMenu.setItemFocus(that._currentFocusMenu);
            }
        }
        return passwdTmp;
    };
    return cCPgFileShare;
});
