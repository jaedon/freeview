/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvFrontPanel.js
/*jslint nomen:true, vars:true */
//    FRONT_PANEL: ['layer_domain/service/dl_dsrvFrontPanel.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "../../model/model"], function (require, exports, __KERNEL__, __MODEL__) {
    var eFRONTPANEL_DISPLAY;
    (function (eFRONTPANEL_DISPLAY) {
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_POWER"] = 0] = "EFRONTPANEL_POWER";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_TV"] = 1] = "EFRONTPANEL_TV";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_RADIO"] = 2] = "EFRONTPANEL_RADIO";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_REC"] = 3] = "EFRONTPANEL_REC";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_PLAY"] = 4] = "EFRONTPANEL_PLAY";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_PAUSE"] = 5] = "EFRONTPANEL_PAUSE";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_RESERVE"] = 6] = "EFRONTPANEL_RESERVE";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_SRS"] = 7] = "EFRONTPANEL_SRS";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_SRC_TS"] = 8] = "EFRONTPANEL_SRC_TS";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_SRC_XT"] = 9] = "EFRONTPANEL_SRC_XT";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_DOLBY"] = 10] = "EFRONTPANEL_DOLBY";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_HD"] = 11] = "EFRONTPANEL_HD";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_1080P"] = 12] = "EFRONTPANEL_1080P";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_720P"] = 13] = "EFRONTPANEL_720P";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_576P"] = 14] = "EFRONTPANEL_576P";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_480P"] = 15] = "EFRONTPANEL_480P";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_1080I"] = 16] = "EFRONTPANEL_1080I";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_576I"] = 17] = "EFRONTPANEL_576I";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_480I"] = 18] = "EFRONTPANEL_480I";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_DOLBY_PLUS"] = 19] = "EFRONTPANEL_DOLBY_PLUS";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_DOLBY_PULSE"] = 20] = "EFRONTPANEL_DOLBY_PULSE";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_WIFI"] = 21] = "EFRONTPANEL_WIFI";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_USB"] = 22] = "EFRONTPANEL_USB";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_HDD_EMPTY"] = 23] = "EFRONTPANEL_HDD_EMPTY";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_HDD_25_FULL"] = 24] = "EFRONTPANEL_HDD_25_FULL";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_HDD_50_FULL"] = 25] = "EFRONTPANEL_HDD_50_FULL";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_HDD_75_FULL"] = 26] = "EFRONTPANEL_HDD_75_FULL";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_HDD_100_FULL"] = 27] = "EFRONTPANEL_HDD_100_FULL";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_LINK"] = 28] = "EFRONTPANEL_LINK";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_MAIL"] = 29] = "EFRONTPANEL_MAIL";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_RED_COLOR"] = 30] = "EFRONTPANEL_RED_COLOR";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_BLUE_COLOR"] = 31] = "EFRONTPANEL_BLUE_COLOR";
        eFRONTPANEL_DISPLAY[eFRONTPANEL_DISPLAY["EFRONTPANEL_ORANGE_COLOR"] = 32] = "EFRONTPANEL_ORANGE_COLOR";
    })(eFRONTPANEL_DISPLAY || (eFRONTPANEL_DISPLAY = {}));
    ;
    var currentChannel = null;
    var mCurrentItem = null;
    var mLastPlaybackLedId = null;
    var mTimer = null;
    //var util = dl.registerApiGroup('dsrvFrontPanel');
    //var mLocalSystemOipfObj = util.getPluginInstance('LocalSystem');
    var dsrvSettingUtil = hx.svc.SETTING_UTIL;
    var dsrvPlayback = hx.svc.PLAYBACK;
    var dsrvTsr = hx.svc.TSR;
    var dsrvChlist = hx.svc.CH_LIST;
    var dsrvAvCtrl = hx.svc.AV_CTRL;
    var cDLFrontPanel = (function (_super) {
        __extends(cDLFrontPanel, _super);
        function cDLFrontPanel(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mLocalSystemOipfObj = this.getPluginInstance('LocalSystem');
            hx.logadd('dsrvFrontPanel');
            // media Playback
            dsrvPlayback.addEventCb('PlayStatePlaying', this, function (e) {
                _this.onPlayStatePlaying(e.item);
            });
            dsrvPlayback.addEventCb('PlayStatePaused', this, function (e) {
                _this.onPlayStatePaused();
            });
            dsrvPlayback.addEventCb('PlaySpeedChanged', this, function (e) {
                _this.onPlaySpeedChanged(e.speed, e.item, false);
            });
            dsrvPlayback.addEventCb('PlayStateStopped', this, function (e) {
                _this.onPlayStateStopped(e.item);
            });
            dsrvPlayback.addEventCb('PlayStateFinished', this, function (e) {
                _this.onPlayStateStopped(e.item);
            });
            dsrvPlayback.addEventCb('PlayStateError', this, function (e) {
                _this.onPlayStateStopped();
            });
            // TSR Playback
            dsrvTsr.addEventCb('PlaySpeedChanged', this, function (e) {
                _this.onPlaySpeedChanged(e.speed, true, e.isLiveTv);
            });
            dsrvTsr.addEventCb('LiveStarted', this, function (e) {
                _this.onLiveStarted();
            });
            dsrvTsr.addEventCb('LiveStopping', this, function (e) {
                _this.onLiveStopped();
            });
        }
        cDLFrontPanel.prototype.fnSetFrontPanelText = function (aText, aAlign) {
            var szOrgtext = aText;
            clearTimeout(mTimer);
            if (hx.config.frontPanel) {
                var bStr = (szOrgtext === '') ? true : false;
                var i = 0, chgText, bfind = false;
                hx.log("dsrvFrontPanel", 'fnSetFrontPanelText Org: ' + szOrgtext);
                if (bStr === false) {
                    for (i = 0; i < hx.config.frontPanel.lcdVfd16.length; i += 1) {
                        if (szOrgtext === hx.config.frontPanel.lcdVfd16[i]) {
                            if (hx.config.usefrontPanel === 'segment7') {
                                chgText = hx.config.frontPanel.segment7[i];
                                szOrgtext = chgText;
                            }
                            else if (hx.config.usefrontPanel === 'lcdVfd10') {
                                chgText = hx.config.frontPanel.lcdVfd10[i];
                                szOrgtext = chgText;
                            }
                            bfind = true;
                            break;
                        }
                    }
                }
            }
            if (hx.config.usefrontPanel !== 'segment7') {
                if (aAlign !== 'left') {
                    var size = szOrgtext.length;
                    var space = '';
                    var i, prefixLength = 0;
                    if (size < 12) {
                        prefixLength = Math.floor((12 - size) / 2);
                        for (i = 0; i < prefixLength; i += 1) {
                            space += ' ';
                        }
                        szOrgtext = space + szOrgtext;
                    }
                }
            }
            dsrvSettingUtil.setFrontPanelString({
                text: szOrgtext
            });
            hx.log("dsrvFrontPanel", 'fnSetFrontPanelText : ' + szOrgtext);
        };
        cDLFrontPanel.prototype.fnClearFrontPanelText = function () {
            dsrvSettingUtil.setFrontPanelString({
                text: ''
            });
        };
        cDLFrontPanel.prototype.fnSetFrontPanelLed = function (aLedId) {
            if (hx.config.usefrontPanel === 'segment7') {
                return;
            }
            dsrvSettingUtil.setFrontPanelLed({
                ledId: aLedId,
                bEnable: true
            });
        };
        cDLFrontPanel.prototype.fnClearFrontPanelLed = function (aLedId) {
            if (mLastPlaybackLedId !== null) {
                dsrvSettingUtil.setFrontPanelLed({
                    ledId: aLedId,
                    bEnable: false
                });
            }
        };
        cDLFrontPanel.prototype.fnSetFrontPanelLedForPlayback = function (aLedId) {
            this.fnClearFrontPanelLed(mLastPlaybackLedId);
            this.fnSetFrontPanelLed(aLedId);
            mLastPlaybackLedId = aLedId;
        };
        cDLFrontPanel.prototype.fnIsMpbOnGoing = function () {
            return dsrvPlayback.isMpbOnGoing();
        };
        cDLFrontPanel.prototype.onPlayStatePlaying = function (aItem) {
            var _this = this;
            if (aItem instanceof __MODEL__.IRecordingItem) {
                this.fnSetFrontPanelText('PLAY');
                if (hx.config.usefrontPanel !== 'segment7') {
                    mTimer = setTimeout(function () {
                        _this.fnSetFrontPanelText(aItem.getTitle(), 'left');
                    }, 2000);
                }
            }
            else {
                this.fnSetFrontPanelText('MEDIA PLAY');
            }
            this.fnSetFrontPanelLedForPlayback(4 /* EFRONTPANEL_PLAY */);
        };
        cDLFrontPanel.prototype.onPlayStatePaused = function () {
            // fnSetFrontPanelText('PAUSE');
            // fnSetFrontPanelLed(eFRONTPANEL_DISPLAY.EFRONTPANEL_PAUSE);
        };
        cDLFrontPanel.prototype.onPlaySpeedChanged = function (aSpeed, aItem, aBool, aIsLiveTv) {
            var _this = this;
            var isMpbOnGoing = this.fnIsMpbOnGoing();
            var ulChannelNum = 0;
            var channelName = dsrvChlist.getCurrentChannel().channel.name;
            if (aSpeed !== undefined) {
                if (Math.abs(aSpeed) > 1) {
                    var prefix = '';
                    if (hx.config.usefrontPanel === 'segment7') {
                        prefix = aSpeed > 0 ? 'FF' : 'FB';
                    }
                    else {
                        prefix = aSpeed > 0 ? 'FFX' : 'FBX';
                    }
                    this.fnSetFrontPanelText(prefix + Math.abs(aSpeed));
                }
                else if (aSpeed === 0) {
                    if (isMpbOnGoing === true) {
                        this.fnSetFrontPanelText('PAUSE');
                        this.fnSetFrontPanelLedForPlayback(5 /* EFRONTPANEL_PAUSE */);
                    }
                }
                else if (aBool === true) {
                    if (aSpeed === 1) {
                        this.fnSetFrontPanelText('PLAY');
                        this.fnSetFrontPanelText('PLAY');
                    }
                    else {
                        if (hx.config.usefrontPanel === 'segment7') {
                            ulChannelNum = dsrvChlist.getCurrentChannel().channel.majorChannel;
                            channelName = ulChannelNum.toString();
                        }
                        else {
                            channelName = dsrvChlist.getCurrentChannel().channel.name;
                        }
                        this.fnSetFrontPanelText(channelName, 'left');
                    }
                }
                else if (aSpeed === 1) {
                    if (aItem instanceof __MODEL__.IRecordingItem) {
                        this.fnSetFrontPanelText('PLAY');
                        if (hx.config.usefrontPanel !== 'segment7') {
                            mTimer = setTimeout(function () {
                                _this.fnSetFrontPanelText(aItem.getTitle(), 'left');
                            }, 2000);
                        }
                    }
                    else {
                        if (aItem === true) {
                            if (hx.config.usefrontPanel === 'segment7') {
                                ulChannelNum = dsrvChlist.getCurrentChannel().channel.majorChannel;
                                channelName = ulChannelNum.toString();
                            }
                            else {
                                channelName = dsrvChlist.getCurrentChannel().channel.name;
                            }
                            // Pauses at Chase and Tsr
                            this.fnSetFrontPanelText(channelName, 'left');
                        }
                        else {
                            // Pauses at VideoPlayback
                            this.fnSetFrontPanelText('MEDIA PLAY');
                        }
                    }
                }
                else {
                    if (aIsLiveTv) {
                        this.fnClearFrontPanelLed(mLastPlaybackLedId);
                    }
                }
            }
        };
        cDLFrontPanel.prototype.onPlayStateStopped = function (aItem) {
            var isMpbOnGoing = this.fnIsMpbOnGoing();
            if (isMpbOnGoing === false) {
                this.fnClearFrontPanelText();
                this.fnClearFrontPanelLed(mLastPlaybackLedId);
                if (aItem instanceof __MODEL__.IRecordingItem) {
                    this.fnSetFrontPanelText('-RECORDINGS-');
                }
            }
        };
        cDLFrontPanel.prototype.onLiveStarted = function () {
            var channelName = '';
            var ulChannelNum = 0;
            //at trickPlay
            this.fnClearFrontPanelText();
            if (hx.config.usefrontPanel === 'segment7') {
                ulChannelNum = dsrvChlist.getCurrentChannel().channel.majorChannel;
                channelName = ulChannelNum.toString();
            }
            else {
                channelName = dsrvChlist.getCurrentChannel().channel.name;
            }
            this.fnSetFrontPanelText(channelName, 'left');
        };
        cDLFrontPanel.prototype.onLiveStopped = function () {
            var channelName = '';
            var ulChannelNum = 0;
            if (hx.config.usefrontPanel === 'segment7') {
                ulChannelNum = dsrvChlist.getCurrentChannel().channel.majorChannel;
                channelName = ulChannelNum.toString();
            }
            else {
                channelName = dsrvChlist.getCurrentChannel().channel.name;
            }
            this.fnSetFrontPanelText(channelName, 'left');
        };
        cDLFrontPanel.prototype.setFrontPanelText = function (aText, align) {
            this.fnSetFrontPanelText(aText, align);
        };
        cDLFrontPanel.prototype.clearFrontPanelText = function () {
            this.fnClearFrontPanelText();
        };
        cDLFrontPanel.prototype.setFrontPanelLed = function (aLedId) {
            this.fnSetFrontPanelLed(aLedId);
        };
        cDLFrontPanel.prototype.clearFrontPanelLed = function () {
            this.fnClearFrontPanelLed(mLastPlaybackLedId);
        };
        cDLFrontPanel.prototype.setFrontPanelLedForPlayback = function (aLedId) {
            this.fnSetFrontPanelLedForPlayback(aLedId);
        };
        return cDLFrontPanel;
    })(__KERNEL__);
    return cDLFrontPanel;
});
