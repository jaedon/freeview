var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var cPgDetailInfo = (function (_super) {
        __extends(cPgDetailInfo, _super);
        function cPgDetailInfo() {
            _super.call(this);
        }
        cPgDetailInfo.prototype.create = function (param) {
            var _this = this;
            hx.al.compose(this);
            this._setVisible(false);
            this._$wrap = $('#wrap');
            this._$showPages = [];
            hx.logadd('CPgDetailInfo');
            hx.svc.RECORDINGS.addEventCb('ShowHDAlternativeRecordWindow', this, function (aButtonInfo) {
                if (_this.getPageVisible(_this.name) === true) {
                    var dlg = prism.createHDAlternativeButtonDlg(null, aButtonInfo);
                    dlg.open({
                        onClosed: function () {
                            _this.setKeyHookingControl(dlg);
                        }
                    });
                    _this.setKeyHookingControl(dlg, true);
                }
            });
        };
        cPgDetailInfo.prototype.destroy = function (param) {
            this._setVisible(false);
            this._hideAndDestroy();
        };
        cPgDetailInfo.prototype.isActive = function () {
            return this._bIsActive;
        };
        cPgDetailInfo.prototype.visible = function () {
            return this._bIsActive;
        };
        cPgDetailInfo.prototype._setVisible = function (aIsVisible) {
            if (aIsVisible !== this._bIsActive) {
                this._bIsActive = aIsVisible;
                this.sendEventToEntity({
                    alEvent: 'alCheckPageVisible'
                });
            }
        };
        cPgDetailInfo.prototype._messageToParent = function (aMessage) {
            hx.log('CPgDetailInfo', '_messageToParent - this._calledPath : ' + this._calledPath);
            if (this._calledPath) {
                this.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: this._calledPath + '?' + aMessage,
                    me: this
                });
                delete this._calledPath;
                return true;
            }
            return false;
        };
        cPgDetailInfo.prototype._on_alKey = function (aEvent) {
            var consumed = false, k = hx.key, kc = aEvent.alKey;
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            var dsrvChlist = hx.svc.CH_LIST;
            var currentChannel = dsrvChlist.getCurrentChannel().channel, channelName = '';
            if (currentChannel) {
                channelName = currentChannel.name;
            }
            if (this._mainControl) {
                consumed = this._mainControl.doKey(kc);
                switch (kc) {
                    case k.KEY_0:
                    case k.KEY_1:
                    case k.KEY_2:
                    case k.KEY_3:
                    case k.KEY_4:
                    case k.KEY_5:
                    case k.KEY_6:
                    case k.KEY_7:
                    case k.KEY_8:
                    case k.KEY_9:
                    case k.KEY_UP:
                    case k.KEY_DOWN:
                    case k.KEY_RIGHT:
                    case k.KEY_LEFT:
                    case k.KEY_CHANNEL_UP:
                    case k.KEY_CHANNEL_DOWN:
                        consumed = true;
                        break;
                    default:
                        break;
                }
            }
            if (!consumed) {
                switch (kc) {
                    case k.KEY_BACK:
                        consumed = this._hideAndDestroy();
                        if (consumed) {
                            this._messageToParent('backToParent');
                            dsrvFrontPanel.clearFrontPanelText();
                        }
                        break;
                    case k.KEY_ESCAPE:
                        consumed = this._hideAndDestroy();
                        if (consumed) {
                            this._messageToParent('backToLive');
                            dsrvFrontPanel.clearFrontPanelText();
                        }
                        consumed = true;
                        break;
                    default:
                        break;
                }
            }
            hx.log('CPgDetailInfo', '_on_alKey consumed=' + consumed);
            return consumed;
        };
        cPgDetailInfo.prototype._on_alMessageToPage = function (aParam) {
            hx.log('CPgDetailInfo', '_on_alMessageToPage : ' + aParam.path);
            var path = aParam.path;
            switch (path) {
                case '/hubProxy':
                    this._calledPath = 'CPgHubProxy';
                    this._createAndShow();
                    break;
                case '/infoHub':
                case '/infoHubCatchup':
                    this._calledPath = 'CPgInfoHub';
                    this._createAndShow();
                    break;
                case '/iPlate':
                    this._calledPath = 'CPgIPlate';
                    this._createAndShow();
                    break;
                case '/playback':
                    this._show();
                    break;
                default:
                    break;
            }
        };
        cPgDetailInfo.prototype._createAndShow = function () {
            var param = hx.il.adhocPageParam.pop();
            if (param) {
                this._prevParam = param;
                this._createProgrammeDetail(param);
            }
        };
        cPgDetailInfo.prototype._reCreateAndShow = function () {
            this._createProgrammeDetail(this._prevParam);
        };
        cPgDetailInfo.prototype._createProgrammeDetail = function (aParam) {
            var _this = this;
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var dsrvChlist = hx.svc.CH_LIST;
            var dsrvAvCtrl = hx.svc.AV_CTRL;
            var dsrvParentalCtrl = hx.svc.PARENTAL_CTRL;
            var dsrvBridge = hx.svc.BRIDGE;
            this._hideAndDestroy();
            this._$showPages = this._$wrap.find('> .-afd.show').not('#main');
            if (this._$showPages.length) {
                this._$showPages.removeClass('show');
            }
            this._setVisible(true);
            var $page = $('<div>').css({
                'width': 'inherit',
                'height': 'inherit'
            });
            this._$wrap.append($page.addClass('show shown'));
            var TPlayPosition = {
                EBEGIN: 0,
                ELAST_POSITION: 1
            };
            var mainControl = new CLayeredGroupControl($page);
            var detail = hx.il.createProgrammeDetailForSugar({
                layeredGroupControl: mainControl,
                sugarPage: this,
                pdeItem: aParam.programmeDetailItem,
                fnDoHideOnOpen: aParam.fnDoHideOnOpen,
                onClosed: aParam.onClosed
            });
            detail.onRequestPlayback = function (item, detailDlg) {
                var fnCreateQueryPasswordDlg = function () {
                    var passwordDlg = prism.createQueryPasswordDlg(detailDlg._parent);
                    passwordDlg.setTextHtml(tr('LOC_ENTER_PASSWORD_ID'));
                    return passwordDlg;
                };
                var fnStartPlayback = function () {
                    var recordingItem = item.getSourceItem();
                    var items = [];
                    var startIndex = 0;
                    var seriesId, seriesList, id, i, length;
                    if (recordingItem.isSeries()) {
                        seriesId = recordingItem.getSeriesId();
                        seriesList = dsrvRecordings2.getSeriesList(seriesId);
                        id = recordingItem.getId();
                        if (seriesList.length !== 0) {
                            items = dsrvRecordings2.arrangeSeriesList(seriesList);
                            length = items.length;
                            for (i = 0; i < length; i += 1) {
                                if (id === items[i].getId()) {
                                    startIndex = i;
                                    break;
                                }
                            }
                        }
                        else {
                            items = [recordingItem];
                        }
                    }
                    else {
                        items = [recordingItem];
                    }
                    dsrvBridge.requestVideoPlayback(items, startIndex);
                };
                if (item.isLocked()) {
                    var passwordDlg = fnCreateQueryPasswordDlg();
                    passwordDlg.open({
                        dimm: true,
                        onPasswordEntered: function (password) {
                            var result = dsrvParentalCtrl.verifyParentalControlPIN({
                                password: password
                            });
                            if (result && (result.result === 0)) {
                                detailDlg._$self.find('.vod img').attr('src', item.getThumbnailUrl());
                                passwordDlg.close();
                                fnStartPlayback();
                            }
                            else {
                                passwordDlg.reset();
                            }
                        }
                    });
                }
                else {
                    fnStartPlayback();
                }
            };
            detail.onRequestLive = function (item) {
                var oipfChannel, changeable;
                oipfChannel = item.getChannelInstance();
                changeable = dsrvChlist.checkChannelChangable({
                    channel: oipfChannel
                });
                if (!changeable) {
                    if (hx.config.useTVStreaming === false) {
                        hx.msg('response', {
                            text: hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                            auto_close: true,
                            close_time: 3000
                        });
                    }
                    else {
                        hx.msg('response', {
                            text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                            auto_close: true,
                            close_time: 3000
                        });
                    }
                    return;
                }
                _this.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: 'CPgLiveController',
                    me: _this
                });
                dsrvAvCtrl.channelChange({
                    channel: oipfChannel,
                    group: aParam.channelGroup || ((oipfChannel.channelType === 0) ? 'TV' : 'RADIO')
                });
                _this._hideAndDestroy();
                if (_this._calledPath === 'CPgInfoHub') {
                    _this._messageToParent('backToParent');
                }
            };
            detail.onRequestClose = function () {
                _this._hideAndDestroy();
            };
            this._mainControl = mainControl;
        };
        cPgDetailInfo.prototype._hideAndDestroy = function () {
            this._setVisible(false);
            if (this._$showPages.length) {
                this._$showPages.addClass('show');
                this._$showPages = [];
            }
            if (this._mainControl) {
                this._mainControl._$self.removeClass('show');
                this._mainControl.destroy();
                delete this._mainControl;
                return true;
            }
            return false;
        };
        cPgDetailInfo.prototype._hide = function () {
            this._mainControl._$self.hide();
            this._setVisible(false);
        };
        cPgDetailInfo.prototype._show = function () {
            if (this._mainControl) {
                this._setVisible(true);
                this._mainControl._$self.show();
            }
        };
        return cPgDetailInfo;
    })(__PAGE_KERNEL__);
    return cPgDetailInfo;
});
