/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvFrontPanel.js
/*jslint nomen:true, vars:true */
//    FRONT_PANEL: ['layer_domain/service/dl_dsrvFrontPanel.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("../../model/model");

enum eFRONTPANEL_DISPLAY {
    EFRONTPANEL_POWER = 0,
    EFRONTPANEL_TV = 1,
    EFRONTPANEL_RADIO = 2,
    EFRONTPANEL_REC = 3,
    EFRONTPANEL_PLAY = 4,
    EFRONTPANEL_PAUSE = 5,
    EFRONTPANEL_RESERVE = 6,
    EFRONTPANEL_SRS = 7,
    EFRONTPANEL_SRC_TS = 8,
    EFRONTPANEL_SRC_XT = 9,
    EFRONTPANEL_DOLBY = 10,
    EFRONTPANEL_HD = 11,
    EFRONTPANEL_1080P = 12,
    EFRONTPANEL_720P = 13,
    EFRONTPANEL_576P = 14,
    EFRONTPANEL_480P = 15,
    EFRONTPANEL_1080I = 16,
    EFRONTPANEL_576I = 17,
    EFRONTPANEL_480I = 18,
    EFRONTPANEL_DOLBY_PLUS = 19,
    EFRONTPANEL_DOLBY_PULSE = 20,
    EFRONTPANEL_WIFI = 21,
    EFRONTPANEL_USB = 22,
    EFRONTPANEL_HDD_EMPTY = 23,
    EFRONTPANEL_HDD_25_FULL = 24,
    EFRONTPANEL_HDD_50_FULL = 25,
    EFRONTPANEL_HDD_75_FULL = 26,
    EFRONTPANEL_HDD_100_FULL = 27,
    EFRONTPANEL_LINK = 28,
    EFRONTPANEL_MAIL = 29,
    EFRONTPANEL_RED_COLOR = 30,
    EFRONTPANEL_BLUE_COLOR = 31,
    EFRONTPANEL_ORANGE_COLOR = 32,
};
var currentChannel = null;
var mCurrentItem = null;
var mLastPlaybackLedId = null;
var mTimer : any = null;

//var util = dl.registerApiGroup('dsrvFrontPanel');
//var mLocalSystemOipfObj = util.getPluginInstance('LocalSystem');
var dsrvSettingUtil = hx.svc.SETTING_UTIL;
var dsrvPlayback = hx.svc.PLAYBACK;
var dsrvTsr = hx.svc.TSR;
var dsrvChlist = hx.svc.CH_LIST;
var dsrvAvCtrl = hx.svc.AV_CTRL;

