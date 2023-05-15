// file name : pages/main/main.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../def/prism.d.ts"/>
///<reference path="../../def/sugar.d.ts"/>
import __PAGE_KERNEL__ = require("pages/page_kernel");
import __MODEL__ = require("model/model");
import __NETFLIX__ = require("layer_domain/service/dl_dsrvNetflix");

declare module String{
    export var format:any;
}

declare var hx: ifSugar;
class cPgMain extends __PAGE_KERNEL__ {
    TPageState = {
        EIdle : 0,
        EMusicPlaybackOpened : 1,
        EVideoPlaybackOpened : 2,
        EPhotoPlaybackOpened : 3
    };
    THMXPhysicalStorageUsage = {
        EUsageUnknown : 0,
        EUsagePVR : 1,
        EUsageSubPVR : 2,
        EUsageStorage : 3
    };
    TPlayType = {
        ELive : 0,
        ETsr : 1,
        EChase : 2,
        EUser : 3,
        EMedia : 4,
        ENone : 5
    };
    TPlaybackStatus = {
        EPaused : 'PAUSED',
        ELoading : 'LOADING',
        EPlaying : 'PLAYING',
        EEndOfFile : 'EOF',
        EStopped : 'STOPPED',
        EError : 'ERROR'
    };
    TDialStatus = {
        ERuning : 1,
        EStop : 0
    }
    mCurrentPageState: any;
    mCurrentPlayType: any;
    mPlayback = null;
    mCurrentPlaybackItem = null;
    mCurrentTrickMode = null;
    mLastNotiLiveBroadcastTime = null;
    mPlayNext = null;
    mNetworkAvailable = false;
    mOldErrorTime = 0;
    mCurrentPositon = 0;
    mBlockRecKey = false;
    mBlockLive = false;
    mForcedBlock = false;
    //mIsStoppingAV = false;
    //mIsRequestStop = false;
    mForcePlayToStop = false;
    mForcePlayToStopTimer = null;
    EREPEAT_ALL: string = '0';
    EREPEAT_NO: string = '1';
    EREPEAT_ONE: string = '2';
    _lastToggleTime: number = 0;
    _mainPage: any;
    _$main: JQuery;
    _$wrap: JQuery;
    _$wrap_smt: JQuery;
    _subToggleObj: any;
    _audToggleObj: any;
    _statusMsg: any;
    _closeNetflixPW: Function;
    _checkPrivacyVersion: Function;
    _showPWForNetfixLaunch: Function;
    _onRequestVideoPlayback: Function;
    _onRequestInstanceRecording: Function;
    _doCreateTvOption: Function;
    _usbWizardSelectionDlg: any;
    _notiSatIpMsg: any;
    _tvOptionDlg: any;
    _preMessage: string;
    _HbbTvAppLoaded : boolean = false;
    _pinObjForNetflix: any;
    _ppDlg: any;
    constructor() {
        super();
    }
    create() {
        hx.logadd("CPgMain");
        this.mCurrentPageState = this.TPageState.EIdle;
        this.mCurrentPlayType = this.TPlayType.ENone;
        var $main, $wrap = $('#wrap'), $wrap_smt = $('#wrap_smt');
        var mainPage = prism.createMainPage($wrap);
        mainPage.removeLayer = (fromTo) => {
            var isRemoveAll = function() {
                return !!(fromTo === 'videoPlayback' || fromTo === 'musicPlayback' || fromTo === 'photoPlayback');
            }
            if (isRemoveAll()) {
                while (mainPage.stackSize()) {
                    CLayeredGroupControl.prototype.removeLayer.call(mainPage);
                }
            } else {
                CLayeredGroupControl.prototype.removeLayer.call(mainPage);
            }
        };
        mainPage.draw();
        $main = $('#main');
        this._mainPage = mainPage;
        this._$main = $main;
        this._$wrap = $wrap;
        this._$wrap_smt = $wrap_smt;
        this._init();
        hx.al.compose(this);
        return this;
    }
    destroy() {
        this._mainPage.destroy();
        delete this._mainPage;
    }
    isActive() {
        return true;
    }
    _init() {
        var that = this;
        var CDlnaItem = __MODEL__.cDlnaItem;
        var CTsrItem = __MODEL__.CTsrItem;
        var dsrvAppMgr = hx.svc.APP_MGR;
        var dsrvAvCtrl = hx.svc.AV_CTRL;
        var dsrvBridge = hx.svc.BRIDGE;
        var dsrvChlist = hx.svc.CH_LIST;
        var dsrvDial = hx.svc.DIAL;
        var dsrvDmr = hx.svc.DMR;
        var dsrvPlayback = hx.svc.PLAYBACK;
        var dsrvRecordings = hx.svc.RECORDINGS;
        var dsrvSettingUtil = hx.svc.SETTING_UTIL;
        var dsrvStorageUtil = hx.svc.STORAGE_UTIL;
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        var dsrvTsr = hx.svc.TSR;
        var dsrvWebStorage = hx.svc.WEB_STORAGE;
        var dsrvHistory = hx.svc.HISTORY;
        var dsrvStatistics = hx.svc.STATISTICS;
        var dsrvTVPortal = hx.svc.TV_PORTAL;
        var dsrvIpEpg = hx.svc.IP_EPG;
        var dsrvNetflix = hx.svc.NETFLIX;
        var dsrvVA = hx.svc.VOICE_ASSISTANCE;
        this.mNetworkAvailable = dsrvSettingUtil.getNetworkAvailable();

        prism.CDialog.showMsgbox = function(aFinished) {
            hx.svc.SETTING_UTIL.showMsgbox();
            aFinished();
        };

        var fnDestroyAllApps = function(turnOnLive?) {
            dsrvAppMgr.destroyAllApps();
            dsrvTVPortal.destroyActiveApp();
            //dsrvAppMgr.destroyHbbTVApplication();
            if (turnOnLive) {
                if (dsrvAvCtrl.isIdle()) {
                    dsrvAppMgr.createHbbTVApplication(false);
                    dsrvAvCtrl.unBlockAV();
                    if (!dsrvAvCtrl.lastChannel()) {
                        dsrvAvCtrl.availableChannel();
                    }
                }
            }
        };
        // dsrvAvCtrl
        var fnStopAv = function(aCallback) {
            dsrvAvCtrl.blockAV({
                'cb' : aCallback
            });
        };
        var fnRestartLive = function() {
            if (hx.config.netflix && dsrvNetflix.isRunning()) {
                hx.log('CPgMain', 'Netflix is running!! Do not restart Live!!');
                return;
            }
            if (that._HbbTvAppLoaded) {
                hx.log('CPgMain', 'Application is running!! Do not restart Live!!');
                return;
            }
            dsrvAvCtrl.unBlockAV();
            var result = dsrvAvCtrl.lastChannel();
            if (result === false) {
                dsrvAvCtrl.availableChannel();
            }
        };
        var fnGetSupportedSubtitles = function() {
            var supportedSubtitles = dsrvAvCtrl.getMediaSubtitleSupportLanguages();
            hx.log('CPgMain', hx.util.format('fnGetSupportedSubtitles - supportedSubtitles = {0}', supportedSubtitles));
            return supportedSubtitles;
        };
        var fnGetSubtitleLanguage = function() {
            var language = dsrvAvCtrl.getMediaSubtitleLanguage();
            hx.log('CPgMain', hx.util.format('fnGetSubtitleLanguage - language = {0}', language));
            return language;
        };
        var fnGetSubtitleSyncTime = function() {
            var syncTime = dsrvAvCtrl.getMediaSubtitleSyncTime();
            hx.log('CPgMain', hx.util.format('fnGetSubtitleSyncTime - syncTime = {0}', syncTime));
            return syncTime;
        };
        var fnGetSubtitleFontSize = function() {
            var fontSize = dsrvAvCtrl.getMediaSubtitleFontSize();
            hx.log('CPgMain', hx.util.format('fnGetSubtitleFontSize - fontSize = {0}', fontSize));
            return fontSize;
        };
        var fnGetSubtitlePosition = function() {
            var position = dsrvAvCtrl.getMediaSubtitleTextPosition();
            hx.log('CPgMain', hx.util.format('fnGetSubtitlePosition - position = {0}', position));
            return position;
        };
        var fnGetSubtitleDisplay = function() {
            var bDisplay = dsrvAvCtrl.getMediaSubtitleDisplay();
            hx.log('CPgMain', hx.util.format('fnGetSubtitleDisplay - bDisplay = {0}', bDisplay));
            return bDisplay;
        };
        var fnGetPictureInfo = function() {
            var info = dsrvSettingUtil.getPictureInfo();
            hx.log('CPgMain', hx.util.format('fnGetPictureInfo'));
            return info;
        };
        var fnSetTvOptionInfo = function(aInfo) {
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            var dsrvAvCtrl = hx.svc.AV_CTRL;
            var dsrvWebStorage = hx.svc.WEB_STORAGE;
            var resolution = aInfo.resolution;
            var display = aInfo.display;
            var audio = aInfo.audio;
            var subtitle = aInfo.subtitle;
            var repeatOption = aInfo.repeatOption;
            var _audioModeHdmi, _audioModePcm, _audioType, _lipsyncDelay;
            var retObjAudio = dsrvSettingUtil.getAudioInfo({
                cb : undefined
            });
            _audioModeHdmi = retObjAudio.audioModeHdmi;
            _audioModePcm = retObjAudio.audioModePcm;
            _audioType = retObjAudio.audioType;
            _lipsyncDelay = retObjAudio.lipSyncDelay;

            if (resolution && display) {
                hx.log('CPgMain', hx.util.format('doSetTOptionInfo - resolution = {0}, display = {1}', resolution, display));
                dsrvSettingUtil.setPictureInfo({
                    curResolution : resolution.nextValue,
                    videoDisplayFormat : display.nextValue
                });
            }
            if (audio !== undefined) {
                hx.log('CPgMain', hx.util.format('doSetTvOptionInfo - position = {0}', audio));
                if(ENV.op === ENV.listOP.UKDTT){
                    if (audio.nextValue || audio.prevValue) {
                        dsrvSettingUtil.setAudioInfo({audioDescription : !!audio.nextValue, audioModePcm : _audioModePcm, audioModeHdmi : _audioModeHdmi, audioType : _audioType, lipSyncDelay : _lipsyncDelay});
                    }
                } else{
                    dsrvAvCtrl.setAudioComponent({
                        off : audio.prevValue,
                        on : audio.nextValue
                    });
                }
            }
            if (subtitle !== undefined) {
                hx.log('CPgMain', hx.util.format('doSetTvOptionInfo - syncTime = {0}', subtitle));
                if(subtitle.bMedia === false) {
                    if(ENV.op === ENV.listOP.UKDTT) {
                        if (subtitle.nextValue || subtitle.prevValue) {
                            dsrvSettingUtil.setSubtitleEnabled(!!subtitle.nextValue);
                            if (!subtitle.nextValue) {
                                dsrvAvCtrl.offSubtitleComponent();
                            } else {
                                var subtitle = null;
                                var hardofHearingEnabled : boolean = dsrvSettingUtil.getHardofHearing();
                                var p = dsrvAvCtrl.getSubtitleComponent();
                                if (p !== undefined) {
                                    subtitle = p.subtitle;
                                }
                                if (subtitle === null || subtitle.list === null || subtitle.list.length === 0) {
                                    return;
                                } else {
                                    var target = null;
                                    var len = subtitle.list.length;
                                    var targetindex = subtitle.index;
                                    var targetlang : string;
                                    var i;

                                    targetlang = dsrvSettingUtil.getLanguage().LangSubTtl;//get lang
                                    for (i=0; i < len ; i++) {
                                        console.log ("  subtitle.list[i].language = " + subtitle.list[i].language);
                                        if(subtitle.list[i].language === targetlang){
                                            if(subtitle.list[i].hearingImpaired === hardofHearingEnabled){
                                                targetindex = i;
                                                break;
                                            }
                                        }
                                    }
                                    if (targetindex === null && hardofHearingEnabled === false){
                                        for (i=0; i < len ; i++) {
                                            if(subtitle.list[i].language === targetlang){
                                                targetindex = i;
                                                break;
                                            }
                                        }
                                    }
                                    if (targetindex !== null) {
                                        target = subtitle.list[targetindex];
                                    }  else {
                                        target = subtitle.list[0];
                                    }
                                    dsrvAvCtrl.setSubtitleComponent({
                                        'off': null,
                                        'on': target
                                    });

                                }
                            }
                        }
                    } else {
                        if (subtitle.nextValue || subtitle.prevValue) {
                            dsrvSettingUtil.setSubtitleEnabled(!!subtitle.nextValue);
                            if (!subtitle.nextValue) {
                                dsrvAvCtrl.offSubtitleComponent();
                            }
                        }
                    }
                } else{
                    if (subtitle.bMedia) {
                        if (subtitle.nextValue === '-1') {
                            dsrvAvCtrl.setMediaSubtitleDisplay({
                                display : false
                            });
                        } else {
                            dsrvAvCtrl.setMediaSubtitleDisplay({
                                display : true
                            });
                            dsrvAvCtrl.setMediaSubtitleLanguage({
                                language : subtitle.nextValue
                            });
                        }
                    } else {
                        if (subtitle.nextValue || subtitle.prevValue) {
                            dsrvAvCtrl.setSubtitleComponent({
                                off : subtitle.prevValue,
                                on : subtitle.nextValue
                            });
                            if (!subtitle.nextValue) {
                                dsrvAvCtrl.offSubtitleComponent();
                            }
                        }
                    }
                }
            }
            if (repeatOption.nextValue !== undefined) {
                hx.log('CPgMain', hx.util.format('doSetTvOptionInfo - repeatOption = {0}', repeatOption.nextValue));
                dsrvWebStorage.setVideoOption({
                    repeatOption : repeatOption.nextValue
                });
            }
        };
        var fnCreateQueryPasswordDlg = () => {
            var passwordDlg = prism.createQueryPasswordDlg(this._mainPage);
            passwordDlg.setTextHtml(tr('LOC_ENTER_PASSWORD_ID'));
            return passwordDlg;
        };
        var onChannelChangeSucceeded = (aChannel) => {
            if (that.mCurrentPageState === that.TPageState.EIdle) {
                if (that.mPlayback) {
                    hx.log('CPgMain', '[onChannelChangeSucceeded] close()');
                    that.mPlayback.close();
                }
                that.mCurrentTrickMode = null;
                that._hideStatusMsg();
            }
        };
        var onChannelChangeError = (aChannel, aErrorState) => {
            if (that.mPlayback) {
                hx.log('CPgMain', '[onChannelChangeSucceeded] close()');
                that.mPlayback.close();
            }
            that.mCurrentTrickMode = null;
            that._hideStatusMsg();
        };

        var destroyMhegApplication = function(){
            //destroy Mheg Applicaiton
            var appMgr = hx.svc.APP_MGR;
            if(appMgr){
                appMgr.destroyMhegApplication();
            }
        }
        var destroyHumaxApplication = function(){
            //destroy Mheg Applicaiton
            var appMgr = hx.svc.APP_MGR;
            if(appMgr){
                appMgr.destroyHbbTvDeeplinkApplication();
            }
        }

        // dsrvBridge
        var onRequestMusicPlayback = (aItems, aStartIndex) => {
            that.mForcePlayToStop = true;
            that.mBlockLive = true;
            hx.log('error', 'onRequestMusicPlayback');
            fnDestroyAllApps(false);
            destroyMhegApplication();
            destroyHumaxApplication();
            while (that._mainPage.stackSize()) {
                that._mainPage.removeLayer();
            }
            that._$wrap.append(that._$main);
            that._$wrap.find('> .-afd').not('#main').removeClass('show');
            if (that.getCenterPageName() === 'CPgSettings') {
                that._changePage('CPgLiveController');
            }
            var musicPlayback = prism.createMusicPlaybackDlg(that._mainPage);
            var item = aItems[aStartIndex];
            var isOperationEnabled = true;
            musicPlayback.setOperationEnabled(isOperationEnabled);
            musicPlayback.setStartIndex(aStartIndex);
            musicPlayback.setData(aItems);
            musicPlayback.setPlaybackService(dsrvPlayback);
            musicPlayback.doSetMusicOption = function(option) {
                dsrvWebStorage.setMusicOption(option);
            };
            musicPlayback.doGetMusicOption = function() {
                return dsrvWebStorage.getMusicOption();
            };
            hx.log('error', 'onRequestMusicPlayback open');
            musicPlayback.open({
                layerName : 'musicPlayback',
                transition : {
                    prevLayer : 'fadeOut',
                    newLayer : 'fadeIn'
                },
                onClosed : () => {
                    if (!that.mForcedBlock) {
                        var excludeIds = '#main, #-test-infoHub';
                        that._$wrap.find('> .-afd').not(excludeIds).addClass('show');
                        if (that.mPlayback) {
                            if (dsrvPlayback.isMpbOnGoing()) {
                                that.mPlayback.stop();
                            } else {
                                that.mBlockLive = false;
                            }
                        } else {
                            that.mBlockLive = false;
                        }
                        if (!that.mBlockLive && !that._HbbTvAppLoaded) {
                            fnRestartLive();
                        }
                    }
                    that.mForcedBlock = false;
                    that.mCurrentPageState = that.TPageState.EIdle;
                    if (this.mPlayback) {
                        this.mPlayback.removeEventHandler(this);
                    }
                    that.mPlayback = null;
                    if (hx.svc.STATISTICS) {
                        dsrvStatistics.statisticsLog({
                            "CODE" : "EMPLAYE", //EVENT_MUSIC_PLAYBACK_END
                            "TRS" : "I"
                        });
                    }
                }
            });
            fnStopAv(() => {
                musicPlayback.play(item);
                that.mPlayback = musicPlayback;
                that.mBlockLive = false;
            });
            that.mCurrentPlaybackItem = item;
            that.mCurrentPageState = that.TPageState.EMusicPlaybackOpened;
        };


        var onRequestVideoPlayback = (aItems, aStartIndex, aKeyCode) => {
            that.mForcePlayToStop = true;
            that.mBlockLive = true;
            var action = 'play';
            var item = aItems[aStartIndex];
            var fnStartPlayback = (isLastPosition) => {
                hx.log('CPgMain', 'onRequestVideoPlayback');
                fnDestroyAllApps(false);
                if ( item instanceof CTsrItem) {
                    // check for External HDD TSR On/Off Model
                    var bActonPvrPlayKey = that._doActonPvrPlayKey();
                    var currentTimeShiftMode = dsrvTsr.getCurrentTimeShiftMode();
                    if (bActonPvrPlayKey === false || currentTimeShiftMode === 0) {
                        that._openNotAvailable();
                        return;
                    }
                    // Check TSR/Recording buffer
                    if (dsrvTsr.isAvailableTrick() === false) {
                        if (dsrvTsr.isRecordingOngoing()) {
                            that._openNotificationDlg(hx.util.format('{0} {1}', hx.l('LOC_NOT_WORK_INSTANCE_RECORDING_ID'), hx.l('LOC_TRY_AGAIN_FEW_SECONDS_ID')));
                        } else {
                            that._openNotificationDlg(hx.util.format('{0} {1}', hx.l('LOC_NOT_WORK_TIME_SHIFT_RECORDING_ID'), hx.l('LOC_TRY_AGAIN_FEW_SECONDS_ID')));
                        }
                        return;
                    }
                }
                while (that._mainPage.stackSize()) {
                    that._mainPage.removeLayer();
                }
                that._$wrap.append(that._$main);
                that._$wrap.find('> .-afd').not('#main').removeClass('show');
                if(that._$wrap_smt[0].children.length > 1){
                    (<HTMLElement>that._$wrap_smt[0].children[1]).style.opacity = "0";
                    that._$wrap[0].style.opacity = "1";
                }
                var skipForwardTime = dsrvSettingUtil.getSkipForwardTime();
                var skipRewindTime = dsrvSettingUtil.getInstantReplayTime();
                var videoPlayback = prism.createVideoPlaybackDlg(that._mainPage);
                var isOperationEnabled = true;
                videoPlayback.setOperationEnabled(isOperationEnabled);
                videoPlayback.setSkipForwardTime(skipForwardTime);
                videoPlayback.setSkipRewindTime(skipRewindTime);
                videoPlayback.doGetSupportedSubtitles = function() {
                    return fnGetSupportedSubtitles() || [];
                };
                videoPlayback.doGetSubtitleLanguage = function() {
                    return fnGetSubtitleLanguage();
                };
                videoPlayback.doGetSubtitleSyncTime = function() {
                    return fnGetSubtitleSyncTime();
                };
                videoPlayback.doGetSubtitleFontSize = function() {
                    return fnGetSubtitleFontSize();
                };
                videoPlayback.doGetSubtitlePosition = function() {
                    return fnGetSubtitlePosition();
                };
                videoPlayback.doGetSubtitleDisplay = function() {
                    return fnGetSubtitleDisplay();
                };
                videoPlayback.doSetSubtitleOption = function(subtitleOption) {
                    var language = subtitleOption.language;
                    var fontSize = subtitleOption.fontSize;
                    var position = subtitleOption.position;
                    var syncTime = subtitleOption.syncTime;
                    var bDisplay = false;
                    if (language !== undefined) {
                        hx.log('CPgMain', hx.util.format('doSetSubtitleOption - language = {0}', language));
                        if (language === '-1') {
                            dsrvAvCtrl.setMediaSubtitleDisplay({
                                display : false
                            });
                        } else {
                            bDisplay = fnGetSubtitleDisplay();
                            if (bDisplay === false) {
                                dsrvAvCtrl.setMediaSubtitleDisplay({
                                    display : true
                                });
                            }
                            dsrvAvCtrl.setMediaSubtitleLanguage({
                                language : language
                            });
                        }
                    }
                    if (fontSize !== undefined) {
                        hx.log('CPgMain', hx.util.format('doSetSubtitleOption - fontSize = {0}', fontSize));
                        dsrvAvCtrl.setMediaSubtitleFontSize({
                            size : fontSize
                        });
                    }
                    if (position !== undefined) {
                        hx.log('CPgMain', hx.util.format('doSetSubtitleOption - position = {0}', position));
                        dsrvAvCtrl.setMediaSubtitleTextPosition({
                            position : position
                        });
                    }
                    if (syncTime !== undefined) {
                        hx.log('CPgMain', hx.util.format('doSetSubtitleOption - syncTime = {0}', syncTime));
                        dsrvAvCtrl.setMediaSubtitleSyncTime({
                            syncTime : syncTime
                        });
                    }
                };
                videoPlayback.doSetVideoOption = function(option) {
                    dsrvWebStorage.setVideoOption(option);
                };
                videoPlayback.doGetVideoOption = function() {
                    return dsrvWebStorage.getVideoOption();
                };
                videoPlayback.doGetSeriesRepeat = function() {
                    return dsrvSettingUtil.getEnableSeriesPlayback();
                };
                videoPlayback.onOpenNotAvailableMsg = function() {
                    that._openNotAvailable();
                };
                videoPlayback.addEventHandler('Show', this, () => {
                    hx.svc.SETTING_UTIL.doingKeyGrab(true);
                });
                videoPlayback.addEventHandler('Hide', this, () => {
                    hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
                });
                videoPlayback.createPlayer(item);

                var saveStatisticLog = (item?) => {
                    var code;

                    if ( item instanceof CTsrItem) {
                        //code = "ETSRPLAYE";
                        //EVENT_TSR_PLAYBACK_END
                    } else if ( item instanceof __MODEL__.IRecordingItem) {
                        code = "ERPLAYE";
                        //EVENT_RECORDING_PLAYBACK_END
                    } else {
                        code = "EVPLAYE";
                        //EVENT_VIDEO_PLAYBACK_END
                    }

                    if (dsrvStatistics) {
                        dsrvStatistics.statisticsLog({
                            "CODE" : code,
                            "TRS" : "I"
                        });
                    }
                };
                videoPlayback.open({
                    layerName : 'videoPlayback',
                    transition : {
                        prevLayer : 'fadeOut',
                        newLayer : 'moveUp'
                    },
                    onClosed : (param) => {
                        if (!that.mForcedBlock) {
                            var excludeIds = '#main, #-test-infoHub';
                            var shownPages = that._$wrap.find('.shown');
                            if (shownPages.length) {
                                shownPages.addClass('show');
                            } else {
                                that._$wrap.find('> .-afd').not(excludeIds).addClass('show');
                            }
                            if(that._$wrap_smt[0].children.length > 1){
                                (<HTMLElement>that._$wrap_smt[0].children[1]).style.opacity = "1";
                                that._$wrap[0].style.opacity = "0";
                            }
                            if (item instanceof CTsrItem) {
                                if (dsrvTsr.isTsrOngoing() || dsrvTsr.isChaseOngoing()) {
                                    if (that.mPlayback) {
                                        that.mPlayback.stop();
                                    }
                                }
                                dsrvTsr.clearTimer();
                            } else {
                                if (that.mPlayback) {
                                    if (dsrvPlayback.isMpbOnGoing()) {
                                        that.mPlayback.stop();
                                    } else {
                                        that.mBlockLive = false;
                                    }
                                } else {
                                    that.mBlockLive = false;
                                }
                            }
                            if (!that.mBlockLive) {
                                if (item instanceof CTsrItem) {
                                    if (dsrvTsr.isTsrOngoing() || dsrvTsr.isChaseOngoing() || dsrvTsr.isRecordingOngoing()) {
                                        if(currentTimeShiftMode){
                                            fnRestartLive();
                                        }
                                    }
                                } else {
                                    if(!that._HbbTvAppLoaded){
                                        fnRestartLive();
                                    }
                                }
                            }
                        }
                        that.mForcedBlock = false;
                        that.mCurrentPageState = that.TPageState.EIdle;
                        that.mPlayback = null;
                        that.mCurrentTrickMode = null;
                        saveStatisticLog(item);
                        dsrvFrontPanel.clearFrontPanelText();
                    }
                });
                if (aKeyCode !== undefined) {
                    if ( item instanceof CTsrItem) {
                        switch (aKeyCode) {
                            case hx.key.KEY_PLAY:
                            case hx.key.KEY_FAST_FWD:
                            case hx.key.KEY_PLAY_SPEED_UP:
                                action = 'play';
                                break;
                            case hx.key.KEY_PAUSE:
                                action = 'pause';
                                break;
                            case hx.key.KEY_REWIND:
                            case hx.key.KEY_PLAY_SPEED_DOWN:
                                action = 'fastRewind';
                                break;
                        }
                    }
                }
                hx.log('CPgMain', hx.util.format('onRequestVideoPlayback - action = {0}', action));
                if ( item instanceof CTsrItem) {
                    dsrvTsr.initStartTime();
                    videoPlayback.setPlaybackService(dsrvTsr);
                } else {
                    videoPlayback.setPlaybackService(dsrvPlayback);
                }
                videoPlayback.setStartIndex(aStartIndex);
                videoPlayback.setData(aItems);
                if ( item instanceof CTsrItem) {
                    videoPlayback.setStartTime(item.getStartTime());
                    videoPlayback.setBuffer(dsrvTsr.getMaxOffset());
                    videoPlayback.setPosition(dsrvTsr.getPlaybackOffset());
                    videoPlayback.setTrickRestrictMode(that.mCurrentTrickMode);
                    videoPlayback[action]();
                    dsrvTsr.setTimer();
                    that.mBlockLive = false;
                    that.mPlayback = videoPlayback;
                } else {
                    fnStopAv(()=> {
                        that.mPlayback = videoPlayback;
                        videoPlayback[action](item, isLastPosition);
                        that.mBlockLive = false;
                    });
                }
                that.mCurrentPlaybackItem = item;
                that.mCurrentPageState = that.TPageState.EVideoPlaybackOpened;
                that.mPlayback = videoPlayback;
            };


            if (item.getLastPlayPosition()) {
                var lastPosition = item.getLastPlayPosition();
                var duration = item.getDuration();
                if (lastPosition < duration) {
                    var btnDlg = prism.createButtonDlg(null, {
                        textHtml : tr('LOC_MSG_PLAYREC_ID'),
                        buttonList : [tr('LOC_YES_ID'), tr('LOC_BTN_PLAYREC_ID')],
                        timeOutValue : 30000,
                        layoutType : 'vertical'
                    }, function(index) {
                        destroyHumaxApplication();

                        if(!(item instanceof CTsrItem)){
                            //Media, Recording
                            destroyMhegApplication();
                        }

                        setTimeout(function(){
                            fnStartPlayback(index === 0);
                            btnDlg.close();
                        },10);
                    });
                    btnDlg.open();
                } else {
                    if(!(item instanceof CTsrItem)) {
                        //Media, Recording
                        destroyMhegApplication();
                    }
                    destroyHumaxApplication();
                    setTimeout(function(){
                        fnStartPlayback(false);
                    },10);
                }
            } else {
                if(!(item instanceof CTsrItem)) {
                    //Media, Recording
                    destroyMhegApplication();
                }
                destroyHumaxApplication();
                setTimeout(function(){
                    fnStartPlayback(false);
                },10);
            }
        };
        var onRequestPhotoPlayback = (items, startIndex) => {
            var photoPlayback = that.mPlayback;
            if (photoPlayback && photoPlayback.isPhotoPbOngoing && photoPlayback.isPhotoPbOngoing()) {
                hx.log('CPgMain', 'onRequestPhotoPlayback - use created PhotoPlaybackDlg()');
                photoPlayback.setStartIndex(startIndex);
                photoPlayback.setData(items);
                that.mCurrentPlaybackItem = items[startIndex];
                that.mBlockLive = false;
                return;
            }
            that.mForcePlayToStop = true;
            that.mBlockLive = true;
            hx.log('CPgMain', 'onRequestPhotoPlayback');
            fnDestroyAllApps(false);
            destroyMhegApplication();
            destroyHumaxApplication();
            while (that._mainPage.stackSize()) {
                that._mainPage.removeLayer();
            }
            photoPlayback = prism.createPhotoPlaybackDlg(that._mainPage);
            that._$wrap.append(that._$main);
            that._$wrap.find('> .-afd').not('#main').removeClass('show');
            that.mCurrentPageState = that.TPageState.EPhotoPlaybackOpened;
            that.mPlayback = photoPlayback;
            var isOperationEnabled = true;
            photoPlayback.setOperationEnabled(isOperationEnabled);
            photoPlayback.setStartIndex(startIndex);
            photoPlayback.setData(items);
            photoPlayback.setPlaybackService(dsrvPlayback);
            photoPlayback.isPhotoPbOngoing = function() {
                return isPhotoPlayback;
            };

            var checkPlayList = () : boolean => {
                var playlist = hx.svc.PLAYLIST.getPlaylist(__MODEL__.IMediaItem.TContentType.EMusic);
                if (playlist && playlist.length > 0) {
                    return true;
                }

                return false;
            }

            //photoPlayback.setBGMList(bgmItemList);
            photoPlayback.doSetPhotoOption = function(option) {
                dsrvWebStorage.setPhotoOption(option);
                if (option.playMusic !== undefined) {
                    if (option.playMusic === 1) {
                        //Check Play List
                        if(checkPlayList()){
                            dsrvPlayback.makeBGM();
                            photoPlayback.setPlaybackService(dsrvPlayback);
                        }
                        else{
                            hx.msg('response', {
                                text : hx.l('LOC_PLAYBAR_PHOTO_CANT_PLAY_MUSIC_ID'),
                                auto_close : true,
                                close_time : 3000
                            });
                        }
                    } else if (option.playMusic === 0) {
                        dsrvPlayback.stopBGM();
                        photoPlayback.setPlaybackService(null);
                    }
                }
            };
            photoPlayback.doRequestPlayBGM = function() {
                if(checkPlayList()){
                    dsrvPlayback.makeBGM();
                }
            };
            photoPlayback.doGetPhotoOption = function() {
                return dsrvWebStorage.getPhotoOption();
            };
            photoPlayback.doGetBGMOption = function() {
                return dsrvWebStorage.getMusicOption();
            };
            photoPlayback.onPlayStatePlaying = function(item) {
                //dsrvFrontPanel.setFrontPanelText('MEDIA PLAY');
                onPlayStatePlaying(item);
            };
            var isPhotoPlayback = true;
            photoPlayback.open({
                layerName : 'photoPlayback',
                dimm : true,
                transition : {
                    prevLayer : 'fadeOut',
                    newLayer : 'fadeIn'
                },
                onClosed : () => {
                    if (!that.mForcedBlock) {
                        var excludeIds = '#main, #-test-infoHub';
                        that._$wrap.find('> .-afd').not(excludeIds).addClass('show');
                    }
                    if (that.mPlayback) {
                        that.mPlayback.stop();
                    } else {
                        that.mBlockLive = false;
                    }
                    isPhotoPlayback = false;
                    if (!that.mForcedBlock) {
                        if (!that.mBlockLive && !that._HbbTvAppLoaded) {
                            fnRestartLive();
                        }
                    }
                    that.mForcedBlock = false;
                    that.mCurrentPageState = that.TPageState.EIdle;
                    that.mPlayback = null;
                    if (hx.svc.STATISTICS) {
                        dsrvStatistics.statisticsLog({
                            "CODE" : "EPPLAYE", //EVENT_PHOTO_PLAYBACK_END
                            "TRS" : "I"
                        });
                    }
                }
            });
            fnStopAv(() => {
                that.mPlayback = photoPlayback;
                photoPlayback.play();
                that.mBlockLive = false;
            });
//            that.mCurrentPageState = item;
        };
        var onRequestInstanceRecording = function(channel) {
            var param: any = {
                tsrPosOffset: 0,
                tsrBufferSize: 0,
                finished: function(result) {
                    hx.log('CPgMain', hx.util.format('_onRequestInstanceRecording - result = {0}', result));
                }
            };
            if (hx.config.useDelayedRec) {
                param.tsrPosOffset = dsrvTsr.getPlaybackOffset();
                param.tsrBufferSize = dsrvTsr.getMaxOffset();
            }
            if (channel && channel.ccid) {
                hx.log('CPgMain', hx.util.format('_onRequestInstanceRecording - channel.ccid = {0}', channel.ccid));
                param.ccid = channel.ccid;
            }
            if (hx.svc.STORAGE_MASS.getRecPVRHDD()) {
                dsrvRecordings.requestInstanceRecording(param);
            } else {
                this._openNotAvailable();
            }
        };
        var onRequestStopPlayback = () => {
            if (that.mCurrentPageState === that.TPageState.EVideoPlaybackOpened || that.mCurrentPageState === that.TPageState.EMusicPlaybackOpened || that.mCurrentPageState === that.TPageState.EPhotoPlaybackOpened) {
                while (that._mainPage.stackSize()) {
                    that._mainPage.removeLayer();
                }
            }
        };

        // dsrvDial
        var onReqLaunch = (name, url)=> {
            hx.log('CPgMain', hx.util.format('ReqLaunch - name = {0}, url = {1}', name, url));

            if (!that._isPlayPossiable()) {
                hx.log('CPgMain', 'not available launch state');
                return;
            }
            that.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgHubProxy?forcedHide'
            });
            //dsrvAppMgr.destroyHumaxApplication(); //if needed (if an app is operating)
            that._changePage('CPgLiveController');

