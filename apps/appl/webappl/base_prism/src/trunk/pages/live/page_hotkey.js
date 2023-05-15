var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "layer_domain/service/dl_dsrvNetflix"], function (require, exports, __PAGE_KERNEL__, __NETFLIX__) {
    var cCPgHotkey = (function (_super) {
        __extends(cCPgHotkey, _super);
        function cCPgHotkey() {
            _super.call(this);
            this._multiCustomMsgHandle = null;
        }
        cCPgHotkey.prototype.create = function () {
            hx.al.compose(this);
            hx.logadd('CPgHotkey');
        };
        cCPgHotkey.prototype.destroy = function () {
        };
        cCPgHotkey.prototype._on_alKey = function (aParam) {
            var that = this;
            var notifierVisible = that.getPageVisible('CPgNotifier');
            var messageOpened = hx.msg.isOpend();
            var buttonDlgOpened = prism.CButtonDlg.isOpened();
            var queryPasswordDlgOpened = prism.CQueryPasswordDlg.isOpened();
            var selectionDlgOpened = prism.CSelectionDlg.isOpened();
            var globalDialogOpened = prism.isGlobalDialogOpened();
            hx.log('CPgHotkey', 'key : ' + aParam.alKey);
            if (notifierVisible) {
                hx.log('CPgHotkey', 'notifierVisible');
            }
            if (messageOpened) {
                hx.log('CPgHotkey', 'messageOpened');
                if (hx.msg.isOpendWithName('pinPC') === true) {
                    hx.log('CPgHotkey', 'messageOpened : but ignore pinPC');
                    messageOpened = false;
                }
            }
            if (buttonDlgOpened) {
                hx.log('CPgHotkey', 'buttonDlgOpened');
            }
            if (queryPasswordDlgOpened) {
                hx.log('CPgHotkey', 'queryPasswordDlgOpened');
            }
            if (selectionDlgOpened) {
                hx.log('CPgHotkey', 'selectionDlgOpened');
            }
            if (globalDialogOpened) {
                hx.log('CPgHotkey', 'globalDialogOpened');
            }
            var dlgOpened = notifierVisible || messageOpened || buttonDlgOpened || queryPasswordDlgOpened || selectionDlgOpened || globalDialogOpened;
            var bAccessiblePage = false;
            var currentPage = this.getCenterPageName();
            switch (currentPage) {
                case "CPgLiveController":
                case "CPgRecordings":
                case "CPgSettings":
                case "CPgGuide":
                case "CPgMediaCenter":
                case "CPgVideo":
                case "CPgMusic":
                case "CPgPhoto":
                case "CPgOnDemand":
                case "CPgTopPicks":
                    bAccessiblePage = true;
                    break;
                default:
                    break;
            }
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                    if (!hx.il.isInWizardPage && !dlgOpened) {
                        if (!bAccessiblePage) {
                            hx.log('CPgHotkey', 'This page(' + currentPage + ') is not accessible for HotKey');
                            break;
                        }
                        if (hx.svc.STATISTICS) {
                            hx.svc.STATISTICS.statisticsLog({
                                "CODE": "OSDIN",
                                "TRS": "I",
                                "OSD": "CPgHOME"
                            });
                        }
                        that.sendEventToEntity({
                            alEvent: 'alMessageToPage',
                            //target : 'CPgHub?show=toggle'
                            target: 'CPgHubProxy?show=toggle'
                        });
                    }
                    break;
                case hx.key.KEY_ESCAPE:
                    hx.svc.TV_PORTAL.destroyActiveApp();
                    break;
                case hx.key.KEY_GUIDE:
                    if (!hx.il.isInWizardPage) {
                        if (!bAccessiblePage) {
                            hx.log('CPgHotkey', 'This page(' + currentPage + ') is not accessible for HotKey');
                            break;
                        }
                        var result = hx.svc.CH_LIST.getChannelCount();
                        hx.log('CPgHotkey', 'getChannelCount');
                        if (result.chCnt === 0) {
                            that.sendEventToEntity({
                                'alEvent': 'alChangePage',
                                'target': 'CPgLiveController',
                                'me': that
                            });
                            that.sendEventToEntity({
                                alEvent: 'alMessageToPage',
                                target: 'CPgLiveController?nochannel'
                            });
                        }
                        else {
                            that._pageChange('CPgGuide');
                        }
                    }
                    break;
                case hx.key.KEY_PLAYER:
                    if (!hx.il.isInWizardPage) {
                        if (!bAccessiblePage) {
                            hx.log('CPgHotkey', 'This page(' + currentPage + ') is not accessible for HotKey');
                            break;
                        }
                        that._pageChange('CPgRecordings');
                    }
                    break;
                case hx.key.KEY_ON_DEMAND:
                    if (!hx.il.isInWizardPage) {
                        if (!bAccessiblePage) {
                            hx.log('CPgHotkey', 'This page(' + currentPage + ') is not accessible for HotKey');
                            break;
                        }
                        // USB URL Launcher : /url/url_hbbtv.txt
                        var svcSettingUtil = hx.svc.SETTING_UTIL;
                        var svcAppMgr = hx.svc.APP_MGR;
                        var hbbUrl = svcSettingUtil.getHbbTvUrl();
                        hx.log('CPgHotkey', '############################## hbbUrl is : ' + hbbUrl);
                        if (hbbUrl) {
                            svcAppMgr.createPortalApp();
                        }
                        else {
                            that._pageChange('CPgTopPicks', '/ondemand');
                        }
                    }
                    break;
                case hx.key.KEY_LIST:
                    location.href = 'test/testpage.html';
                    break;
                case hx.key.KEY_MULTICUSTOM:
                    that._getRCUCustomCode('message');
                    break;
                case hx.key.KEY_MULTICUSTOM_SUCCESS:
                    that._getRCUCustomCode('success');
                    break;
                case hx.key.KEY_MULTICUSTOM_FAIL:
                    that._getRCUCustomCode('failure');
                    break;
                case hx.key.KEY_POWER:
                    // power off
                    if (hx.il.isGoingOperationFromStandby) {
                        hx.log('error', ' [HOTKEY] cannot go to standby cos hx.il.isGoingOperationFromStandby');
                        break;
                    }
                    that.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgStandby',
                        me: that
                    });
                    break;
                case hx.key.KEY_NETFLIX:
                    if (hx.config.netflix) {
                        if (hx.il.isCriticalTasking() || hx.il.isInWizardPage || dlgOpened) {
                            hx.log('CPgHotkey', 'not allowed while Critical Task, Wizard, Dialog is going on.');
                            break;
                        }
                        if (!bAccessiblePage) {
                            hx.log('CPgHotkey', 'This page(' + currentPage + ') is not accessible for HotKey');
                            break;
                        }
                        hx.il.launchApp("Netflix", undefined, undefined, that, __NETFLIX__.START_TYPE.NETFLIX_BUTTON);
                    }
                    break;
                default:
                    return false;
            }
            return true;
        };
        cCPgHotkey.prototype._multiCustomMsgCreate = function (aCustomCode) {
            var text = hx.l('LOC_MULTICUSTOMMSG_ID') + ' ' + aCustomCode;
            this._multiCustomMsgHandle = hx.msg('response', {
                text: text,
                auto_close: false
            });
        };
        cCPgHotkey.prototype._multiCustomMsgClose = function () {
            if (this._multiCustomMsgHandle !== null) {
                hx.msg.close('response', this._multiCustomMsgHandle);
                this._multiCustomMsgHandle = null;
            }
        };
        cCPgHotkey.prototype._multiCustomResultMsg = function (aResult, aCustomCode) {
            var text = '';
            if (aResult === true) {
                text = hx.l('LOC_MULTICUSTOMSUCCESSMSG_ID') + ' ' + aCustomCode;
            }
            else {
                text = hx.l('LOC_MULTICUSTOMFAILMSG_ID');
            }
            hx.msg('response', {
                text: text,
                auto_close: true,
                close_time: 3000
            });
        };
        cCPgHotkey.prototype._getRCUCustomCode = function (aCommand) {
            var retObj, rcuCustomCode = '';
            retObj = hx.svc.SETTING_UTIL.getRCUCustomCode();
            if (retObj !== undefined) {
                rcuCustomCode = retObj.rcuCustomCode;
            }
            if (aCommand === 'message') {
                this._multiCustomMsgClose();
                this._multiCustomMsgCreate(rcuCustomCode);
            }
            else if (aCommand === 'success') {
                this._multiCustomMsgClose();
                this._multiCustomResultMsg(true, rcuCustomCode);
            }
            else if (aCommand === 'failure') {
                this._multiCustomMsgClose();
                this._multiCustomResultMsg(false, rcuCustomCode);
            }
        };
        cCPgHotkey.prototype._getNetworkAvailable = function () {
            return hx.svc.SETTING_UTIL.getNetworkAvailable();
        };
        cCPgHotkey.prototype._isDnsAvailable = function () {
            var address = hx.svc.SETTING_UTIL.getallAddress(), available = false;
            if (address) {
                if (address.dns1st && (address.dns1st !== '0.0.0.0')) {
                    available = true;
                }
                if (address.dns2nd && (address.dns2nd !== '0.0.0.0')) {
                    available = true;
                }
            }
            return available;
        };
        cCPgHotkey.prototype._getCheckUpTvAppsHelp = function () {
            return hx.svc.WEB_STORAGE.getHelp();
        };
        cCPgHotkey.prototype._setCheckUpTvAppsHelp = function (aParam) {
            return hx.svc.WEB_STORAGE.setHelp({
                tvapps: aParam
            });
        };
        /**
         * # if there is no channels, target page does not be changed.
         */
        cCPgHotkey.prototype._pageChange = function (aPage, aPath) {
            //if (hx.svc.TSR.isTsrOngoing() || hx.svc.PLAYBACK.isMpbOnGoing()) {
            //    hx.svc.BRIDGE.requestStopPlayback();
            //}
            this.sendEventToEntity({
                'alEvent': 'alChangePage',
                'target': aPage + (aPath || '') + '?refresh=true',
                'me': this
            });
        };
        return cCPgHotkey;
    })(__PAGE_KERNEL__);
    return cCPgHotkey;
});
