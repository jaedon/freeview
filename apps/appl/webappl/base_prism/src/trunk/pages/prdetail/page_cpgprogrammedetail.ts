// file name : pages/prdetail/CPgProgrammeDetail.js
/*jslint nomen:true, vars:true, browser:true*/
/*global hx*/
/*global $*/
/*global CLayeredGroupControl*/
///<reference path="../../def/sugar.d.ts"/>
///<reference path="../../def/prism.d.ts"/>
///<reference path="../../def/controls.d.ts"/>
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx: ifSugar;
class cPgDetailInfo extends __PAGE_KERNEL__ {
    _$wrap: JQuery;
    _$showPages: any;
    _bIsActive: boolean;
    _calledPath: string;
    _mainControl: any;
    _prevParam: any;

    constructor() {
        super();
    }
    create(param) {
        hx.al.compose(this);
        this._setVisible(false);
        this._$wrap = $('#wrap');
        this._$showPages = [];
        hx.logadd('CPgDetailInfo');
        hx.svc.RECORDINGS.addEventCb('ShowHDAlternativeRecordWindow', this, (aButtonInfo) => {
            if (this.getPageVisible(this.name) === true) {
                var dlg = prism.createHDAlternativeButtonDlg(null, aButtonInfo);
                dlg.open({
                    onClosed: () => {
                        this.setKeyHookingControl(dlg);
                    }
                });
                this.setKeyHookingControl(dlg, true);
            }
        });
    }
    destroy(param?) {
        this._setVisible(false);
        this._hideAndDestroy();
    }
    isActive() {
        return this._bIsActive;
    }
    visible() {
        return this._bIsActive;
    }
    _setVisible(aIsVisible: boolean) {
        if (aIsVisible !== this._bIsActive) {
            this._bIsActive = aIsVisible;
            this.sendEventToEntity({
                alEvent : 'alCheckPageVisible'
            });
        }
    }
    _messageToParent(aMessage): boolean {
        hx.log('CPgDetailInfo', '_messageToParent - this._calledPath : ' + this._calledPath);
        if (this._calledPath) {
            this.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : this._calledPath + '?' + aMessage,
                me : this
            });
            delete this._calledPath;
            return true;
        }
        return false;
    }
    _on_alKey(aEvent): boolean {
        var consumed: boolean = false, k = hx.key, kc: number = aEvent.alKey;
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        var dsrvChlist = hx.svc.CH_LIST;
        var currentChannel = dsrvChlist.getCurrentChannel().channel, channelName = '';
        if (currentChannel) {
            channelName = currentChannel.name;
        }
        if (this._mainControl) {
            consumed = this._mainControl.doKey(kc);
            switch(kc) {
                case k.KEY_0 :
                case k.KEY_1 :
                case k.KEY_2 :
                case k.KEY_3 :
                case k.KEY_4 :
                case k.KEY_5 :
                case k.KEY_6 :
                case k.KEY_7 :
                case k.KEY_8 :
                case k.KEY_9 :
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
            switch(kc) {
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
    }
    _on_alMessageToPage(aParam) {
        hx.log('CPgDetailInfo', '_on_alMessageToPage : ' + aParam.path);
        var path : string= aParam.path;

        switch( path ) {
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
    }
    _createAndShow() {
        var param = hx.il.adhocPageParam.pop();
        if (param) {
            this._prevParam = param;
            this._createProgrammeDetail(param);
        }
    }
    _reCreateAndShow() {
        this._createProgrammeDetail(this._prevParam);
    }
    _createProgrammeDetail(aParam) {
        var dsrvRecordings2 = hx.svc.RECORDINGS;
        var dsrvChlist = hx.svc.CH_LIST;
        var dsrvAvCtrl = hx.svc.AV_CTRL;
        var dsrvParentalCtrl = hx.svc.PARENTAL_CTRL ;
        var dsrvBridge = hx.svc.BRIDGE;
        this._hideAndDestroy();
        this._$showPages = this._$wrap.find('> .-afd.show').not('#main');
        if (this._$showPages.length) {
            this._$showPages.removeClass('show');
        }
        this._setVisible(true);
        var $page = $('<div>').css({
            'width' : 'inherit',
            'height' : 'inherit'
        });
        this._$wrap.append($page.addClass('show shown'));
        var TPlayPosition = {
            EBEGIN : 0,
            ELAST_POSITION : 1
        };
        var mainControl = new CLayeredGroupControl($page);
        var detail = hx.il.createProgrammeDetailForSugar({
            layeredGroupControl : mainControl,
            sugarPage : this,
            pdeItem : aParam.programmeDetailItem,
            fnDoHideOnOpen : aParam.fnDoHideOnOpen,
            onClosed : aParam.onClosed
        });
        detail.onRequestPlayback = (item, detailDlg) => {
            var fnCreateQueryPasswordDlg = function() {
                var passwordDlg = prism.createQueryPasswordDlg(detailDlg._parent);
                passwordDlg.setTextHtml(tr('LOC_ENTER_PASSWORD_ID'));
                return passwordDlg;
            };
            var fnStartPlayback = () => {
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
                        for ( i = 0; i < length; i += 1) {
                            if (id === items[i].getId()) {
                                startIndex = i;
                                break;
                            }
                        }
                    } else {
                        items = [recordingItem];
                    }
                } else {
                    items = [recordingItem];
                }
                dsrvBridge.requestVideoPlayback(items,startIndex);
            };
            if (item.isLocked()) {
                 var passwordDlg = fnCreateQueryPasswordDlg();
                 passwordDlg.open({
                     dimm : true,
                     onPasswordEntered : function(password) {
                         var result = dsrvParentalCtrl.verifyParentalControlPIN({
                             password : password
                         });
                         if (result && (result.result === 0)) {
                             detailDlg._$self.find('.vod img').attr('src', item.getThumbnailUrl());
                             passwordDlg.close();
                             fnStartPlayback();
                         } else {
                             passwordDlg.reset();
                         }
                     }
                 });
            } else {
                fnStartPlayback();
            }
        };
        detail.onRequestLive = (item) => {
            var oipfChannel, changeable;

            oipfChannel = item.getChannelInstance();
            changeable = dsrvChlist.checkChannelChangable({
                channel : oipfChannel
            });
            if (!changeable) {
                if (hx.config.useTVStreaming === false) {
                    hx.msg('response', {
                        text : hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                        auto_close : true,
                        close_time : 3000
                    });
                } else {
                    hx.msg('response', {
                        text : hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                        auto_close : true,
                        close_time : 3000
                    });
                }
                return;
            }

            this.sendEventToEntity({
                alEvent : 'alChangePage',
                target : 'CPgLiveController',
                me : this
            });
            dsrvAvCtrl.channelChange({
                channel : oipfChannel,
                group : aParam.channelGroup || ((oipfChannel.channelType === 0) ? 'TV' : 'RADIO')
            });
            this._hideAndDestroy();
            if (this._calledPath === 'CPgInfoHub') {
                this._messageToParent('backToParent');
            }
        };
        detail.onRequestClose = () => {
            this._hideAndDestroy();
        };
        this._mainControl = mainControl;
    }
    _hideAndDestroy() {
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
    }
    _hide() {
        this._mainControl._$self.hide();
        this._setVisible(false);
    }
    _show() {
        if (this._mainControl) {
            this._setVisible(true);
            this._mainControl._$self.show();
        }
    }
}
export = cPgDetailInfo;