class cDLFrontPanel extends __KERNEL__ implements ifSVCFrontPanel {
    private mLocalSystemOipfObj: any = this.getPluginInstance('LocalSystem');

    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('dsrvFrontPanel');
        // media Playback
        dsrvPlayback.addEventCb('PlayStatePlaying', this, (e) => {
            this.onPlayStatePlaying(e.item);
        });
        dsrvPlayback.addEventCb('PlayStatePaused', this, (e) => {
            this.onPlayStatePaused();
        });
        dsrvPlayback.addEventCb('PlaySpeedChanged', this, (e) => {
            this.onPlaySpeedChanged(e.speed, e.item, false);
        });
        dsrvPlayback.addEventCb('PlayStateStopped', this, (e) => {
            this.onPlayStateStopped(e.item);
        });
        dsrvPlayback.addEventCb('PlayStateFinished', this, (e) => {
            this.onPlayStateStopped(e.item);
        });
        dsrvPlayback.addEventCb('PlayStateError', this, (e) => {
            this.onPlayStateStopped();
        });
        // TSR Playback
        dsrvTsr.addEventCb('PlaySpeedChanged', this, (e) => {
            this.onPlaySpeedChanged(e.speed, true, e.isLiveTv);
        });
        dsrvTsr.addEventCb('LiveStarted', this, (e) => {
            this.onLiveStarted();
        });
        dsrvTsr.addEventCb('LiveStopping', this, (e) => {
            this.onLiveStopped();
        });
    }
    private fnSetFrontPanelText (aText, aAlign?) {
        var szOrgtext : string = aText;

        clearTimeout(mTimer);

        if (hx.config.frontPanel) {
            var bStr : boolean = (szOrgtext === '') ? true : false;
            var i : number = 0, chgText : string, bfind : boolean = false;

            hx.log("dsrvFrontPanel", 'fnSetFrontPanelText Org: ' + szOrgtext);
            if (bStr === false) {
                for ( i = 0; i < hx.config.frontPanel.lcdVfd16.length; i += 1) {
                    if (szOrgtext === hx.config.frontPanel.lcdVfd16[i]) {
                        if (hx.config.usefrontPanel === 'segment7') {
                            chgText = hx.config.frontPanel.segment7[i];
                            szOrgtext = chgText;
                        } else if (hx.config.usefrontPanel === 'lcdVfd10') {
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
                var size : number = szOrgtext.length;
                var space : string = '';
                var i : number, prefixLength : number = 0;
                if (size < 12) {
                    prefixLength = Math.floor((12 - size) / 2);
                    for ( i = 0; i < prefixLength; i += 1) {
                        space += ' ';
                    }
                    szOrgtext = space + szOrgtext;
                }
            }
        }
        dsrvSettingUtil.setFrontPanelString({
            text : szOrgtext
        });
        hx.log("dsrvFrontPanel", 'fnSetFrontPanelText : ' + szOrgtext);
    }

    private fnClearFrontPanelText () {
        dsrvSettingUtil.setFrontPanelString({
            text : ''
        });
    }
    private fnSetFrontPanelLed (aLedId) {
        if (hx.config.usefrontPanel === 'segment7') {
            return;
        }

        dsrvSettingUtil.setFrontPanelLed({
            ledId : aLedId,
            bEnable : true
        });
    }
    private fnClearFrontPanelLed (aLedId) {
        if (mLastPlaybackLedId !== null) {
            dsrvSettingUtil.setFrontPanelLed({
                ledId : aLedId,
                bEnable : false
            });
        }
    }
    private fnSetFrontPanelLedForPlayback (aLedId) {
        this.fnClearFrontPanelLed(mLastPlaybackLedId);
        this.fnSetFrontPanelLed(aLedId);
        mLastPlaybackLedId = aLedId;
    }
    private fnIsMpbOnGoing () {
        return dsrvPlayback.isMpbOnGoing();
    }

    onPlayStatePlaying (aItem) {
        if ( aItem instanceof __MODEL__.IRecordingItem) {
            this.fnSetFrontPanelText('PLAY');
            if (hx.config.usefrontPanel !== 'segment7') {
                mTimer = setTimeout( () => {
                    this.fnSetFrontPanelText(aItem.getTitle(), 'left');
                }, 2000);
            }
        } else {
            this.fnSetFrontPanelText('MEDIA PLAY');
        }
        this.fnSetFrontPanelLedForPlayback(eFRONTPANEL_DISPLAY.EFRONTPANEL_PLAY);
    }
    onPlayStatePaused () {
        // fnSetFrontPanelText('PAUSE');
        // fnSetFrontPanelLed(eFRONTPANEL_DISPLAY.EFRONTPANEL_PAUSE);
    }
    onPlaySpeedChanged (aSpeed, aItem, aBool, aIsLiveTv?) {
        var isMpbOnGoing : boolean = this.fnIsMpbOnGoing();
        var ulChannelNum : number = 0;
        var channelName : string = dsrvChlist.getCurrentChannel().channel.name;

        if (aSpeed !== undefined) {
            if (Math.abs(aSpeed) > 1) {
                var prefix : string = '';
                if (hx.config.usefrontPanel === 'segment7') {
                    prefix = aSpeed > 0 ? 'FF' : 'FB';
                } else {
                    prefix = aSpeed > 0 ? 'FFX' : 'FBX';
                }
                this.fnSetFrontPanelText(prefix + Math.abs(aSpeed));
            } else if (aSpeed === 0) {
                if (isMpbOnGoing === true) {
                    this.fnSetFrontPanelText('PAUSE');
                    this.fnSetFrontPanelLedForPlayback(eFRONTPANEL_DISPLAY.EFRONTPANEL_PAUSE);
                }
            } else if (aBool === true) {
                if (aSpeed === 1) {
                    this.fnSetFrontPanelText('PLAY');
                    this.fnSetFrontPanelText('PLAY');
                } else {
                    if (hx.config.usefrontPanel === 'segment7') {
                        ulChannelNum = dsrvChlist.getCurrentChannel().channel.majorChannel;
                        channelName = ulChannelNum.toString();
                    } else {
                        channelName = dsrvChlist.getCurrentChannel().channel.name;
                    }
                    this.fnSetFrontPanelText(channelName, 'left');
                }
            } else if (aSpeed === 1) {
                if (aItem instanceof __MODEL__.IRecordingItem) {
                    this.fnSetFrontPanelText('PLAY');
                    if (hx.config.usefrontPanel !== 'segment7') {
                        mTimer = setTimeout( () => {
                            this.fnSetFrontPanelText(aItem.getTitle(), 'left');
                        }, 2000);
                    }
                } else {
                    if (aItem === true) {
                        if (hx.config.usefrontPanel === 'segment7') {
                            ulChannelNum = dsrvChlist.getCurrentChannel().channel.majorChannel;
                            channelName = ulChannelNum.toString()
                        } else {
                            channelName = dsrvChlist.getCurrentChannel().channel.name;
                        }
                        // Pauses at Chase and Tsr
                        this.fnSetFrontPanelText(channelName, 'left');
                    } else {
                        // Pauses at VideoPlayback
                        this.fnSetFrontPanelText('MEDIA PLAY');
                    }
                }
            } else {
                if (aIsLiveTv) {
                    this.fnClearFrontPanelLed(mLastPlaybackLedId);
                }
            }
        }
    }
    onPlayStateStopped (aItem?) {
        var isMpbOnGoing : boolean = this.fnIsMpbOnGoing();
        if (isMpbOnGoing === false) {
            this.fnClearFrontPanelText();
            this.fnClearFrontPanelLed(mLastPlaybackLedId);
            if ( aItem instanceof __MODEL__.IRecordingItem) {
                this.fnSetFrontPanelText('-RECORDINGS-');
            }
        }
    }
    onLiveStarted () {
        var channelName : string = '';
        var ulChannelNum : number = 0;
        //at trickPlay
        this.fnClearFrontPanelText();
        if (hx.config.usefrontPanel === 'segment7') {
            ulChannelNum = dsrvChlist.getCurrentChannel().channel.majorChannel;
            channelName = ulChannelNum.toString();
        } else {
            channelName = dsrvChlist.getCurrentChannel().channel.name;
        }
        this.fnSetFrontPanelText(channelName, 'left');
    }
    onLiveStopped () {
        var channelName : string = '';
        var ulChannelNum : number = 0;

        if (hx.config.usefrontPanel === 'segment7') {
            ulChannelNum = dsrvChlist.getCurrentChannel().channel.majorChannel;
            channelName = ulChannelNum.toString();
        } else {
            channelName = dsrvChlist.getCurrentChannel().channel.name;
        }
        this.fnSetFrontPanelText(channelName, 'left');
    }

    setFrontPanelText  (aText : string, align?) : void {
        this.fnSetFrontPanelText(aText, align);
    }
    clearFrontPanelText () : void {
        this.fnClearFrontPanelText();
    }
    setFrontPanelLed (aLedId) : void {
        this.fnSetFrontPanelLed(aLedId);
    }
    clearFrontPanelLed () : void {
        this.fnClearFrontPanelLed(mLastPlaybackLedId);
    }
    setFrontPanelLedForPlayback (aLedId) : void {
        this.fnSetFrontPanelLedForPlayback(aLedId);
    }
}
export =  cDLFrontPanel;