            if (hx.config.netflix && name === 'Netflix') {
                hx.log('CPgMain', 'onReqLaunch Dial Netflix !!!');
                dsrvNetflix.start({
                    type: __NETFLIX__.START_TYPE.DIAL,
                    url: encodeURIComponent(url)
                });
            } else {
                if (hx.config.netflix && dsrvNetflix.isRunning()) {
                    hx.log('CPgMain', 'onReqLaunch Dial Youtube & Stop Netflix !!!');
                    dsrvNetflix.stop(__NETFLIX__.STOP_REASON_TYPE.Reason_DIAL_Other_Launch);
                }
                that._mainPage.createLayer({
                    fnAfterCreate : () => {
                        fnStopAv(() => {
                            dsrvAppMgr.createHbbTvDeeplinkApplication({
                                name : name,
                                url : url,
                                onHumaxApplicationLoaded : function() {
                                    dsrvDial.updateApplicationState(name, that.TDialStatus.ERuning);
                                },
                                onHumaxApplicationUnloaded : function(appInfo) {
                                    dsrvDial.updateApplicationState(name, that.TDialStatus.EStop);
                                }
                            }); // through createHumaxApplication()
                        });
                    }
                });
            }
        };

        var onReqStop = (name) => {
            hx.log('CPgMain', hx.util.format('ReqStop - name = {0}', name));
            if (hx.config.netflix && name === 'Netflix') {
                hx.log('CPgMain', 'onReqStop Dial Netflix !!!');
                dsrvNetflix.stop(__NETFLIX__.STOP_REASON_TYPE.Reason_DIAL_Stop);
            }
        };

        // dsrvDmr
        var onRequestPlay = (item)=> {
            that.mForcePlayToStop = true;
            hx.log('CPgMain', 'onRequestPlay');
            var uri = '';
            var fullPath = item.getFullPath();
            if (fullPath === '' || fullPath === null) {
                that._openNotificationDlg(hx.l('LOC_SENDTOHMS_ERROR_ID'));
            } else {
                var result = fullPath.split("://");
                if (that._isPlayPossiable()) {
                    if (result.length > 0) {
                        var prefix = result[0];
                        uri = fullPath.substring(prefix.length + 3);
                        hx.log('CPgMain', 'result.length = ' + result.length + ', prefix = ' + prefix + ', uri = ' + uri);
                        that.sendEventToEntity({
                            alEvent : 'alMessageToPage',
                            target : 'CPgDetailInfo/FORCE_EXIT',
                            me : that
                        });
                        if (hx.config.netflix && dsrvNetflix.isRunning() === true) {
                            hx.log('CPgMain', 'onRequestPlay Stop Netflix !!!');
                            dsrvNetflix.stop((prefix === "hdvb") ? __NETFLIX__.STOP_REASON_TYPE.Reason_USER_Nav : __NETFLIX__.STOP_REASON_TYPE.Reason_DIAL_Other_Launch);
                        }
                        if (prefix === "hdvb") {
                            while (that._mainPage.stackSize()) {
                                that._mainPage.removeLayer();
                            }
                            that._changePage('CPgLiveController');
                            var resultObj = null;
                            var temp = uri.split(":");
                            if (temp[0] === "triplet") {
                                var triplet = temp[1].split(".");
                                if (triplet.length === 3) {
                                    resultObj = dsrvChlist.getChannelByTripletExceptALL({
                                        onid : triplet[0],
                                        tsid : triplet[1],
                                        sid : triplet[2]
                                    });
                                }
                            } else if (temp[0] === "ccid") {
                                resultObj = dsrvChlist.getChannelByCcidExceptALL({
                                    ccid : uri
                                });
                            }
                            if (resultObj !== null && resultObj.channel !== null) {
                                var changeable = dsrvChlist.checkChannelChangable({
                                    channel : resultObj.channel
                                });
                                if (changeable) {
                                    dsrvAvCtrl.unBlockAV();
                                    dsrvAvCtrl.channelChange({
                                        channel : resultObj.channel,
                                        group : resultObj.group
                                    });
                                } else {
                                    hx.log('CPgMain', 'channel is not changeable');
                                }
                            } else {
                                hx.log('CPgMain', 'channel is null');
                            }
                        } else {
                            if (that.getCenterPageName() !== 'CPgLiveController') {
                                // no dap..
                                var isPhoto = __MODEL__.IMediaItem.TContentType.EPhoto === item.getContentType();
                                var playback = hx.al.entity.getRepository().get("CPgMain").mPlayback;
                                if (isPhoto && !(playback.isPhotoPbOngoing && playback.isPhotoPbOngoing())) {
                                    that._changePage('CPgLiveController');
                                }
                            }
                            dsrvBridge.requestPlay([item], 0);
                        }
                        dsrvAppMgr.destroyAllApps();
                    }
                }
            }
        };
        var onRequestStop = () => {
            hx.log('CPgMain', 'onRequestStop');
            that.mForcePlayToStop = false;
            if (that.mPlayback) {
                clearTimeout(that.mForcePlayToStopTimer);
                that.mForcePlayToStopTimer = setTimeout(function() {
                    hx.log("info", "========================[onRequestStop] mForcePlayToStop ::: " + that.mForcePlayToStop);
                    if (!that.mForcePlayToStop) {
                        that.mPlayback.close();
                    }
                }, 3000);
            }
        };
        var onRequestPause = () => {
            hx.log('CPgMain', 'onRequestPause');
            if (that.mPlayback) {
                that.mPlayback.pause();
            }
        };
        var onRequestResume = () => {
            hx.log('CPgMain', 'onRequestResume');
            if (that.mPlayback) {
                that.mPlayback.resume();
            }
        };
        var onRequestSetPosition = (position) => {
            hx.log('CPgMain', 'onRequestResume');
            dsrvPlayback.seek({
                offset : position
            });
        };
        var onRequestSetVolume = () => {
            hx.log('CPgMain', 'onRequestSetVolume');
        };
        var onRequestMute = function() {
            hx.log('CPgMain', 'onRequestMute');
        };
        var onRequestBringIn = function() {
            hx.log('CPgMain', 'onRequestBringIn');
            var channel = dsrvAvCtrl.getVideoBroadcastCurrentChannel();
            var ccid;
            if (channel) {
                ccid = channel.ccid;
                dsrvDmr.setMedia(ccid);
            }
        };
        var onMusicInfoChanged = (title, album, artist) => {
            if (that.mPlayback && that.mPlayback.updateMusicInfo) {
                that.mPlayback.updateMusicInfo(title, album, artist);
            }
        };
        var onAlbumImgChanged = (path) => {
            if (that.mPlayback && that.mPlayback.updateAlbumImg) {
                that.mPlayback.updateAlbumImg(path);
            }
        };

        // dsrvPlayback
        var onPlaySpeedChanged = (speed) => {
            hx.log('CPgMain', hx.util.format('onPlaySpeedChanged - speed = {0}', speed));
            if (that.mPlayback && that.mPlayback.setSpeed) {
                that.mPlayback.setSpeed(speed);
            }
        };
        var onPlaybackOptionChange = (item) => {
            if (item.state === 'ForwardTime') {
                if (that.mPlayback && that.mPlayback.setSkipForwardTime) {
                    that.mPlayback.setSkipForwardTime(item.time);
                }
            }

            if (item.state === 'ReplayTime') {
                if (that.mPlayback && that.mPlayback.setSkipRewindTime) {
                    that.mPlayback.setSkipRewindTime(item.time);
                }
            }
        };
        var onPlaySpeedsArrayChanged = (supportedSpeedsArray) => {
            hx.log('CPgMain', hx.util.format('onPlaySpeedsArrayChanged - supportedSpeedsArray = {0}', supportedSpeedsArray));
            if (that.mPlayback) {
                that.mPlayback.setPlaySpeedsArray(supportedSpeedsArray);
            }
        };
        var onPlayPositionChanged = (position, buffer) => {
            hx.log('CPgMain', hx.util.format('onPlayPositionChanged - position = {0}, buffer = {1}', position, buffer));
            var duration = 0;
            that.mCurrentPositon = position;
            if (that.mPlayback && that.mPlayback.setPosition) {
                if (!that.mNetworkAvailable && that.mCurrentPlaybackItem instanceof __MODEL__.CNetworkMediaItem) {
                    // TODO find mediaPage
                    prism.createButtonDlg(null, {
                        textHtml : hx.l('LOC_STR_MESG_904_ID'),
                        buttonList : [hx.l('LOC_OK_ID')]
                    }, function(index) {
                        that.mCurrentPlaybackItem.close();
                    }).open();
                    that.mPlayback.close();
                    return;
                }
                if (that.mCurrentPlaybackItem) {
                    that.mPlayback.setPosition(position);
                    if (buffer > 0 && that.mPlayback.setBuffer) {
                        that.mPlayback.setBuffer(Math.floor(buffer / 1000));
                    }
                }
                if (position === 0) {
                    that.mPlayback.resume();
                }
            }
        };
        var onEndOfBufferReached = () => {
            hx.log('CPgMain', 'onEndOfBufferReached');
            if (that.mPlayback && that.mPlayback.changeToLive) {
                if (that.mCurrentPlaybackItem) {
                }
            }
        };
        var onPlayStateStopped = () => {
            hx.log('CPgMain', 'onPlayStateStopped');
            if (that.mCurrentPlaybackItem) {
                if (that.mCurrentPageState === that.TPageState.EPhotoPlaybackOpened) {
                    if (that.mPlayNext) {
                        that.mPlayNext();
                    }
                    return;
                }
                if (that.mCurrentPlaybackItem.getContentType() === __MODEL__.IMediaItem.TContentType.EVideo) {
                    that.mCurrentPlaybackItem.getThumbnailUrl();
                    if (that.mCurrentPositon > 0) {
                        var historyInfoList = dsrvHistory.getHistoryList();
                        var historyInfo = null;
                        for (var i = 0, length = historyInfoList.length; i < length; i += 1) {
                            historyInfo = historyInfoList[i];
                            if (historyInfo instanceof __MODEL__.cMediaPlaybackHistoryItem) {
                                if (that.mCurrentPlaybackItem.getFullPath() === historyInfo.getFullPath()) {
                                    dsrvHistory.removeHistoryItem({
                                        type : __MODEL__.THistoryType.Media,
                                        id : historyInfo.getFullPath()
                                    });
                                    historyInfo.setLastPlayPosition(that.mCurrentPositon);
                                    dsrvHistory.addMediaPlayed({
                                        mediaItem : historyInfo
                                    });
                                    break;
                                }
                            } else if (historyInfo instanceof __MODEL__.cPlaybackHistoryItem) {
                                if (that.mCurrentPlaybackItem.getId() === historyInfo.getId()) {
                                    dsrvHistory.removeHistoryItem({
                                        type : __MODEL__.THistoryType.Playback,
                                        id : historyInfo.getId()
                                    });
                                    historyInfo.setLastPlayPosition(that.mCurrentPositon);
                                    dsrvHistory.addMediaPlayed({
                                        mediaItem : historyInfo
                                    });
                                }
                            }
                        }
                    }
                }
            }
            if (that.mPlayNext) {
                that.mPlayNext();
                that.mPlayNext = null;
            }
            that._hideStatusMsg();
            dsrvDmr.setMedia(null);
        };
        var onPlayStateFinished = () => {
            hx.log('CPgMain', 'onPlayStateFinished');
            if (that.mPlayback) {
                var nextPlayItem = that.mPlayback.getNextPlayItem(that.mCurrentPlaybackItem);
                that.mPlayNext = () => {
                    if (that.mPlayback) {
                        if (nextPlayItem) {
                            if (nextPlayItem.isLocked && nextPlayItem.isLocked()) {
                                var passwordDlg = fnCreateQueryPasswordDlg();
                                passwordDlg.open({
                                    dimm : true,
                                    onPasswordEntered : function(password) {
                                        var result = hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                                            password : password
                                        });
                                        if (result && (result.result === 0)) {
                                            passwordDlg.close();
                                            that.mPlayback.play(nextPlayItem);
                                            if (that.mCurrentPageState === that.TPageState.EVideoPlaybackOpened) {
                                                that.mPlayback.doShow();
                                            }
                                        } else {
                                            passwordDlg.reset();
                                        }
                                    }
                                });
                            } else {
                                that.mPlayback.play(nextPlayItem);
                                if (that.mCurrentPageState === that.TPageState.EVideoPlaybackOpened) {
                                    that.mPlayback.doShow();
                                }
                            }
                        } else if (that.mPlayback._bgmItemList === undefined) {
                            that.mPlayback.close();
                        }
                        that.mPlayNext = null;
                    }
                };
                if (that.mCurrentPageState === that.TPageState.EPhotoPlaybackOpened) {
                    //dsrvPlayback.stop();
                    that.mPlayback.stopBGM();
                } else {
                    that.mPlayback.stop();
                    if (that.mCurrentPageState === that.TPageState.EVideoPlaybackOpened) {
                        that.mPlayback.doHide();
                    }
                }
                dsrvDmr.setMedia(null);
            }
        };
        var onPlayStateError = (error) => {
            hx.log('CPgMain', 'onPlayStateError');
            var now = new Date().getTime();
            var jump = false;
            if (now - that.mOldErrorTime < 3000) {
                jump = true;
            }
            that.mOldErrorTime = now;
            if (jump) {
                return;
            }
            var message, data, uri, nextPlayItem;
            if (that.mPlayback) {
                nextPlayItem = that.mPlayback.getNextPlayItem(that.mCurrentPlaybackItem);
                that.mPlayNext = function() {
                    if (that.mPlayback) {
                        if (nextPlayItem) {
                            setTimeout(function() {
                                if (that.mPlayback) {
                                    that.mPlayback.play(nextPlayItem);
                                }
                            }, 3000);
                        } else if (that.mPlayback._bgmItemList === undefined) {
                            that.mPlayback.close();
                        }
                        that.mPlayNext = null;
                    }
                };
                if (that.mCurrentPageState === that.TPageState.EPhotoPlaybackOpened) {
                    that.mPlayback.stopBGM();
                } else {
                    that.mPlayback.stop();
                }
            }
            data = dsrvPlayback.getData();
            uri = dsrvDmr.getCurrentUri();
            if (data === uri) {
                message = hx.l('LOC_SENDTOHMS_ERROR_ID');
            } else {
                switch (that.mCurrentPageState) {
                    case that.TPageState.EVideoPlaybackOpened:
                        message = hx.l('LOC_MSG_PLAYBACK_ERROR_01_ID');
                        break;
                    case that.TPageState.EMusicPlaybackOpened:
                        message = hx.l('LOC_MSG_MUSIC_ERROR_01_ID');
                        break;
                    //case that.TPageState.EPhotoPlaybackOpened:
                    //    message = hx.l('LOC_MSG_MUSIC_ERROR_01_ID');
                    //    break;
                }
            }
            if (message) {
                that._openNotificationDlg(message);
            }
        };
        var onPlayStatePlaying = (item) => {
            hx.log('CPgMain', 'onPlayStatePlaying');
            if (!!item) {
                var uri = item.getFullPath();
                if (uri.indexOf('://') === -1) {
                    uri = 'file://' + uri;
                }
                dsrvDmr.setMedia(uri);
                if (item.getFullPath().indexOf('HPVR://') === 0) {
                    dsrvHistory.addRecordingPlayed({
                        playbackItem : item._property
                    });
                } else {
                    dsrvHistory.addMediaPlayed({
                        mediaItem : item
                    });
                }
                if (item.getContentType() === __MODEL__.IMediaItem.TContentType.EVideo) {
                    var subtitles = fnGetSupportedSubtitles();
                    if (!subtitles) {
                        var sub = $('.sub');
                        if (sub) {
                            sub.addClass('disabled');
                            sub.removeClass('-f');
                        }
                    }
                }
                that.mCurrentPlaybackItem = item;
            }
        };
        var onDRMRightsError = (errorState, trickLimit)=> {
            hx.log('CPgMain', 'onDRMRightsError');
            var errorText = '';
            switch(errorState) {
                case 0:
                    errorText = hx.l('LOC_CHANNEL_NOT_AVAILABLE_ID');
                    that._showStatusMsg(errorText);
                    break;
                case 1:
                    errorText = hx.l('LOC_DRM_BLOCK_ID');
                    that._showStatusMsg(errorText);
                    break;
                default:
                    that._hideStatusMsg();
                    break;
            }
            // this._openNotificationDlg(hx.l('LOC_TRICK_LIMIT_ID'));
        };
        var onBGMListCreated = (bgmItemList)=> {
            hx.log('CPgMain', 'onBGMListCreated');
            if (that.mPlayback && that.mPlayback.setBGMList) {
                that.mPlayback.setBGMList(bgmItemList);
            }
        };
        var onNoValidMusicFile = () => {
            this._openNotificationDlg(hx.l('LOC_THERE_IS_NO_VALID_MUSIC_FILE_ID'));
        };

        var onAudioOutputRestrict = () => {            
            hx.log('CPgMain', hx.util.format('onAudioOutputRestrict'));            
            if (that.mPlayback) {
                that.mPlayback.audioRestrict();
            }            
        };

        var onCheckPrivacyVersion = () => {
            hx.log('debug', '[MAIN][onCheckPrivacyVersion]');
            dsrvTVPortal.checkPrivacyVersion(() => {
                dsrvTVPortal.login({
                    fn : () => {},
                    fnError : () => {}
                });
            }, () => {
                setTimeout(() => {
                    if (that._ppDlg) {
                        hx.log('debug', '[MAIN][onCheckPrivacyVersion] do not pop it up more. the user should confirm it.');
                        return;
                    }
                    that._ppDlg = prism.createButtonDlg(null, {
                        textHtml: hx.l('LOC_HOME_ACCEPT_PP_TC_ID'),
                        buttonList : ["OK"]
                    }, function(index) {
                        if (index == 0) {
                            that.sendEventToEntity({
                                alEvent : 'alChangePage',
                                target : 'CPgSettings?set=privacypolicy',
                                me : that
                            });
                        }
                        that._ppDlg = null;
                    });
                    that._ppDlg.open({
                        passiveClose : true
                    });
                }, 1000);
            });
        };

        var onInternetAvailableChanged = (aInternetAvailable) => {
            hx.log('CPgMain', 'onInternetAvailableChanged : ' + aInternetAvailable);
            dsrvIpEpg.getAuthToken(0);
            onCheckPrivacyVersion();
        };

        var onThermalProtectionOccured = () => {
            hx.log('CPgMain', 'onThermalProtectionOccured : go standby in 5 seconds');
            hx.msg('status', {
                text: hx.l('LOC_THERMAL_DESC_ID'),
                auto_close: true,
                timeout: 5000,
                callback_fn: (p) => {
                    hx.log('CPgMain', 'onThermalProtectionOccured : go!! standby');
                    this.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgStandby',
                        me: this
                    });
                },
                validOnNetflix : true
            })
        };
        // dsrvStorageUtil
        var onPhysicalStorageAttached =  (physicalStorage)=>  {
            hx.log('CPgMain', 'onPhysicalStorageAttached');
            var id = physicalStorage.id;
            var kind = physicalStorage.kind;
            var interfaceType = physicalStorage.interfaceType;
            var usage = physicalStorage.usage;
            var TStorageType = __MODEL__.IMediaItem.TStorageType;
            var ulusage = physicalStorage.usage;

            if (that.getCenterPageName() === 'CPgStandby') {
                return;
            }
            if (that._isPlayPossiable()) {
                if (interfaceType ===TStorageType.ESata || interfaceType === TStorageType.EUsb || interfaceType === TStorageType.ESdCard) {
                    if ((interfaceType !== TStorageType.ESata) && (ulusage !== that.THMXPhysicalStorageUsage.EUsagePVR)) {
                        if (physicalStorage.ismount) {
                            var info = {
                                title : hx.l('LOC_USB_DETACHED_ID'),
                                video : hx.l('LOC_STR_VIDEO_01_ID'),
                                photo : hx.l('LOC_STR_PHOTO_01_ID'),
                                music : hx.l('LOC_STR_MUSIC_01_ID')
                            };
                            var selectionDlg = prism.createUsbWizardSelectionDlg(null, info, function(pageName) {
                                that.sendEventToEntity({
                                    alEvent : 'alMessageToPage',
                                    target : 'CPgHubProxy?forcedHide'
                                });
                                selectionDlg.close();
                                fnDestroyAllApps(true);
                                dsrvBridge.setTicket(physicalStorage.id);
                                that._changePage(pageName);
                            });
                            selectionDlg.open({
                                dimm : true,
                                onClosed: () => {
                                    this.setKeyHookingControl(selectionDlg);
                                }
                                /*transition : {
                                 newLayer : 'scaleY'
                                 }*/
                            });
                            hx.svc.SETTING_UTIL.doingKeyGrab(true);
                            that._usbWizardSelectionDlg = selectionDlg;
                            this.setKeyHookingControl(selectionDlg);
                        }
                        else {
                            that._openNotificationDlg(hx.l('LOC_STR_MESG_1007_ID'));
                        }
                    }
                }
                else if (interfaceType !== TStorageType.ESata)
                {

                }
            }
            var prefixUrl = hx.svc.SETTING_UTIL.getPrefixUrl();
            // TODO: remove this
            hx.log('debug', '==== prefix url renewal:');
            hx.log('debug', prefixUrl);
            (<any>hx.svc.AV_CTRL)._eventSource._fireEvent("NetworkUriChanged", {'uri' : prefixUrl, 'mode':'USB'});

            var dsrvTVPortal = hx.svc.TV_PORTAL;
            var tvPortalUrl = hx.svc.SETTING_UTIL.getTVPortalUrl();

            if (dsrvTVPortal.getPortalUrl() !== tvPortalUrl)
            {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE" : "SSTAND", //STATUS_STANDBY
                        "TRS" : "I"
                    });
                    hx.svc.STATISTICS.transmitLog();
                }
                hx.svc.STATISTICS.setStatisticsMode(false);
                hx.svc.TV_PORTAL.login({
                    fn: ()=>{
                        hx.svc.STATISTICS.startLog();
                        that.sendEventToEntity({
                            alEvent : 'alMessageToPage',
                            target : 'CPgHubProxy?updateTVPortalUrl'
                        });
                    }, fnError: ()=>{}
                });
            }
        };
        var onPhysicalStorageDetached = (physicalStorage) => {
            hx.log('CPgMain', 'onPhysicalStorageDetached');
            if (that._usbWizardSelectionDlg) {
                that._usbWizardSelectionDlg.close();
                delete that._usbWizardSelectionDlg;
            }

            if (hx.config.useInternalPvr === 0) {
                var stPartition = null, stStroage = null, i = 0;

                for ( i = 0; i < physicalStorage.partitions.length; i += 1) {
                    stPartition = physicalStorage.partitions[i];
                    var key;
                    for (key in stPartition) {
                        hx.log('CPgMain', 'key: ' + key + ', value : ' + stPartition[key]);
                    }
                    if ((stPartition.kind === 1) && (stPartition.usage === 1) && (stPartition.pvr === 0)) {
                        continue;
                    }
                    if (stPartition.usage === that.THMXPhysicalStorageUsage.EUsagePVR) {
                        // TODO : - JHLEE : Check Other pairing HDD
                        if (that.mPlayback) {
                            if (that.mCurrentPlaybackItem && that.mCurrentPlaybackItem instanceof CTsrItem) {
                                hx.log('CPgMain', '[onPhysicalStorageDetached] - Close Playbar');
                                that.mPlayback.close();
                            }
                        }
                    }
                }
            }

            var isFileOp = hx.svc.MEDIA_ITEM_MANAGER.isFileOp();
            if (isFileOp) {
                prism.createButtonDlg(null, {
                        textHtml : hx.util.format('{0} {1}', hx.l('LOC_USB_UNPLUGGED_ID'), hx.l('LOG_DAMAGE_OCCUR_FILE_ID')),
                        buttonList : [hx.l('LOC_OK_ID')]
                    }, function(index) {}
                ).open();
            }
        };

        var onPhysicalStorageStatus = function(id, status) {
            hx.log('CPgMain', 'onPhysicalStorageStatus - status : ' + status);
            if (status === 15 || status === 16) {// eOPL_STORAGE_EVENT_OverCurrent, eOPL_STORAGE_EVENT_UnMountFail
                if (that.mPlayback) {
                    that.mPlayback.close();
                }
            }
        };

        // dsrvTsr
        //var onRecordingEvent = function(error, state, recordingId) {
        //    hx.log('CPgMain', 'onRecordingEvent');
        //};
        var onLiveStarted = () => {
            hx.log('CPgMain', 'onLiveStarted');
            if (!that.mBlockLive) {
                if (that.mPlayback && that.mPlayback.changeToLive) {
                    var item = that.mCurrentPlaybackItem;
                    that.mPlayback.changeToLive();
                    that.mPlayback.setPlaybackService(dsrvTsr);
                    that.mPlayback.play(item);
                    dsrvTsr.setTimer();
                    fnRestartLive();
                }
                var now = new Date().getTime();
                if (!that.mLastNotiLiveBroadcastTime || (now - that.mLastNotiLiveBroadcastTime) > 3000) {
                    that._openNotificationDlg(hx.l('LOC_GENRE_LIVE_BROADCAST_ID'));
                }
                that.mLastNotiLiveBroadcastTime = now;
            }
            dsrvFrontPanel.clearFrontPanelLed();
            dsrvFrontPanel.clearFrontPanelText();
        };
        var onTsrPBStarted = ()=> {
            hx.log('CPgMain', 'onTsrPBStarted');
            if (that.mPlayback && that.mPlayback.tsrPBStarted) {
                that.mPlayback.tsrPBStarted();
            }

            destroyMhegApplication();
        };
        var onLiveStopping = function() {
            hx.log('CPgMain', 'onLiveStopping');
        };
        var onProgrammesChanged = (programmes) =>{
            hx.log('CPgMain', 'onProgrammesChanged');
            if (ENV.op === ENV.listOP.SES) {
                if (that.mPlayback && that.mPlayback.addDivisionPoint) {
                    that.mPlayback.addDivisionPoint(programmes);
                }
            }
        };
        var onPlayBufferChanged = (buffer) => {
            hx.log('CPgMain', hx.util.format('onPlayBufferChanged - buffer = {0}', buffer));
            if (that.mPlayback && that.mPlayback.setBuffer) {
                that.mPlayback.setBuffer(buffer);
            }
        };
        var onRecordingItemAdded = (item) => {
            if (item) {
                hx.log('CPgMain', '[onRecordingItemAdded] - item exist ...');
                var channel = dsrvChlist.getCurrentChannel().channel;
                if (item.getChannelId() === channel.ccid) {
                    if (that.mPlayback) {
                        if (that.mCurrentPlaybackItem && that.mCurrentPlaybackItem instanceof CTsrItem) {
                            hx.log('CPgMain', '[onRecordingItemAdded] - Close Playbar');
                            that.mPlayback.close();
                        }
                    }
                }
                dsrvHistory.addRecordingStarted({
                    recordingItem : item._property
                });
            }
        };
        var onRecordingCompleted = (item)=> {
            if (!!item) {
                hx.log('CPgMain', 'onRecordingCompleted - item exist ...');
                var channel = dsrvChlist.getCurrentChannel().channel;
                if (item.getChannelId() === channel.ccid) {
                    if (that.mPlayback) {
                        if (that.mCurrentPlaybackItem && that.mCurrentPlaybackItem instanceof CTsrItem) {
                            hx.log('CPgMain', 'onRecordingCompleted - Close Playbar');
                            that.mPlayback.close();
                        }
                    }
                }
            }
        };
        var onNotiSatIpRequested = (item)=> {
            var bEnable = dsrvSettingUtil.getEnableSatIpNotification();
            if (!bEnable) {
                hx.log('CPgMain', 'onNotiSatIpRequested - disable SAT>IP Notification : ' + bEnable);
                return;
            }
            var currentCenterPage = that.getCenterPageName();
            var bPlayPossible = that._isPlayPossiable();
            hx.log('CPgMain', 'onNotiSatIpRequested - currentCenterPage : ' + currentCenterPage);
            if ((currentCenterPage !== 'CPgLiveController' &&
                currentCenterPage !== 'CPgGuide' &&
                currentCenterPage !== 'CPgRecordings' &&
                currentCenterPage !== 'CPgVideo' &&
                currentCenterPage !== 'CPgPhoto' &&
                currentCenterPage !== 'CPgMusic' &&
                currentCenterPage !== 'CPgDetailInfo' &&
                currentCenterPage !== 'CPgMediaCenter') || !bPlayPossible){
                hx.log('CPgMain', 'onNotiSatIpRequested - cannot display it on ' + currentCenterPage + ' / bPlayPossible : ' + bPlayPossible);
                return;
            }
            if (that._notiSatIpMsg) {
                hx.log('CPgMain', 'onNotiSatIpRequested - resetTimeout for SAT>IP notiMsg');
                that._notiSatIpMsg.resetTimeout();
                return;
            }
            var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID');
            function callback_fn(status, param) {
                switch(status) {
                    case 'key_back':
                    case 'key_power':
                    case 'key_esc':
                    case 'auto_close':
                    case no:
                        break;
                    case yes:
                        that.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings?set=satip',
                            me : that
                        }); // go to SAT>IP Mode
                        break;
                    case 'checkBox':
                        dsrvSettingUtil.setEnableSatIpNotification(!param.bChecked) // do not display again
                        return;
                }
                that._notiSatIpMsg = null;
                $('#DIthis.osd').removeClass('osd');
            }
            var html = hx.l('LOC_SAT_IP_STABLE_FEATURES_SYSTEM_ID') + '<br>' + hx.l('LOC_DO_YOU_WANT_TO_ENTER_SAT_IP_ID');
            var checkText = hx.l('LOC_DO_NOT_DISPLAY_AGAIN_ID');
            that._notiSatIpMsg = hx.msg('checkDialog', {
                title : hx.l('LOC_SATIP_NOTI_TITLE_ID'),
                html : html,
                checkText : checkText,
                auto_close : true,
                timeout : 30000,
                btn_title_arr : [yes, no],
                dia_class : 'osd',
                callback_fn : callback_fn
            });
        };
        var onTrickRestrictModeChanged = (trickMode) => {
            hx.log('CPgMain', hx.util.format('onTrickRestrictModeChanged - trickMode = {0}', trickMode));
            if (ENV.op === ENV.listOP.SES) {
                that.mCurrentTrickMode = trickMode;
                if (that.mPlayback) {
                    that.mPlayback.setTrickRestrictMode(trickMode);
                }
            }
        };

        var onTrickError = () => {
            this.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgNotAvailable?show'
            });
        };
        var onApplicationLoadError = function(appInfo) {
            if (!appInfo || appInfo.name === 'CPgOnDemand') {
                dsrvAvCtrl.unBlockAV();
                if (!dsrvAvCtrl.lastChannel()) {
                    dsrvAvCtrl.availableChannel();
                }
            }
        };

        var onChangeCPgLiveController = (type) => {
            // check no service
            var result = hx.svc.CH_LIST.getChannelCount({});
            if(type !== undefined && type === __NETFLIX__.CTRL_TYPE.START) {
                if (result.chCnt > 0) {// not no sdb
                    this.sendEventToEntity({
                        'alEvent': 'alChangePage',
                        'target': 'CPgLiveController?AppStarted'
                    });
                    this.sendEventToEntity({
                        alEvent : 'alMessageToPage',
                        target : 'CPgInfoHub?backToLive',
                        me : this
                    });
                } else {
                    this.sendEventToEntity({
                        'alEvent' : 'alChangePage',
                        'target' : 'CPgLiveController?hideerrorstate',
                        'me' : this
                    });
                    this.sendEventToEntity({
                        'alEvent' : 'alMessageToPage',
                        'target' : 'CPgLiveController?hideNoChannelConfirm',
                        'me' : this
                    });
                }
            } else {
                if (result.chCnt === 0) {// no sdb
                    this.sendEventToEntity({
                        'alEvent' : 'alMessageToPage',
                        'target' : 'CPgLiveController?nochannel',
                        'me' : this
                    });
                }
            }
        };

        var onShowPWForNetfixLaunch = (type) => {
            hx.log("CPgMain","[onShowPWForNetfixLaunch] type :" + type);
            var text = hx.l('LOC_ENTER_PASSWORD_ID');
            that._pinObjForNetflix = hx.msg('pinPC', {
                text : text,
                auto_close : false,
                enable : true,
                callback_fn : function(_status, _input) {
                    if (_status === 'stop') {
                        hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                            password : _input,
                            cb : function(retObj) {
                                if (retObj.result === 0) {
                                    _pinMsgClose();
                                    dsrvNetflix.start({
                                        type: type,
                                        passedPW: true
                                    });
                                } else {
                                    _pinMsgClose();
                                    hx.msg('pinPC', {
                                        text : text,
                                        auto_close : true,
                                        enable : false,
                                        timeout : 500,
                                        callback_fn : function(_status, _input) {
                                            if (_status === 'auto_close') {
                                                onShowPWForNetfixLaunch(type);
                                            }
                                        }
                                    });
                                }
                            }
                        });
                    }
                }
            });

            var _pinMsgClose = function() {
                if (that._pinObjForNetflix !== null) {
                    hx.msg.close('pinPC', that._pinObjForNetflix);
                    that._pinObjForNetflix = null;
                }
            }
        };

        var onHideNetflixPW = (param?) => {
            hx.log("CPgMain","[onHideNetflixPW] ");
            if(!hx.config.netflix) {
                return;
            }
            if (that._pinObjForNetflix !== null) {
                hx.msg.close('pinPC', that._pinObjForNetflix);
                that._pinObjForNetflix = null;
            }
            dsrvNetflix.setPwStatus(false);
        };

        var onSkipChannels = (channelCount) => {
            var sendResponse = () => {
                var channel = hx.svc.CH_LIST.getCurrentChannel().channel;
                var channelData = {
                    number : channel.majorChannel,
                    callSign : channel.name,
                    affiliateCallSign : channel.name
                };
                dsrvVA.publish(channelData);
            };

            if (that.getCenterPageName() === 'CPgLiveController') {
                if (channelCount > 0) {
                    dsrvAvCtrl.naviChannel({
                        'direction': 'next'
                    });
                    sendResponse();
                } else if (channelCount < 0) {
                    dsrvAvCtrl.naviChannel({
                        'direction': 'prev'
                    });
                    sendResponse();
                } else {
                    hx.log('CPgMain', '[onSkipChannels] Something wrong.');
                }
            }

        };

        var onChangeChannelByNumber = (number) => {
            var channel = hx.svc.CH_LIST.getChannel({
                lcn: number
            });

            if (that.getCenterPageName() === 'CPgLiveController') {
                if(channel) {
                    var isChangeable = hx.svc.CH_LIST.checkChannelChangable({
                        channel: channel.channel
                    });
                    if(isChangeable === true) {
                        hx.svc.AV_CTRL.channelChange({
                            channel: channel.channel
                        });

                        var channelData = {
                            number : channel.channel.majorChannel,
                            callSign : channel.channel.name,
                            affiliateCallSign : channel.channel.name
                        };

                        dsrvVA.publish(channelData);
                    } else {
                        hx.log('CPgMain', '[onChangeChannel] Warning, Can not change channel.');
                    }
                } else {
                    hx.log('CPgMain', '[onChangeChannel] Error, Could not found channel Info.');
                }
            }

        };

        var onChangeChannelByName = (payloadChannel) => {
            var channels = hx.svc.CH_LIST.getChlist({});
            var channel = null;
            var len = 0;
            var eitChannelName = '';
            var isChangeable = false;
            var callsign = payloadChannel.callsign;
            var name = payloadChannel.name;

            if (that.getCenterPageName() === 'CPgLiveController') {
                if (channels) {
                    callsign = callsign.replace(/ /gi, "").toLowerCase();
                    name = name.replace(/ /gi, "").toLowerCase();
                    channels = channels.chList;
                    len = channels ? channels.length : 0;
                    for (var i = 0; i < len; i++) {
                        channel = channels[i];
                        eitChannelName = channel.name;
                        eitChannelName = eitChannelName.replace(/ /gi, "").toLowerCase();
                        if (callsign === eitChannelName || name === eitChannelName) {
                            isChangeable = hx.svc.CH_LIST.checkChannelChangable({
                                channel: channel
                            });
                            if (isChangeable) {
                                hx.svc.AV_CTRL.channelChange({
                                    channel: channel
                                });

                                var channelData = {
                                    number : channel.majorChannel,
                                    callSign : payloadChannel.callsign,
                                    affiliateCallSign : payloadChannel.callsign
                                };

                                dsrvVA.publish(channelData);
                            }
                            break;
                        }
                    }
                }
            }
        };

        var onTurnOn = () => {
            if (that.getCenterPageName() === 'CPgStandby') {
                dsrvVA.processData();
                dsrvVA.publish();
            }
        };

        var onTurnOff = () => {
            if (that.getCenterPageName() !== 'CPgStandby') {
                dsrvVA.processData();
                dsrvVA.publish();
            }
        };

        var onSetMute = () => {
            var isMute = false;

            isMute = dsrvSettingUtil.isMute();
            if (isMute === false) {
                dsrvVA.processData();
                dsrvVA.publish();
            }
        };

        var onNextMediaPlay = () => {
            var centerPage = this.getCenterPageName();

            if (centerPage === 'CPgVideo' || centerPage === 'CPgPhoto' || centerPage === 'CPgMusic' ||
                centerPage === 'CPgRecordings') {
                if (this.mCurrentPageState === this.TPageState.EPhotoPlaybackOpened) {
                    var isSmartSearchEnabled = function() {
                        return !!($('#wrap_smt').find('#smt2.in').parent().css('opacity') === "1");
                    };
                    var isProgrammeDetailEnabled = function() {
                        return !!($('#wrap #pde2').parent().parent().parent().hasClass('show')) || !!($('#wrap_smt #pde2').parent().parent().css('opacity') === "1");
                    };
                    if (!isSmartSearchEnabled() && !isProgrammeDetailEnabled()) {
                        this._mainPage.doKey(hx.key.KEY_RIGHT);
                    }
                    dsrvVA.publish();
                } else {
                    if (that.mPlayback) {
                        var nextPlayItem = that.mPlayback.getNextPlayItem(that.mCurrentPlaybackItem);
                        that.mPlayNext = () => {
                            if (that.mPlayback) {
                                if (nextPlayItem) {
                                    if (nextPlayItem.isLocked && nextPlayItem.isLocked()) {
                                        var passwordDlg = fnCreateQueryPasswordDlg();
                                        passwordDlg.open({
                                            dimm : true,
                                            onPasswordEntered : function(password) {
                                                var result = hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                                                    password : password
                                                });
                                                if (result && (result.result === 0)) {
                                                    passwordDlg.close();
                                                    that.mPlayback.play(nextPlayItem);
                                                    if (that.mCurrentPageState === that.TPageState.EVideoPlaybackOpened) {
                                                        that.mPlayback.doShow();
                                                    }
                                                } else {
                                                    passwordDlg.reset();
                                                }
                                            }
                                        });
                                    } else {
                                        that.mPlayback.play(nextPlayItem);
                                        if (that.mCurrentPageState === that.TPageState.EVideoPlaybackOpened) {
                                            that.mPlayback.doShow();
                                        }
                                    }
                                } else if (that.mPlayback._bgmItemList === undefined) {
                                    that.mPlayback.close();
                                }
                                that.mPlayNext = null;
                            }
                        };
                        if (that.mCurrentPageState === that.TPageState.EPhotoPlaybackOpened) {
                            //dsrvPlayback.stop();
                            that.mPlayback.stopBGM();
                        } else {
                            that.mPlayback.stop();
                            if (that.mCurrentPageState === that.TPageState.EVideoPlaybackOpened) {
                                that.mPlayback.doHide();
                            }
                        }
                        dsrvDmr.setMedia(null);
                    }
                    dsrvVA.publish();
                }
            }
        };

        var onPreviousMediaPlay = () => {
            var centerPage = this.getCenterPageName();

            if (centerPage === 'CPgVideo' || centerPage === 'CPgPhoto' || centerPage === 'CPgMusic' ||
                centerPage === 'CPgRecordings') {
                if (this.mCurrentPageState === this.TPageState.EPhotoPlaybackOpened) {
                    var isSmartSearchEnabled = function () {
                        return !!($('#wrap_smt').find('#smt2.in').parent().css('opacity') === "1");
                    };
                    var isProgrammeDetailEnabled = function () {
                        return !!($('#wrap #pde2').parent().parent().parent().hasClass('show')) || !!($('#wrap_smt #pde2').parent().parent().css('opacity') === "1");
                    };
                    if (!isSmartSearchEnabled() && !isProgrammeDetailEnabled()) {
                        this._mainPage.doKey(hx.key.KEY_LEFT);
                    }
                    dsrvVA.publish();
                } else {
                    hx.log('CPgMain', 'onPlayStateFinished');
                    if (that.mPlayback) {
                        var prevPlayItem = that.mPlayback.getPrevPlayItem(that.mCurrentPlaybackItem);
                        that.mPlayNext = () => {
                            if (that.mPlayback) {
                                if (prevPlayItem) {
                                    if (prevPlayItem.isLocked && prevPlayItem.isLocked()) {
                                        var passwordDlg = fnCreateQueryPasswordDlg();
                                        passwordDlg.open({
                                            dimm: true,
                                            onPasswordEntered: function (password) {
                                                var result = hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                                                    password: password
                                                });
                                                if (result && (result.result === 0)) {
                                                    passwordDlg.close();
                                                    that.mPlayback.play(prevPlayItem);
                                                    if (that.mCurrentPageState === that.TPageState.EVideoPlaybackOpened) {
                                                        that.mPlayback.doShow();
                                                    }
                                                } else {
                                                    passwordDlg.reset();
                                                }
                                            }
                                        });
                                    } else {
                                        that.mPlayback.play(prevPlayItem);
                                        if (that.mCurrentPageState === that.TPageState.EVideoPlaybackOpened) {
                                            that.mPlayback.doShow();
                                        }
                                    }
                                } else if (that.mPlayback._bgmItemList === undefined) {
                                    that.mPlayback.close();
                                }
                                that.mPlayNext = null;
                            }
                        };
                        if (that.mCurrentPageState === that.TPageState.EPhotoPlaybackOpened) {
                            //dsrvPlayback.stop();
                            that.mPlayback.stopBGM();
                        } else {
                            that.mPlayback.stop();
                            if (that.mCurrentPageState === that.TPageState.EVideoPlaybackOpened) {
                                that.mPlayback.doHide();
                            }
                        }
                        dsrvDmr.setMedia(null);
                    }
                    dsrvVA.publish();
                }
            }
        };

        var onStartOver = () => {
            var centerPage = this.getCenterPageName();

            if (centerPage === 'CPgVideo' || centerPage === 'CPgPhoto' || centerPage === 'CPgMusic' ||
                centerPage === 'CPgRecordings') {
                if (that.mCurrentPageState !== that.TPageState.EPhotoPlaybackOpened) {
                    if (that.mPlayback) {
                        dsrvPlayback.seek({
                            offset : 0
                        });
                    }
                }
            }
            dsrvVA.publish();
        };

        // dsrvBridge
        dsrvBridge.addEventCb('RequestMusicPlayback', this, (e) => {
            onRequestMusicPlayback(e.items, e.startIndex);
        });
        dsrvBridge.addEventCb('RequestVideoPlayback', this, (e) => {
            onRequestVideoPlayback(e.items, e.startIndex, e.keyCode);
        });
        dsrvBridge.addEventCb('RequestPhotoPlayback', this, (e) => {
            onRequestPhotoPlayback(e.items, e.startIndex);
        });
        dsrvBridge.addEventCb('RequestInstanceRecording', this, (e) => {
            onRequestInstanceRecording(e.channel);
        });
        dsrvBridge.addEventCb('RequestStopPlayback', this, (e) => {
            onRequestStopPlayback();
        });

        // dsrvDial
        dsrvDial.addEventCb('ReqLaunch', this, (e) => {
            onReqLaunch(e.name, e.url);
        });
        dsrvDial.start();

        dsrvDial.addEventCb('ReqStop', this, (e)=> {
            onReqStop(e.name);
        });

        // dsrvDmr
        dsrvDmr.addEventCb('RequestPlay', this, (e) => {
            var dlnaItem = new CDlnaItem(e.item);
                onRequestPlay(dlnaItem);
        });
        dsrvDmr.addEventCb('RequestStop', this, (e) => {
            onRequestStop();
        });
        dsrvDmr.addEventCb('RequestPause', this, (e) => {
            onRequestPause();
        });
        dsrvDmr.addEventCb('RequestSetVolume', this, (e) => {
            onRequestSetVolume();
        });
        dsrvDmr.addEventCb('RequestMute', this, (e) => {
            onRequestMute();
        });
        dsrvDmr.addEventCb('RequestBringIn', this, (e) => {
            onRequestBringIn();
        });

        // dsrvPlayback
        dsrvPlayback.addEventCb('NoValidMusicFile', this, (e) => {
            onNoValidMusicFile();
        });
        dsrvPlayback.addEventCb('PlaySpeedChanged', this, (e) => {
            onPlaySpeedChanged(e.speed /*, e.item*/);
        });
        dsrvPlayback.addEventCb('PlaySpeedsArrayChanged', this, (e) => {
            onPlaySpeedsArrayChanged(e.supportedSpeedsArray);
        });
        dsrvPlayback.addEventCb('PlayPositionChanged', this, (e) => {
            onPlayPositionChanged(e.position, e.buffer);
        });
        dsrvPlayback.addEventCb('EndOfBufferReached', this, (e) => {
            onEndOfBufferReached();
        });
        dsrvPlayback.addEventCb('PlayStateStopped', this, (e) => {
            onPlayStateStopped();
        });
        dsrvPlayback.addEventCb('PlayStateFinished', this, (e) => {
            onPlayStateFinished();
        });
        dsrvPlayback.addEventCb('PlayStateError', this, (e) => {
            onPlayStateError(e.error);
        });
        dsrvPlayback.addEventCb('PlayStatePlaying', this, (e) => {
            onPlayStatePlaying(e.item);
        });
        dsrvPlayback.addEventCb('DRMRightsError', this, (e) => {
            onDRMRightsError(e.errorState, e.trickLimit);
        });
        dsrvPlayback.addEventCb('BGMListCreated', this, (e) => {
            onBGMListCreated(e.bgmItemList);
        });
        dsrvPlayback.addEventCb('TrickRestrictModeChanged', this, (e) => {
            onTrickRestrictModeChanged(e.trickMode);
        });
        hx.log('error','addEventCb AudioOutputRestrict!!!!!');
        dsrvPlayback.addEventCb('AudioOutputRestrict', this, (e) => {
            onAudioOutputRestrict();
        });
        hx.log('error','addEventCb AudioOutputRestrict END!!!!!');

        // dsrvRecordings
        dsrvRecordings.addEventCb('RecordingItemAdded', this, (e) => {
            onRecordingItemAdded(e.item);
        });
        dsrvRecordings.addEventCb('RecordingCompleted', this, (e) => {
            onRecordingCompleted(e.item);
        });

        // dsrvSettingUtil
        dsrvSettingUtil.addEventCb('NetworkAvailableChanged', this, (networkAvailable) => {
            if (!this.mNetworkAvailable && networkAvailable) { // false -> true
                if (dsrvWebStorage.getAlexaDeviceEnable() === true) {
                    var options = dsrvVA.getMQTTOptions();
                    var systemInfo = hx.svc.SETTING_UTIL.getSystemInfo();
                    var lanMacAddress = systemInfo.wiredMacAddress;

                    options.userId = lanMacAddress;
                    dsrvVA.createMQTTClient(options);
                    dsrvVA.connectMQTTClient(options);
                }
            } else if (this.mNetworkAvailable && !networkAvailable) { // true -> false
                if (dsrvWebStorage.getAlexaDeviceEnable() === true) {
                    dsrvVA.removeMQTTClient();
                }
            }
            this.mNetworkAvailable = networkAvailable;
        });
        dsrvSettingUtil.addEventCb('InternetAvailableChanged', this, (aInternetAvailable) => {
            onInternetAvailableChanged(aInternetAvailable);
        });
        dsrvSettingUtil.addEventCb('ThermalProtectionOccured', this, (e) => {
            onThermalProtectionOccured();
        });

        // dsrvStorageUtil
        dsrvStorageUtil.addEventCb('PhysicalStorageAttached', this, (e) => {
            onPhysicalStorageAttached(e.physicalStorage);
        });
        dsrvStorageUtil.addEventCb('PhysicalStorageDetached', this, (e) => {
            onPhysicalStorageDetached(e.storage);
        });
        dsrvStorageUtil.addEventCb('PhysicalStorageStatus', this, (e) => {
            onPhysicalStorageStatus(e.id, e.status);
        });

        // dsrvTsr
        dsrvTsr.addEventCb('PlaySpeedChanged', this, (e) => {
            onPlaySpeedChanged(e.speed);
        });
        dsrvTsr.addEventCb('PlayPositionChanged', this, (e) => {
            onPlayPositionChanged(e.position, e.buffer);
        });
        dsrvTsr.addEventCb('PlayBufferChanged', this, (e) => {
            onPlayBufferChanged(e.buffer);
        });
        //dsrvTsr.addEventCb('RecordingEvent', this, (e) => {
        //    onRecordingEvent(e.error, e.state, e.recordingId);
        //});
        dsrvTsr.addEventCb('LiveStarted', this, (e) => {
            onLiveStarted();
        });
        dsrvTsr.addEventCb('TsrPBStarted', this, (e) => {
            onTsrPBStarted();
        });
        dsrvTsr.addEventCb('LiveStopping', this, (e) => {
            onLiveStopping();
        });
        dsrvTsr.addEventCb('ProgrammesChanged', this, (e) => {
            onProgrammesChanged(e.programmes);
        });
        dsrvTsr.addEventCb('ChannelChangeSucceeded', this, (e) => {
            onChannelChangeSucceeded(e.channel);
        });
        dsrvTsr.addEventCb('ChannelChangeError', this, (e) => {
            onChannelChangeError(e.channel, e.errorState);
        });
        dsrvTsr.addEventCb('TrickRestrictModeChanged', this, (e) => {
            onTrickRestrictModeChanged(e.trickMode);
        });
        dsrvTsr.addEventCb('TrickError', this, (e) => {
            onTrickError();
        });
        dsrvSettingUtil.addEventCb('PlaybackOptionChange', this, (e) => {
            onPlaybackOptionChange(e);
        });
        dsrvAppMgr.addEventCb('ApplicationLoadError', this, function(appInfo) {
            onApplicationLoadError(appInfo);
        });
        dsrvAppMgr.addEventCb('ApplicationHide', this, () => {
            hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
        });

        dsrvAppMgr.addEventCb('HbbTvAppEnter', this, () => {
            that._HbbTvAppLoaded = true;//do not setChannel
            onRequestStopPlayback();
        });

        dsrvAppMgr.addEventCb('HbbTVAppUnloaded', this, () => {
            that._HbbTvAppLoaded = false;
        });

        //dsrvAvCtrl.addEventCb('evtSetChannel', this, (e) => {
        //    if (this.mPlayback) {
        //        this.mPlayback.close();
        //    }
        //});
        dsrvAvCtrl.addEventCb('MhegIcsStatus',this,(bIcsOn:number)=>{
            /*bIcsOn Off : 0 On : 1*/
            switch(bIcsOn){
                case 0:
                    break;
                case 1:
                    if (this.mPlayback) {
                        this.mPlayback.close();
                    }
                    break;
            }
        });
        dsrvAvCtrl.addEventCb('ChannelChangeSucceeded', this, function(e) {
            if (!hx.config.useNetflixPW) {
                return;
            }
            //onHideNetflixPW(e);
        });
        dsrvAvCtrl.addEventCb('HideNetflixPW', this, function(e) {
            if (!hx.config.useNetflixPW) {
                return;
            }
            onHideNetflixPW(e);
            //this.sendEventToEntity({
            //    alEvent : 'alMessageToPage',
            //    target : 'CPgLiveController?showpin=pin'
            //});
        });

        // dsrvNetflix
        dsrvNetflix.addEventCb('ChangeCPgLiveController', this, (e) => {
            onChangeCPgLiveController(e);
        });
        dsrvNetflix.addEventCb('ShowNetflixPW', this, (e) => {
            if (!hx.config.useNetflixPW) {
                return;
            }
            onShowPWForNetfixLaunch(e);
        });

        // dsrvVoiceAssistance
        dsrvVA.addEventCb('SkipChannels', this, (e) => {
            onSkipChannels(e);
        });
        dsrvVA.addEventCb('ChangeChannelByNumber', this, (e) => {
            onChangeChannelByNumber(e);
        });
        dsrvVA.addEventCb('ChangeChannelByName', this, (e) => {
            onChangeChannelByName(e);
        });
        dsrvVA.addEventCb('TurnOn', this, (e) => {
            onTurnOn();
        });
        dsrvVA.addEventCb('TurnOff', this, (e) => {
            onTurnOff();
        });
        dsrvVA.addEventCb('SetMute', this, (e) => {
            onSetMute();
        });
        dsrvVA.addEventCb('Next', this, (e) => {
            onNextMediaPlay();
        });
        dsrvVA.addEventCb('Previous', this, (e) => {
            onPreviousMediaPlay();
        });
        dsrvVA.addEventCb('StartOver', this, (e) => {
            onStartOver();
        });

        this._closeNetflixPW = function() {
            onHideNetflixPW();
        };
        this._checkPrivacyVersion = function() {
            onCheckPrivacyVersion();
        };
        this._onRequestVideoPlayback = function(items, startIndex, isLastPosition, keyCode) {
            dsrvBridge.requestVideoPlayback(items, startIndex, keyCode);
        };
        this._onRequestInstanceRecording = function(channel) {
            onRequestInstanceRecording(channel);
        };
        this._doCreateTvOption = (bMedia) => {
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            var dsrvWebStorage = hx.svc.WEB_STORAGE;
            var dsrvAvCtrl = hx.svc.AV_CTRL;
            var dsrvTsr2 = hx.svc.TSR;
            var dsrvRecordings = hx.svc.RECORDINGS;
            var i, tempIdx, audioLen, subtitleLen, supportedSubtitleList = [], supportedAudioList = [], supportedResList = [], displayList = [];
            var displayIdx = 0, audioIdx = 0, subtitleIdx = 0, resIdx = 0, repeatidx = 0;
            this._tvOptionDlg = prism.createSettingDlg(that._mainPage, {
                timeout : 5000
            });
            var avInfo = dsrvAvCtrl.getAVComponent();
            var pictureInfo = fnGetPictureInfo();
            var supportedSubtitles = fnGetSupportedSubtitles();
            var language = fnGetSubtitleLanguage();
            var bSubttlDisp = fnGetSubtitleDisplay();

            var _isSubtitleDisplay = dsrvSettingUtil.getSubtitleEnabled();
            var _audioModeHdmi, _audioModePcm, _audioType, _lipsyncDelay, _audioDescription;
            var retObjAudio = dsrvSettingUtil.getAudioInfo({
                cb : undefined
            });
            _audioModeHdmi = retObjAudio.audioModeHdmi;
            _audioModePcm = retObjAudio.audioModePcm;
            _audioType = retObjAudio.audioType;
            _lipsyncDelay = retObjAudio.lipSyncDelay;
            _audioDescription = retObjAudio.audioDescription;

            var videoOption = dsrvWebStorage.getVideoOption();
            if (that.mCurrentPlaybackItem && ((that.mCurrentPlaybackItem instanceof CTsrItem) || (that.mCurrentPlaybackItem.getFullPath().indexOf('HPVR://') === 0))){
                bMedia = false;
            }
            if(bMedia === false){
                this._tvOptionDlg.setTvOptionListItemsByData({
                    avInfo : avInfo,
                    pictureInfo : pictureInfo,
                    supportedSubtitles : _isSubtitleDisplay,
                    language : language,
                    AD : _audioDescription,
                    bMedia : bMedia,
                    //bSubttlDisp : bSubttlDisp,
                    repeatOption : Number(videoOption.repeatOption)
                });
            } else{
                this._tvOptionDlg.setTvOptionListItemsByData({
                    avInfo : avInfo,
                    pictureInfo : pictureInfo,
                    supportedSubtitles : supportedSubtitles,
                    language : language,
                    AD : _audioDescription,
                    bMedia : bMedia,
                    repeatOption : Number(videoOption.repeatOption)
                });
            }

            this._tvOptionDlg.setDrawParam('width', 700);
            this._tvOptionDlg.setTitleHtml(tr('LOC_SEL_OPTION_VAL_ID'));
            this._tvOptionDlg.onItemChanged = function(index, settingItem, changedValue) {
            };
            this._tvOptionDlg.onSettingsItemSelected = function(param, finished) {
                var index = param.itemKey;
                // TODO find settingListItems!!!
//                var handler = settingListItems[index].handler;
//                if (handler) {
//                    handler(this, finished);
//                }
            };
            this._tvOptionDlg.onOk = function(changedInfoList) {
                var i, length, optionIndex, optionValue, changedInfo;
                var option: {
                    repeatOption: any;
                    resolution?: any;
                    display?: any;
                    audio?: any;
                    subtitle?: any;
                    AD?: any;
                } = {
                    repeatOption: {}
                };

                var optionList = ['resolution', 'display', 'audio', 'subtitle', 'repeatOption'];
                length = changedInfoList.length;
                ////////////////////
                var mVideoOption = dsrvWebStorage.getVideoOption();
                option.repeatOption = {};
                option.repeatOption.nextValue = mVideoOption.repeatOption === undefined ? 0 : mVideoOption.repeatOption;
                ///////////////////
                for ( i = 0; i < length; i += 1) {
                    changedInfo = changedInfoList[i];
                    optionIndex = changedInfo.index;
                    if (!bMedia) {
                        option[optionList[optionIndex]] = {
                            prevValue : changedInfo.settingItem.config.selectionList[changedInfo.prevValue].value,
                            nextValue : changedInfo.settingItem.config.selectionList[changedInfo.changedValue].value
                        };
                    } else {
                        optionValue = changedInfo.settingItem.config.selectionList[changedInfo.changedValue].value;
                        option[optionList[optionIndex]] = {
                            nextValue : optionValue
                        };
                    }
                    prism.log('================================================this._doCreateTvOption this._tvOptionDlg.onOk========');
                    if (length === 1) {//Changed one 'resolution' or 'display' case
                        if (optionList[optionIndex] === 'resolution') {
                            switch (pictureInfo.videoDisplayFormat) {
                                case 'Pillarbox' :
                                case 'Letterbox' :
                                    pictureInfo.videoDisplayFormat = 'Sqeeze';
                                    break;
                                case 'Zoom' :
                                case 'Center' :
                                    pictureInfo.videoDisplayFormat = 'CenterCutOut';
                                    break;
                            }
                            option[optionList[1]] = {
                                nextValue : pictureInfo.videoDisplayFormat
                            };

                        } else if (optionList[optionIndex] === 'display') {
                            option[optionList[0]] = {
                                nextValue : pictureInfo.curResolution
                            };
                        }
                    }

                    if (optionList[optionIndex] === 'subtitle') {
                        option[optionList[optionIndex]].bMedia = bMedia;
                    }
                }
                if (!option.resolution) {
                    option.resolution = {};
                    option.resolution.nextValue = pictureInfo.curResolution;
                }
                if (!option.display) {
                    option.display = {};
                    option.display.nextValue = pictureInfo.videoDisplayFormat;
                }
                fnSetTvOptionInfo({
                    resolution : option.resolution,
                    display : option.display,
                    audio : option.audio,
                    subtitle : option.subtitle,
                    AD : option.AD,
                    repeatOption : option.repeatOption
                });
            };
            this._tvOptionDlg.doKeyEscape = function() {
                return this._tvOptionDlg.doKeyBack();
            }.bind(this);
            this._tvOptionDlg.open({
                transition : {
                    prevLayer : 'moveDown',
                    newLayer : 'moveUp',
                    dontRestore : true
                },
                onClosed : () => {
                    if (this.mPlayback && this.mPlayback.resetHidingTimer) {
                        this.mPlayback.resetHidingTimer();
                    }
                    this.setKeyHookingControl(this._tvOptionDlg);
                }
            });
            this.setKeyHookingControl(this._tvOptionDlg);
            return true;
        };
    }
    _openNotificationDlg(aMessage) {
        if (aMessage !== this._preMessage) {
            hx.msg('response', {
                text : aMessage,
                auto_close : true,
                close_time : 3000
            });
            this._preMessage = aMessage;
            setTimeout(() => {
                this._preMessage = null;
            }, 3000);
        }
        /*var mainPage = this._mainPage;
         if (mainPage) {
         var notificationDlg = prism.createNotificationDlg(mainPage, message);
         notificationDlg.open();
         }*/
    }
    _doActonPvrPlayKey() {
        var bActionPvrPlay: boolean = true, bChaseOngoing: boolean = false, bIsGetPvrHDD: boolean = false;

        if (hx.config.useTsrOnOff === 1) {
            var bTsrOnOff = false;

            bTsrOnOff = hx.svc.SETTING_UTIL.getpvrTsrOnOff();
            bChaseOngoing = hx.svc.TSR.isChaseOngoing();
            bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();

            if ((bTsrOnOff === false) && (bChaseOngoing !== true)) {
                bActionPvrPlay = false;
            } else {
                if (bIsGetPvrHDD === true) {
                    bActionPvrPlay = true;
                } else {
                    bActionPvrPlay = false;
                }
            }
        } else {
            bActionPvrPlay = true;
        }

        return bActionPvrPlay;
    }
    _openNotAvailable() {
        this.sendEventToEntity({
            alEvent : 'alMessageToPage',
            target : 'CPgNotAvailable?show'
        });
    }
    _showStatusMsg(aMessage) {
        this._hideStatusMsg();
        this._statusMsg = hx.msg('status', {
            text : aMessage,
            auto_close : false,
            callback_fn : function(p) {
            }
        });
    }
    _hideStatusMsg() {
        if (this._statusMsg !== null) {
            hx.msg.close('status', this._statusMsg);
            this._statusMsg = null;
        }
    }
    _isPlayPossiable() {
        var dsrvBridge = hx.svc.BRIDGE;
        var possible = true;
        var playEnabled = dsrvBridge.isPlayEnabled();
        if (!playEnabled) {
            hx.log('CPgMain', '_isPlayPossiable - Playback is not enabled');
            possible = false;
        } else if (hx.il.isCriticalTasking()) {
            hx.log('CPgMain', '_isPlayPossiable - Critical Tasking');
            possible = false;
        } else if (this.getPageVisible('CPgNotifier')) {
            hx.log('CPgMain', '_isPlayPossiable - CPgNotifier is Visible');
            possible = false;
        //} else if (this.getPageVisible('CPgMessage')) {
        //    hx.log('CPgMain', '_isPlayPossiable - CPgMessage is Visible');
        //    possible = false;
        } else if (hx.msg ? hx.msg.isOpend() : null) {
            hx.log('CPgMain', '_isPlayPossiable - CPgMessage is Visible');
            possible = false;
        } else if (prism.CButtonDlg.isOpened()) {
            hx.log('CPgMain', '_isPlayPossiable - prism.CButtonDlg.isOpened');
            possible = false;
        } else if (prism.CQueryPasswordDlg.isOpened()) {
            hx.log('CPgMain', '_isPlayPossiable - prism.CQueryPasswordDlg.isOpened');
            possible = false;
        } else if (prism.CSelectionDlg.isOpened()) {
            hx.log('CPgMain', '_isPlayPossiable - prism.CSelectionDlg.isOpened');
            possible = false;
        } else if (prism.isGlobalDialogOpened()) {
            hx.log('CPgMain', '_isPlayPossiable - prism.isGlobalDialogOpened');
            possible = false;
        }
        hx.log('CPgMain', hx.util.format('_isPlayPossiable - possible = {0}', possible));
        return possible;
    }
    _changePage(aPageName) {
        var that = this;
        var dsrvAppMgr = hx.svc.APP_MGR;
        var dsrvAvCtrl = hx.svc.AV_CTRL;
        var dsrvTVPortal = hx.svc.TV_PORTAL;

        if (aPageName === 'CPgMediaCenter') {
            this.sendEventToEntity({
                alEvent : 'alChangePage',
                target : 'CPgLiveController',
                me : this
            });
            var fnDestroyAllApps = function(turnOnLive?) {
                dsrvAppMgr.destroyAllApps();
                dsrvTVPortal.destroyActiveApp();
                if (turnOnLive) {
                    if (dsrvAvCtrl.isIdle()) {
                        dsrvAppMgr.createHbbTVApplication(false);
                        dsrvAvCtrl.unBlockAV();
                        if (!dsrvAvCtrl.lastChannel()) {
                            dsrvAvCtrl.availableChannel();
                        }
                    }
                }
            };
            var info = {
                title : 'Select the item as below.',
                video : hx.l('LOC_STR_VIDEO_01_ID'),
                photo : hx.l('LOC_STR_PHOTO_01_ID'),
                music : hx.l('LOC_STR_MUSIC_01_ID')
            };
            var selectionDlg = prism.createUsbWizardSelectionDlg(null, info, function(pageName) {
                that.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgHubProxy?forcedHide'
                });
                selectionDlg.close();
                fnDestroyAllApps(true);
                that._changePage(pageName);
            });
            selectionDlg.open({
                dimm : true,
                onClosed: () => {
                    this.setKeyHookingControl(selectionDlg);
                }
                /*transition : {
                 newLayer : 'scaleY'
                 }*/
            });
            hx.svc.SETTING_UTIL.doingKeyGrab(true);
            this.setKeyHookingControl(selectionDlg);
        } else {
            this.sendEventToEntity({
                alEvent : 'alChangePage',
                target : aPageName + '?refresh=true',
                me : this
            });
        }
        if (this.mPlayback && this.mPlayback.isShown()) {
            this.mPlayback.hide();
        }
    }
    _toggleAudio() {
        var currentTime = Date.now();
        var diff = Math.abs(currentTime - this._lastToggleTime);
        if (diff < 700) {
            return;
        }
        this._lastToggleTime = currentTime;
        var audioL = false;
        var audioR = false;
        var dualMono = hx.svc.AV_CTRL.getAudioComponent().audio;
        var retAudioSettings = hx.svc.SETTING_UTIL.getAudioInfo({});
        if (retAudioSettings) {
            if (retAudioSettings.audioType === 'Stereo') {
                if (dualMono.selected.language2 !== null) {
                    audioL = true;
                    audioR = false;
                } else {
                    audioL = true;
                    audioR = true;
                }
            } else if (retAudioSettings.audioType === 'Left') {
                audioL = true;
            } else if (retAudioSettings.audioType === 'Right') {
                audioR = true;
            }
        }
        var audioInfo = null;
        if (hx.msg.isOpendWithName('toggleAudio') === true) {
            audioInfo = hx.svc.AV_CTRL.toggleAudio();
        } else {
            audioInfo = hx.svc.AV_CTRL.getCurAudio();
        }
        if (audioInfo) {
            if (this._subToggleObj !== null) {
                hx.msg.close('toggleSubtitle', this._subToggleObj);
                this._subToggleObj = null;
            }
            if (this._audToggleObj !== null) {
                hx.msg.close('toggleAudio', this._audToggleObj);
                this._audToggleObj = null;
            }
            this._audToggleObj = hx.msg('toggleAudio', {
                'text' : audioInfo.message,
                'encoding' : audioInfo.encoding,
                'streoL' : audioL,
                'streoR' : audioR,
                'auto_close' : true,
                'close_time' : 3000
            });
        }
    }
    _toggleAudioDescription() {
        var AudioDescription;
        var currentTime = new Date().getTime();
        var diff = Math.abs(currentTime - this._lastToggleTime);
        if (diff < 700) {
            return;
        }
        this._lastToggleTime = currentTime;
        var retAudioSettings = hx.svc.SETTING_UTIL.getAudioInfo({});

        if (retAudioSettings) {
            if (retAudioSettings.audioDescription === true){
                AudioDescription = true;
            } else{
                AudioDescription = false;
            }
        }
        var audioInfo = null;
        if (hx.msg.isOpendWithName('toggleAudiodescription') === true) {
            audioInfo = hx.svc.AV_CTRL.toggleAudiodescription();
        } else {
            audioInfo = hx.svc.AV_CTRL.getCurAudiodescription();
        }
        if (audioInfo) {
            if (this._subToggleObj !== null) {
                hx.msg.close('toggleSubtitleonoff', this._subToggleObj);
                this._subToggleObj = null;
            }
            if (this._audToggleObj !== null) {
                hx.msg.close('toggleAudiodescription', this._audToggleObj);
                this._audToggleObj = null;
            }
            this._audToggleObj = hx.msg('toggleAudiodescription', {
                'text' : audioInfo.message,
                'AudioDescription' :  AudioDescription,
                'auto_close' : true,
                'close_time' : 3000
            });
        }
    }
    _toggleSubtitle() {
        var dsrvAvCtrl = hx.svc.AV_CTRL;
        var currentTime = new Date().getTime();
        var diff = Math.abs(currentTime - this._lastToggleTime);
        if (diff < 700) {
            return;
        }
        this._lastToggleTime = currentTime;
        var subtitleInfo = null;
        if (hx.msg.isOpendWithName('toggleSubtitle') === true) {
            subtitleInfo = dsrvAvCtrl.toggleSubtitle();
        } else {
            subtitleInfo = dsrvAvCtrl.getCurSubtitle();
        }
        if (subtitleInfo) {
            if (this._subToggleObj !== null) {
                hx.msg.close('toggleSubtitle', this._subToggleObj);
                this._subToggleObj = null;
            }
            if (this._audToggleObj !== null) {
                hx.msg.close('toggleAudio', this._audToggleObj);
                this._audToggleObj = null;
            }
            this._subToggleObj = hx.msg('toggleSubtitle', {
                'text' : subtitleInfo.message,
                'subType' : subtitleInfo.subType,
                'hardofHearing' : subtitleInfo.hardofHearing,
                'auto_close' : true,
                'close_time' : 3000
            });
        }
    }
    _toggleSubtitleOnOff() {
        var dsrvAvCtrl = hx.svc.AV_CTRL;
        var currentTime = new Date().getTime();
        var diff = Math.abs(currentTime - this._lastToggleTime);
        if (diff < 700) {
            return;
        }
        this._lastToggleTime = currentTime;
        var subtitleInfo = null;
        if (hx.msg.isOpendWithName('toggleSubtitleonoff') === true) {
            subtitleInfo = dsrvAvCtrl.toggleSubtitleonoff();
        } else {
            subtitleInfo = dsrvAvCtrl.getCurSubtitleonoff();
        }
        if (subtitleInfo) {
            if (this._subToggleObj !== null) {
                hx.msg.close('toggleSubtitleonoff', this._subToggleObj);
                this._subToggleObj = null;
            }
            if (this._audToggleObj !== null) {
                hx.msg.close('toggleAudiodescription', this._audToggleObj);
                this._audToggleObj = null;
            }
            this._subToggleObj = hx.msg('toggleSubtitleonoff', {
                'text' : subtitleInfo.message,
                'subType' : subtitleInfo.subType,
                'hardofHearing' : subtitleInfo.hardofHearing,
                'auto_close' : true,
                'close_time' : 3000
            });
        }
    }
    _on_alMessageToPage(param) {
        var that = this;
        switch (param.path) {
            case '/hub':
                if (this.mCurrentPageState === this.TPageState.EVideoPlaybackOpened) {
                    this.mPlayback.hide();
                }
                if (param.param) {
                    hx.log('CPgMain', hx.util.format('_on_alMessageToPage - change(page) to {0}', param.param));
                    if (this._tvOptionDlg) {
                        this._tvOptionDlg.close();
                        this._tvOptionDlg = null;
                    }
                    this._changePage(param.param);
                }
                break;
            case '/serverremove':
                var btn: string = hx.l('LOC_OK_ID');
                var confirmMsg = hx.msg('confirm', {
                    text : hx.l('LOC_STR_MESG_904_ID'),
                    auto_close : false,
                    dia_class : 'osd',
                    btn_title_arr : [btn],
                    callback_fn : function(param) {
                        if (param === btn) {
                            hx.msg.close('confirm', confirmMsg);
                            if (that.mPlayback) {
                                that.mPlayback.close();
                            }
                        }
                    }
                });
                break;
            case '/changeDuration':
                if (this.mCurrentPageState === this.TPageState.EVideoPlaybackOpened) {
                    if (this.mPlayback) {
                        this.mPlayback.setDuration(parseInt(param.query));
                    }
                }
                break;
            case '/CloseNetflixPW':
                this._closeNetflixPW();
                break;
            case '/checkPrivacyVersion':
                this._checkPrivacyVersion();
                break;
            default:
                break;
        }
    }

    _on_alKey(e): boolean {
        var consumed: boolean = false, k = hx.key, kc = e.alKey;
        var CTsrItem = __MODEL__.CTsrItem;
        var dsrvPlayback = hx.svc.PLAYBACK;
        var dsrvRecordings = hx.svc.RECORDINGS;
        var centerPageName = this.getCenterPageName();
        var isSmartSearchEnabled = function() {
            return !!($('#wrap_smt').find('#smt2.in').parent().css('opacity') === "1");
        };
        var isProgrammeDetailEnabled = function() {
            return !!($('#wrap #pde2').parent().parent().parent().hasClass('show')) || !!($('#wrap_smt #pde2').parent().parent().css('opacity') === "1");
        };
        //#107095
        if(this._HbbTvAppLoaded){
            switch (kc){
                case k.KEY_SEARCH:
                case k.KEY_SURROUND_MODE_NEXT:
                case k.KEY_SUBTITLE:
                case k.KEY_OPTION:
                case k.KEY_RECORD:
                    hx.log('CPgMain', '_on_alKey: Block REC/OPT/SUB/... KEY, keyCode=' + kc + ' caused by this._HbbTvAppLoaded');
                    return true;
            }
        }
        if (!isSmartSearchEnabled() && !isProgrammeDetailEnabled()) {
            consumed = this._mainPage.doKey(kc);
        }
        hx.log('CPgMain', '_on_alKey: keyCode=' + kc + ' consumed=' + consumed);
        var isAvailableKeyOnMainPage = function() {
            var keyEnabled = true;
            var $pages = $('#wrap').find('> .show').not('#main, #-test-infoHub');
            if ($pages.length > 0) {
                keyEnabled = false;
            }   // check pages
            if (isSmartSearchEnabled() || isProgrammeDetailEnabled()) {
                keyEnabled = false;
            }   // check smart search & programme detail
            return keyEnabled;
        };
        if (!consumed && isAvailableKeyOnMainPage()) {
            switch(kc) {
                case k.KEY_LEFT:
                case k.KEY_RIGHT:
                case k.KEY_UP:
                case k.KEY_DOWN:
                    break;
                case k.KEY_CHANNEL_UP:
                    if (this.mPlayback && !isSmartSearchEnabled() && this.mCurrentPlaybackItem instanceof CTsrItem && !this.getPageVisible('CPgInfoHub')) {
                        if (centerPageName === 'CPgLiveController') {
                            while (this._mainPage.stackSize()) {
                                this._mainPage.removeLayer();
                            }
                        }
                    }
                    break;
                case k.KEY_CHANNEL_DOWN:
                    if (this.mPlayback && !isSmartSearchEnabled() && this.mCurrentPlaybackItem instanceof CTsrItem && !this.getPageVisible('CPgInfoHub')) {
                        if (centerPageName === 'CPgLiveController') {
                            while (this._mainPage.stackSize()) {
                                this._mainPage.removeLayer();
                            }
                        }
                    }
                    break;
                case k.KEY_OK:
                    if (this.mPlayback && !isSmartSearchEnabled()) {
                        if (centerPageName === 'CPgLiveController') {
                            if (this.mPlayback.isShown()) {
                                this.mPlayback.hide();
                                consumed = true;
                            }
                            else {
                                if (this.mCurrentPageState && !(this.mCurrentPlaybackItem instanceof CTsrItem)) {
                                    this.mPlayback.show();
                                    consumed = true;
                                }
                            }
                        } else {
                            if ((this.mCurrentPageState === this.TPageState.EVideoPlaybackOpened) && (centerPageName !== 'CPgSettings')) {
                                if (!this.mPlayback.isShown()) {
                                    this.mPlayback.show();
                                } else {
                                    this.mPlayback.hide();
                                }
                                consumed = true;
                            }
                        }
                    }
                    break;
                case k.KEY_BACK:
                    break;
                case k.KEY_OPTION:
                    if (!isSmartSearchEnabled()) {
                        if (this.mCurrentPageState === this.TPageState.EVideoPlaybackOpened && !this.getPageVisible('CPgInfoHub')) {
                            this._doCreateTvOption(true);
                            consumed = true;
                        } else if (centerPageName === 'CPgLiveController' && !this.getPageVisible('CPgInfoHub') && !this.getPageVisible('CPgDetailInfo')) {
                            this._doCreateTvOption(false);
                            consumed = true;
                        }
                    }
                    break;
                case k.KEY_PLAY:
                case k.KEY_PAUSE:
                case k.KEY_FAST_FWD:
                case k.KEY_PLAY_SPEED_UP:
                case k.KEY_REWIND:
                case k.KEY_PLAY_SPEED_DOWN:
                    if (centerPageName === 'CPgLiveController' && !this.getPageVisible('CPgInfoHub') && !isSmartSearchEnabled()) {
                        this._onRequestVideoPlayback([new CTsrItem(null)], 0, false, kc);
                        consumed = true;
                    }
                    break;
                case k.KEY_STOP:
                    if (centerPageName === 'CPgLiveController') {
                        dsrvRecordings.requestStopRecording();
                        consumed = true;
                    }
                    break;
                case k.KEY_RECORD:
                    if (centerPageName === 'CPgLiveController' && !this.getPageVisible('CPgInfoHub')) {
                        hx.log('CPgMain', '[_on_alKey] Pushed the Record Key.');
                        if (this.mBlockRecKey === false) {
                            hx.log('CPgMain', '[_on_alKey] Request Instant Reocrding');
                            this._onRequestInstanceRecording();
                            this.mBlockRecKey = true;
                            setTimeout(() => {
                                this.mBlockRecKey = false;
                            }, 2000);
                        } else {
                            hx.log('CPgMain', '[_on_alKey] Blocked the Record Key.');
                        }
                        consumed = true;
                    }
                    break;
                case k.KEY_SEARCH:
                    if (this.mPlayback && this.mPlayback.isShown() && this.mCurrentPageState === this.TPageState.EVideoPlaybackOpened) {
                        this.mPlayback.hide();
                    }
                    break;
                case k.KEY_GUIDE:
                case k.KEY_PLAYER:
                case k.KEY_ON_DEMAND:
                    if (this.mPlayback && this.mPlayback.isShown()) {
                        this.mPlayback.hide();
                    }
                    break;
                case k.KEY_SCREEN_MODE_NEXT:
                    var pgwide = this._entity._repos.get('CPgWide');
                    if (!(pgwide.isActive())) {
                        this.sendEventToEntity({
                            alEvent : 'alMessageToPage',
                            target : 'CPgWide?show=toggle'
                        });
                        consumed = true;
                    }
                    break;
                case k.KEY_TELETEXT:
                    if (this.mPlayback && dsrvPlayback.isMpbOnGoing()) {
                        this.sendEventToEntity({
                            alEvent : 'alMessageToPage',
                            target : 'CPgNotAvailable?show'
                        });
                    } else {
                        hx.svc.APP_MGR.teletextToggle();
                    }
                    consumed = true;
                    break;
                case k.KEY_SUBTITLE:
                    this._toggleSubtitleOnOff();
                    consumed = true;
                    break;
                case k.KEY_SURROUND_MODE_NEXT:
                    this._toggleAudioDescription();
                    consumed = true;
                    break;
            }
            if (hx.config.netflix) {
                switch(kc) {
                    case k.KEY_NETFLIX:
                        if (this.mPlayback && this.mPlayback.isShown()) {
                            this.mPlayback.hide();
                        }
                        break;
                }
            }
        }
        //if ($smt.find('.programmeDetailDlg').length > 0 && this.mPlayback) {
        //    consumed = true;
        //}
        return consumed;
    }
    _on_alEndOfRepeatKey(e) {
        var k = hx.key, kc = e.alKey;
        hx.log('CPgMain', '_on_alEndOfRepeatKey: keyCode=' + kc);

        if (this.mPlayback && this.mPlayback.isShown()) {
            if (kc === hx.key.KEY_OK) {
                this.mPlayback.moveToCurrentPosition();
            } else {
                this.mPlayback.onKeyUp();
            }
        }
        return false;
    }
    visible() {
        var ret = super.visible();
        if (!ret) {
            var pgMessage = hx.al.entity.getRepository().get("CPgMessage");
            if (pgMessage) {
                ret = __PAGE_KERNEL__.prototype.visible.call(pgMessage);
            }
            if (!ret) {
                var pgLiveController = hx.al.entity.getRepository().get("CPgLiveController");
                if (pgLiveController) {
                    ret = __PAGE_KERNEL__.prototype.visible.call(pgLiveController);
                }
            }
        }
        return ret;
    }
}
export = cPgMain;
