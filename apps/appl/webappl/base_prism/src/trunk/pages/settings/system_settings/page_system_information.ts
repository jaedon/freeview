/*file name : pages/settings/system_settings/system_information.js */
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../../def/sugar.d.ts"/>
import __PAGE_KERNEL__ = require("pages/page_kernel");
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
import __NETFLIX__ = require("layer_domain/service/dl_dsrvNetflix");
declare var hx: ifSugar;
class cDetectNewSoftware extends __PAGE_KERNEL__ {
    _parent = null;
    _waiting = null;
    _ssuDialog = null;
    _copyWaiting = null;
    constructor(aParentObject) {
        super();
        this._parent = aParentObject
    }
    showWaitingProgress() {
        if (this._waiting === null) {
            this._waiting = hx.msg('progress', {
                'text' : hx.l('LOC_CHECKING_NEW_SOFTWARE_UPDATE_MSG_ID'),
                'auto_close' : false,
                'dia_class' : '',
                'callback_fn' : (status) => {
                    hx.log("CPgSystem", hx.util.format('++++++ ######## showWaitingProgress : status {0} ######## ++++++', status));

                    if (status === 'key_back') {
                        this.closeWaitingProgress();
                        this.cancel();
                    }
                }
            });
        }

        return this._waiting;
    }
    closeWaitingProgress() {
        if (this._waiting) {
            hx.msg.close('progress', this._waiting);
            this._waiting = null;
        }
    }
    closeUsbCopyWaitingProgress() {
        if (this._copyWaiting) {
            hx.msg.close('progress', this._copyWaiting);
            this._copyWaiting = null;
        }
    }
    getNewSoftwareWithType(aParamOtaType) {
        hx.svc.AV_CTRL.stopCurChannel({
                'stopAV' : 1,
                'stopTsr' : 1
        });

        hx.svc.SW_UPDATE.startDetectWithType({
                'otaType' : aParamOtaType
        });
    }
    cancel() {
        var a = this._parent;
        if (this._waiting) {
            hx.msg.close('progress', this._waiting);
            this._waiting = null;
        }

        hx.svc.SW_UPDATE.cancelDetect();
        if ((!hx.svc.PLAYBACK.isMpbOnGoing()) && (!hx.svc.TSR.isTsrOngoing()) && (!hx.svc.AV_CTRL.isMhegIcsOnGoing())) {
            this._changeToLastChannel();
        }

        a._state = 'normal';
    }
    showSuccessMessage(aCurrentVersion, aNewVersion, aCallbck) {
        var yes: string = hx.l('LOC_YES_ID'), no: string = hx.l('LOC_NO_ID');
        this._ssuDialog = hx.msg('ssuDialog', {
            title : hx.l('LOC_SOFTWAREUPDATE_ID'),
            text : hx.l('LOC_NEWSWDETECTED_ID'),
            text1 : hx.l('LOC_DOYOUWANTUPDATE_ID'),
            first_line_arr : [hx.l('LOC_CURSWVERSION_ID'), aCurrentVersion],
            second_line_arr : [hx.l('LOC_NEWSWVERSION_ID'), aNewVersion],
            auto_close : true,
            timeout : 30000,
            btn_title_arr : [yes, no],
            dia_class : 'osd',
            callback_fn : aCallbck
        });
        this.closeWaitingProgress();
        return this._ssuDialog;
    }
    showSuccessUSBMessage(aSuccessCB) {
        var yes: string = hx.l('LOC_YES_ID'), no: string = hx.l('LOC_NO_ID');

        var selectMsg = hx.msg('ssuDialog', {
            title : hx.l('LOC_SOFTWAREUPDATE_ID'),
            text : hx.l('LOC_USB_NEWSWDETECTED_ID'),
            text1 : hx.l('LOC_DOYOUWANTUPDATE_NEW_ID'),
            auto_close : true,
            timeout : 30000,
            btn_title_arr : [yes, no],
            dia_class : 'osd',
            callback_fn : (_status) => {
                if (_status === yes) {
                    this._copyWaiting = hx.msg('progress', {
                        text : hx.l('LOC_COPYING_USB_SOFTWARE_ID'),
                        auto_close : false,
                        dia_class : '',
                        callback_fn : (status) => {
                            if (status === 'key_back') {
                                hx.msg.close('progress', this._copyWaiting);
                                this._copyWaiting = null;
                                this.cancel();
                            }
                        }
                    });

                    if (aSuccessCB) {
                        aSuccessCB(this._copyWaiting);
                    }
                } else if (_status === no || _status === 'key_back' || _status === 'key_esc' || _status === 'auto_close') {
                    this.cancel();
                    hx.msg.close('ssuDialog', selectMsg);
                }
            }
        });

        this.closeWaitingProgress();

        return selectMsg;
    }
    showFailMessage() {
        hx.msg('response', {
            text : hx.l('LOC_NEW_SOFTWARE_NOT_DETECTED_ID'),
            auto_close : true,
            close_time : 3000
        });

        this.closeWaitingProgress();
        this.cancel();
    }
    _changeToLastChannel() {
        // tune to available channel
        var result = hx.svc.AV_CTRL.lastChannel();
        if (result === false) {
            hx.svc.AV_CTRL.availableChannel();
        }
    }
}
class cSoftwareDownload {
    _parent = null;
    _enumSource = hx.svc.SW_UPDATE.TSWUSource;
    _SUCCESSED = 1;
    _FAIL: number = 0;
    _progress: any;
    $: JQuery;
    _$title: JQuery;
    _$info: JQuery
    downloadBtn: any;
    _bStandbyOta: boolean;
    constructor(aTargetObject, aSource) {
        this._create(aTargetObject, aSource);
    }
    _create(targetObject, source) {
        this._parent = targetObject;
        this._bStandbyOta = this._parent._bStandbyOtaInstallInProgress || false;

        if ((source === this._enumSource.ESourceRf || source === this._enumSource.ESourceIp || this._bStandbyOta)) {
            this.$ = $('<article>', {
                'class' : 'wrap alignC colum2'
            });
            this._$title = $('<h2>', {
                'class' : 'infoTitle',
                text : hx.l('LOC_DOWNLOAD_PROGRESS_ID')
            });
            this._$info = $('<p>', {
                'class' : 'infoTxt',
                html : hx.l('LOC_DOWNLOAD_IS_BEING_DOWNLOAD_ID') + '<br><em>' + hx.l('LOC_DO_NOT_TURN_OFF_ID') + '</em>'
            });
            this._progress = hx.al.component('searchProgress', {
                tag : 'dl',
                data : [' ', '0%', 'width:0%'],
                attr : {
                    'class' : 'progress type3 infoProgress'
                }
            });
            this.downloadBtn = hx.al.CPgSystem.OTAButton('LOC_CANCEL_ID', 'download');

            this.$.append(this._$title).append(this._$info).append(this._progress.$).append(this.downloadBtn.$);
            $('.dateTime').after(this.$);

            this._parent.append(this.downloadBtn);
            this.downloadBtn.on();
        }

        if (hx.config.netflix) {
            hx.svc.NETFLIX.stop(__NETFLIX__.STOP_REASON_TYPE.Reason_USER_Kill);
        }

        // Standby OTA Installation Check
        var enumMultiOtaSpec;
        var enumOtaStep;
        if (this._bStandbyOta === true) {
            enumMultiOtaSpec = hx.svc.SW_UPDATE.TSWUSpec;
            enumOtaStep = hx.svc.SW_UPDATE.TSWUStep;
            hx.svc.SW_UPDATE.startSwupdateState({
                    otaSpec : enumMultiOtaSpec.ESPEC_HUMAX,
                    otaStep : enumOtaStep.EStepInstall
            });
        } else {// Basic Download process.
            enumMultiOtaSpec = hx.svc.SW_UPDATE.TSWUSpec;
            var ulOtaSpec = enumMultiOtaSpec.ESPEC_NONE;

            if (hx.config.useOTA === 'DVBSSU') {
                ulOtaSpec = enumMultiOtaSpec.ESPEC_SATDVBSSU;
            } else if (hx.config.useOTA === 'ASTRA') {
                ulOtaSpec = enumMultiOtaSpec.ESPEC_ASTRA;
            } else {
                ulOtaSpec = enumMultiOtaSpec.ESPEC_IRDETO;
            }

            hx.svc.SW_UPDATE.startDownload({
                    'otaSpec' : ulOtaSpec
            });
        }
    }
    destroy() {
        var $ = this.$;

        if (this.$ !== null) {
            this.$.remove();
        }

        if (this._parent._bStandbyOtaInstallInProgress) {
            this._parent._bStandbyOtaInstallInProgress = false;
        }
        if (hx.config.netflix) {
            hx.svc.NETFLIX.start({
                type: __NETFLIX__.START_TYPE.SUSPENDED_AFTER_APP_RESTART
            });
        }
    }
    stopDownload() {
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;

        hx.svc.SW_UPDATE.cancelDownload();
        dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
        if ((!hx.svc.PLAYBACK.isMpbOnGoing()) && (!hx.svc.TSR.isTsrOngoing()) && (!hx.svc.AV_CTRL.isMhegIcsOnGoing())) {
            this._changeToLastChannel();
        }
        this.destroy();
    }
    updateEvent(aParam) {
        var cmptProgress;
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        var SwUpdateEventInfo = aParam.SwUpdateEventInfo;
        var eventType = SwUpdateEventInfo.getInfo('event');
        var progress = SwUpdateEventInfo.getInfo('progress');
        var progressMax = SwUpdateEventInfo.getInfo('progress_max');
        var enumSWUEvent = hx.svc.SW_UPDATE.TSWUEvent;

        if ((eventType === enumSWUEvent.EEventDownloadFail) || (eventType === enumSWUEvent.EEventInstallFail)) { // DOWNLOAD FAIL
            this.stopDownload();
        } else if ((eventType === enumSWUEvent.EEventDownloadProgress) || (eventType === enumSWUEvent.EEventInstallProgress)) {
            if (this._parent._otaSource === this._enumSource.ESourceRf || this._parent._otaSource === this._enumSource.ESourceIp || (this._bStandbyOta === true)) {
                cmptProgress = this._progress;
                cmptProgress.$.find('dd').eq(0).html(progress + '%');
                cmptProgress.$.find('dd').eq(1).find('span').css('width', progress + '%');
            }

            hx.log("CPgSystem", "[updateEvent] progress : " + progress + ' / ' + progressMax);

            if (progress === progressMax) {
                this._parent._state = 'complete';
                dsrvFrontPanel.clearFrontPanelText();
                setTimeout(() => {
                    hx.svc.SW_UPDATE.restartSystem();
                }, 1000 * 2);
            }

            hx.svc.SETTING_UTIL.setFrontPanelString({
                    'text' : hx.util.format('D{0}{1}{2}%', parseInt(progress) / 100, parseInt(progress) % 100 / 10, parseInt(progress) % 10)
            });
        } else {
            hx.log("CPgSystem", "[updateEvent]Ignore Other messages: " + eventType);
            dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
        }
    }
    _changeToLastChannel() {
        // tune to available channel
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        var result = hx.svc.AV_CTRL.lastChannel();
        if (result === false) {
            hx.svc.AV_CTRL.availableChannel();
        }
        dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
    }
}
class cPgSystem extends __KERNEL_SETTINGS__ {
    aniConfig = {
        fadeIn : {
            start : {
                'opacity' : 0
            },
            end : {
                'opacity' : 1
            }
        },
        menu : {
            changeIn : {
                start : {
                    'opacity' : 0,
                    'left' : '20px'
                },
                end : {
                    'opacity' : 1,
                    'left' : 0
                }
            },
            changeOut3 : {
                start : {
                    'opacity' : 1,
                    'left' : 0
                },
                end : {
                    'opacity' : 0,
                    'left' : '20px'
                }
            }
        }
    };
    _state: string = 'normal';
    _FAIL: number = 0;
    _otaSource = null;
    _softwareDetect = null;
    _softwareDownload = null;
    _ssuDialog = null;
    _usbOTAConfirmMsg = null;
    _usbCopyWaiting = null;
    _isChangePageInSetting: boolean = false;
    _checkedPB: boolean = false; // to check playback status, only one time
    _ulRestPwdKeySeq: number = 0;
    _ulhiddenKeySeq: number = 0;
    TSystemHiddenAction = {
        ACTION_HIDDEN_NONE : 0,
        ACTION_HIDDEN_ALL : 1,
        ACTION_HIDDEN_PWRESET : 2
    };
    _TITLE: string;
    _FRONT_TITLE: string;
    _infoItems = [];
    _pageUrl = null;
    _enumSource = hx.svc.SW_UPDATE.TSWUSource;
    _$menuWrap: JQuery;
    _systemInformation: any;
    _newSoftwareBtn: any;
    _settingDlg: any;
    _bBootupOtaInProgress: boolean;
    constructor() {
        super();
    }
    create(aEntity, aPageUrl) {
        super.create();
        var i: number = 0;
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        this._pageUrl = aPageUrl;
        //this._TITLE = hx.l(hx.config.settings.systeminfo.title);
        this._TITLE = hx.l(this.productData.system_settings.system_information.title);
        this._FRONT_TITLE = this.productData.system_settings.system_information.title;



        for ( i = 0; i < this.productData.system_settings.system_information.itemId.length; i += 1) {
            if (this.productData.system_settings.system_information.itemId[i] === 'macaddlen') {
                this._infoItems[i] = hx.l(this.productData.system_settings.system_information.itemStr[i]) + ' (' + hx.l('LOC_LAN_ID') + ')';
            } else if (this.productData.system_settings.system_information.itemId[i] === 'macaddwlen') {
                this._infoItems[i] = hx.l(this.productData.system_settings.system_information.itemStr[i]) + ' (' + hx.l('LOC_WLAN_ID') + ')';
            } else if (this.productData.system_settings.system_information.itemId[i] === 'macaddbluetooth') {
                this._infoItems[i] = hx.l(this.productData.system_settings.system_information.itemStr[i]) + ' (' + hx.l('LOC_1_BLUETOOTH_ID') + ')';
            } else {
                this._infoItems[i] = hx.l(this.productData.system_settings.system_information.itemStr[i]);
            }
        }
            /*
        for ( i = 0; i < hx.config.settings.systeminfo.itemId.length; i += 1) {
            if (hx.config.settings.systeminfo.itemId[i] === 'macaddlen') {
                this._infoItems[i] = hx.l(hx.config.settings.systeminfo.itemStr[i]) + ' (' + hx.l('LOC_LAN_ID') + ')';
            } else if (hx.config.settings.systeminfo.itemId[i] === 'macaddwlen') {
                this._infoItems[i] = hx.l(hx.config.settings.systeminfo.itemStr[i]) + ' (' + hx.l('LOC_WLAN_ID') + ')';
            } else if (hx.config.settings.systeminfo.itemId[i] === 'macaddbluetooth') {
                this._infoItems[i] = hx.l(hx.config.settings.systeminfo.itemStr[i]) + ' (' + hx.l('LOC_1_BLUETOOTH_ID') + ')';
            } else {
                this._infoItems[i] = hx.l(hx.config.settings.systeminfo.itemStr[i]);
            }
        }  */


        hx.logadd('CPgSystem');
        this._init();

        this._$menuWrap = $('<article>', {
            'class' : 'wrap'
        });
        this._systemInformation = hx.al.CPgSystem.Menu(this._infoItems);
        this._newSoftwareBtn = hx.al.CPgSystem.OTAButton('LOC_CHECKNEWSOFT_ID', 'newSoftware');
        var $title = $('#set .title');
        if ($title[0]) {
            (<HTMLElement>$title[0].children[0]).innerHTML = this._TITLE;
        }

        this._draw();

        hx.al.compose(this);

        $title.ani({
            style : this.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });
        this._$menuWrap.ani({
            style : this.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            },
            cb : () => {
                this.append(this._newSoftwareBtn);
                this._newSoftwareBtn.on();
                this._isChangePageInSetting = false;

                // to detect new software
                this._softwareDetect = new cDetectNewSoftware(this);

                // auto start downloading
                if (!!hx.config.useWakeUpOTA || !!hx.config.useLiveIPOTA) {
                    if (!!this._pageUrl && this._pageUrl.param === "startBootupHumaxOta") {
                        this._bBootupOtaInProgress = true;
                        this._getNewSoftware();
                    }
                }
            }
        });
        this._checkedPB = false;
        // Have you checked the playback status?
    }
    _removeEventListeners() {
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        hx.svc.SW_UPDATE.removeEventListener(this);
        hx.svc.STORAGE_MASS.clearEventCb(this);
        dsrvFrontPanel.clearFrontPanelText();
        var dsrvRecordings = hx.svc.RECORDINGS;
        dsrvRecordings.clearEventCb(this, 'ScheduledWillBeStart');
    }
    destroy() {
        super.destroy();
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        this._removeEventListeners();

        if (this._softwareDownload) {
            this._softwareDownload.stopDownload();
            delete this._softwareDownload;
            this._softwareDownload = null;
        }

        if (this._softwareDetect) {
            this._softwareDetect.cancel();
            delete this._softwareDetect;
            this._softwareDetect = null;
        }

        this._$menuWrap.remove();
        if (!this._isChangePageInSetting) {
            $('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }
        dsrvFrontPanel.clearFrontPanelText();
        this._$menuWrap = undefined;
        this._systemInformation = undefined;
        this._newSoftwareBtn = undefined;
        this._settingDlg = undefined;
        this._updateCurrentOTATasking(false);
    }
    _init() {
        if (hx.config.useHiddenFunction) {
            this._ulRestPwdKeySeq = 0;
            this._ulhiddenKeySeq = 0;
        }
        hx.svc.SW_UPDATE.addEventListener({
                'event' : 'UpdateEvent',
                'objTarget' : this
        });
        hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageDetached', this, (aPhysicalStorage) => {
            this._on_PhysicalStorageDetached(aPhysicalStorage);
        });

        this._bindCurrentRecordingEvent();
    }
    _draw() {
        $('#set').append(this._$menuWrap.append(this._systemInformation.$).append(this._newSoftwareBtn.$));

        var datas = [], i, l;

        datas = this._getItemData();
        for ( i = 0, l = datas.length; i < l; i += 1) {
            if (this._systemInformation.getDtItem(i)[0].innerHTML === "ESN") {
                this._systemInformation.getDtItem(i)[0].style.width = 130;
                this._systemInformation.getItem(i)[0].style.width = 582;
            }
            this._systemInformation.getItem(i).html(datas[i]);
        }
    }
    _hide() {
        this._$menuWrap[0].style.display = 'none';
    }
    _show() {
        this._$menuWrap[0].style.display = 'block';
    }
    _on_NotiSignalInfo(aParam) {
        var i: number;
        //not consider 2 conn
        var strength: string = '0';
        var quality: string = '0';
        if (aParam.signal) {
            for ( i = 0; i < aParam.signal.length; i += 1) {
                if (aParam.signal[i]) {
                    if (aParam.signal[i].lock) {
                        strength = aParam.signal[i].strength;
                        quality = aParam.signal[i].quality;
                        break;
                    }
                }
            }
        }
        this._settingDlg.setStatusItem({
            type : 'signal',
            strength : parseInt(strength) * 0.8,
            quality : parseInt(quality) * 0.8
        });
    }
    _on_PhysicalStorageDetached(aPhysicalStorage) {
        var storageKind = aPhysicalStorage.kind;

        hx.log("CPgSystem", "========= [system_information][_on_PhysicalStorageDetached] =========");
        hx.log("CPgSystem", hx.util.format('otaWorking : {0}, otaSource : {1}, state : {2}', this._isOtaWorking(), this._otaSource, this._state));
    }
    _on_UpdateEvent(aParam) {
        var SwUpdateEventInfo = aParam.SwUpdateEventInfo;
        var detectSW = this._softwareDetect;
        var yes: string = hx.l('LOC_YES_ID'), no: string = hx.l('LOC_NO_ID');
        var enumSWUEvent = hx.svc.SW_UPDATE.TSWUEvent;

        var eventType = SwUpdateEventInfo.getInfo('event');
        var source = SwUpdateEventInfo.getInfo('source_type');

        if (eventType === enumSWUEvent.EEventDownloadProgress) {
            var pro = SwUpdateEventInfo.getInfo('progress');
            var proMax = SwUpdateEventInfo.getInfo('progress_max');
        }

        if (this._state === 'complete') {
            hx.log('warning', '\n *********** Completed OTA. will be restart system ***********\n');
            return true;
        }

        // auto update without user input
        if (!!hx.config.useWakeUpOTA || !!hx.config.useLiveIPOTA) {
            if (!!this._pageUrl && this._pageUrl.param === "startBootupHumaxOta") {
                this._pageUrl = null; // clear state
                if (this._bBootupOtaInProgress && (eventType === enumSWUEvent.EEventDetectSuccess)) {
                    this._otaSource = source;
                    this._closeDetectWaitingProgress();
                    this._startDownload(source);
                    // Bootup OTA may use IP, event if ota type is RF at the bootup time.
                    this._bBootupOtaInProgress = false;
                    return ;
                }
            }
        }

        var callback_fn = (aStatus: string) => {
            switch(aStatus) {
                case 'key_back':
                case 'key_power':
                case 'key_esc':
                case 'auto_close':
                case no:
                    this._cancelDetect();
                    break;
                case yes:
                    this._hide();
                    this._startDownload(source);
                    break;
            }
        }

        if (eventType === enumSWUEvent.EEventDetectSuccess) {
            hx.log("CPgSystem", 'Event Detect Success!!!');
            this._state = 'showDialog';
            this._otaSource = source;
            var curver = SwUpdateEventInfo.getInfo('current_version');
            var foundver = SwUpdateEventInfo.getInfo('found_version');
            if (source === this._enumSource.ESourceRf || source === this._enumSource.ESourceIp) {
                hx.log("CPgSystem", 'source : '+source);
                this._ssuDialog = detectSW.showSuccessMessage(curver, foundver, callback_fn);
            } else if (source === this._enumSource.ESourceUsb) {
                this._usbOTAConfirmMsg = detectSW.showSuccessUSBMessage((waitingHandle) => {
                    this._usbCopyWaiting = waitingHandle;
                    this._startDownload(source);
                });
            }

        } else if (eventType === enumSWUEvent.EEventDetectFail) {
            hx.log("CPgSystem", 'Event Detect Fail!!!');
            this._state = 'normal';
            this._updateCurrentOTATasking(false);

            this._closeDetectWaitingProgress();
            detectSW.showFailMessage();

            hx.log("CPgSystem", hx.util.format('ssuDialog handle : {0}, usbOTAConfirmMsg handle : {1}', this._ssuDialog, this._usbOTAConfirmMsg));
        } else if (this._softwareDownload) {
            var progress = SwUpdateEventInfo.getInfo('progress');
            var progressMax = SwUpdateEventInfo.getInfo('progress_max');
            if ((eventType === enumSWUEvent.EEventDownloadFail) || (eventType === enumSWUEvent.EEventInstallFail)) {
                var text = hx.l('LOC_DOWNLOAD_FAIL_ID');
                var ok_btn = hx.l('LOC_OK_ID');
                var confirmMsg = hx.msg('confirm', {
                    'text' : text,
                    'auto_close' : false,
                    'btn_title_arr' : [ok_btn],
                    'callback_fn' : (_status) => {
                        // hx.log("CPgSystem", hx.util.format('[_on_UpdateProgress] _status : {0}', _status));
                        if (_status === ok_btn || _status === 'key_back' || _status === 'key_esc') {
                            this._state = 'normal';
                            this._show();
                            this._newSoftwareBtn.on();
                            hx.msg.close('confirm', confirmMsg);
                        }
                    }
                });

                if (this._usbCopyWaiting) {
                    hx.log("CPgSystem", 'hx.msg.close[usbCopyWaiting - progress]');

                    hx.msg.close('progress', this._usbCopyWaiting);
                    this._usbCopyWaiting = null;
                }

                this._updateCurrentOTATasking(false);
            }
            this._softwareDownload.updateEvent(aParam);
        }
    }
    _on_alClicked(aPAram): boolean {
        var bConsumed: boolean = false, func;

        function clickedFunc(aName) {
            return {
                'newSoftware' : 'getNewSoftware',
                'download' : 'stopDownload'
            }[aName];
        }


        hx.log("CPgSystem", hx.util.format('++++++ ######## _on_alClicked : _isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        func = '_' + clickedFunc(aPAram.alData.me.name);

        hx.log("CPgSystem", hx.util.format('name : {0}, func : {1}', aPAram.alData.me.name, func));

        if (this[func]) {
            this[func]();

            bConsumed = true;
        }

        return bConsumed;
    }
    _on_alExit(aParam): boolean {
        var bConsumed:boolean = false;
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;

        hx.log("CPgSystem", hx.util.format('++++++ ######## _on_alExit : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        if (this._state === 'downloading') {
            bConsumed = true;
            this._stopDownload();
            dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
        } else {
            if (aParam.alData) {
                bConsumed = true;
                this._isChangePageInSetting = true;
                hx.al.decompose(this);
                dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
                this._$menuWrap.ani({
                    style : this.aniConfig.menu.changeOut3,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb : () => {
                        this.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings',
                            me : this
                        });
                    }
                });
            }
        }
        return bConsumed;
    }
    _on_alKey(aParam): boolean {
        var bConsumed: boolean = false;
        var dsrvFrontPanel = hx.svc.FRONT_PANEL;
        if (hx.config.useHiddenFunction) {
            var ulHiddenAction = this.TSystemHiddenAction.ACTION_HIDDEN_NONE;
        }

        switch (aParam.alKey) {
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
            case hx.key.KEY_POWER :
                if (this._isOtaWorking()) {
                    bConsumed = true;
                }
                break;
            case hx.key.KEY_BACK:
                dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
                bConsumed = true;
                break;
            case hx.key.KEY_ESCAPE:
                if (aParam.alData) {
                    bConsumed = true;
                    dsrvFrontPanel.clearFrontPanelText();
                    this.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgLiveController',
                        me : this
                    });
                }
                break;
            default:
                if (hx.config.useHiddenFunction) {
                    ulHiddenAction = this._checkHiddenMenuKeySeq(aParam.alKey);
                    if (ulHiddenAction !== this.TSystemHiddenAction.ACTION_HIDDEN_NONE) {
                        this._isChangePageInSetting = true;
                        this.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgHiddenSetting?' + ulHiddenAction,
                            me : this
                        });
                    }
                }
                break;
        }
        return bConsumed;
    }
    _check_ScheduledItems_WillbeStarted(): boolean {
        var dsrvRecordings, scheduleList;
        var i, ti, item, sooner, nowTime, currentTime, hasItem: boolean = false;

        dsrvRecordings = hx.svc.RECORDINGS;
        scheduleList = dsrvRecordings.getScheduledList();

        if (scheduleList.length) {
            sooner = scheduleList[0];
            for ( i = 1, ti = scheduleList.length; i < ti; i += 1) {
                item = scheduleList[i];
                if (sooner.getStartTime() > item.getStartTime()) {
                    sooner = item;
                }
            }

            if (!sooner.isReminder()) {
                nowTime = new Date();
                var time = sooner.getStartTime() - nowTime;
                if ((time > 0) && (time < 30000)) {
                    hasItem = true;
                }
            }
        }

        hx.log("CPgSystem", hx.util.format('++++++ ######## [_check_ScheduledItems_WillbeStarted] hasItem = {0} ######## ++++++', hasItem));

        return hasItem;
    }
    _getNewSoftware() {
        var currentRecordings, length: number;
        var bPbGoing: boolean = false;
        var dsrvSWUpdate = hx.svc.SW_UPDATE;

        hx.log("CPgSystem", hx.util.format('++++++ ######## _getNewSoftware : _isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        if (this._state !== 'normal') {
            return;
        }

        /* check current playback status */
        if (this._checkedPB === false) {
            bPbGoing = hx.svc.TSR.isTsrOngoing() || hx.svc.TSR.isChaseOngoing() || hx.svc.PLAYBACK.isMpbOnGoing();
            this._checkedPB = true;
        }
        /* if current status is playback(PB_TSR, PB_CHASE, PB_USR, PB_MED), goto the live broadcast mode. */
        if (bPbGoing) {
            hx.svc.BRIDGE.requestStopPlayback();
            hx.log("CPgSystem", "Since the current State is playback, go to the live broadcast!!!");
            setTimeout(() => {
                this._getNewSoftware();
            }, 1000 * 3);
            return;
        }

        currentRecordings = this._getCurrentRec();
        length = currentRecordings.length;

        var dsrvRecordings = hx.svc.RECORDINGS;
        var scheduleList = dsrvRecordings.getScheduledList();

        var liveStreamingItem = null;
        if (scheduleList.length) {
            var i;
            for ( i = 0; i < scheduleList.length; i += 1) {
                var item = scheduleList[i];
                if (item.isStreaming()) {
                    liveStreamingItem = item;
                    break;
                }
            }
        }

        if (length || liveStreamingItem) {
            var yes = hx.l('LOC_YES_ID'), later = hx.l('LOC_LATER_ID'), message = hx.l('LOC_OTA_RECORDING_IS_IN_PROCESS_ID');
            var selectMsg = hx.msg('confirm', {
                'text' : message, //The recording is in process. Do you want to continue updating the new software now?
                'auto_close' : true,
                'timeout' : 30000,
                'btn_title_arr' : [yes, later],
                'btn_last_focus' : 1,
                'dia_class' : 'osd',
                'callback_fn' : (_status) => {
                    hx.log("CPgSystem", hx.util.format('[confirm][_getNewSoftware] status', _status));
                    if (_status === yes) {// OTA Check!
                        this._showDetectWaitingProgress();
                        var cb = () =>{
                            hx.log("CPgSystem", '------> GO! _tryGetNewSoftwareDetect');
                            this._tryGetNewSoftwareDetect();
                        };
                        if (liveStreamingItem) {
                            dsrvRecordings.removeScheduledRecording({
                                'items' : [liveStreamingItem],
                                'finished' : function(result) {
                                    if (result) {
                                        cb();
                                    }
                                }
                            });
                        } else {
                            this._stopCurrentRecordings(cb);
                        }
                    } else if (_status === later || _status === 'key_back' || _status === 'key_esc' || _status === 'auto_close') {
                        hx.msg.close('confirm', selectMsg);
                        this._closeDetectWaitingProgress();
                    }
                }
            });
        } else {
            this._showDetectWaitingProgress();
            //show Progress : 'Searching for new software...'
            var scheduleTimerId = null, has_HotScheduledItem;
            has_HotScheduledItem = this._check_ScheduledItems_WillbeStarted();
            if (has_HotScheduledItem) {
                scheduleTimerId = setTimeout(() => {
                    clearTimeout(scheduleTimerId);
                    scheduleTimerId = null;
                    this._getNewSoftware();
                }, 30000);
            }
            if (scheduleTimerId === null) {
                this._tryGetNewSoftwareDetect();
            }
        }
    }
    _stopCurrentRecordings(aCallback) {
        hx.log("CPgSystem", hx.util.format('++++++ ######## _stopCurrentRecordings : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        var dsrvRecordings = hx.svc.RECORDINGS;
        var currentRecordingList = dsrvRecordings.getCurrentRecordings();
        if (currentRecordingList && currentRecordingList.length) {
            dsrvRecordings.stopRecord({
                'items' : currentRecordingList,
                'finished' : function(result) {
                    hx.log("CPgSystem", hx.util.format('Recordings STOP OK! . result - {0}', result));

                    if (result) {
                        aCallback();
                    }
                }
            });
        }
    }
    _doScheduledEvent(aParam) {
        var paramItem = aParam.item;
        var paramRes = aParam.res;

        var setResult = (item, res) => {
            var channel, bChangeable = false;
            var dsrvRecordings = hx.svc.RECORDINGS;

            dsrvRecordings.recordConfirm({
                scheduledRecording : item,
                type : 0,
                check : res
            });

            if ((res === 1) && item.isReminder()) {
                hx.log('CPgSystem', '[ScheduleEvent] reminder result : yes');
                channel = item.getChannelInstance();
                //if (this.getCenterPageName() === 'CPgTvApps') {}

                ///////////////////////////////
                // check tunable
                bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                        channel : channel
                });
                if (bChangeable === false) {
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

                ///////////////////////////////
                // set last channel
                var o = hx.svc.CH_LIST.getAvailableGroupForChannel({
                        'channel' : channel
                });

                if (o.channel !== null && o.group !== null) {
                    hx.svc.WEB_STORAGE.updateLastService({
                            'ccid' : o.channel.ccid,
                            'group' : o.group
                    });
                }

                ///////////////////////////////
                // exit page
                hx.log('CPgSystem', '[ScheduleEvent] reminder result : page change to liveController');
                // the following statement doesn't work, is not needed since it is done just after OTA stopped.
                this.sendEventToEntity({
                    'alEvent' : 'alChangePage',
                    'target' : 'CPgLiveController',
                    'me' : this
                });
                ///////////////////////////////
                // channel change
                if (o.channel !== null && o.group !== null) {
                    var fnChChange = function() {
                        hx.log('CPgSystem', '[ScheduleEvent] reminder result : channel change!!!!');
                        var VBcurrentChannel = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
                        if (VBcurrentChannel) {
                            if (VBcurrentChannel.ccid === o.channel.ccid) {
                                hx.log('CPgSystem', '[ScheduleEvent] reminder result : return!! same channel!!!');
                                return;
                            }
                        }
                        hx.svc.AV_CTRL.channelChange({
                                channel : o.channel,
                                group : o.group
                        });
                    };
                    setTimeout(fnChChange, 0);
                }
            } else if ((res === 1) && !item.isReminder()) {
                var chInfo;
                channel = item.getChannelInstance();
                chInfo = hx.svc.CH_LIST.getAvailableGroupForChannel({
                        'channel' : channel
                });
                if ((chInfo.channel !== null) && (chInfo.chInfo !== null)) {
                    hx.svc.AV_CTRL.channelChange({
                            channel : chInfo.channel,
                            group : chInfo.group
                    });
                }
            }

        };
        setResult(paramItem, paramRes);
    }
    _willTheRecordingStartNowQuestion2(aItem) {
        var yes: string = hx.l('LOC_YES_ID'), later: string = hx.l('LOC_LATER_ID'), message: string = hx.l('LOC_OTA_RECORDING_WILL_START_ID');
        var dsrvRecordings = hx.svc.RECORDINGS;

        hx.log("CPgSystem", hx.util.format('++++++ ######## _willTheRecordingStartNowQuestion : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        if (this._state === 'downloading') {
            if (aItem.isReminder()) {
                hx.log("CPgSystem", "++++++ ######## The scheduled Reminder is canceld, becasue STB is downloading SW  ######## ++++++");
                dsrvRecordings.removeScheduledRecording({
                    items : [aItem]
                });
            } else {
                hx.log("CPgSystem", "++++++ ######## Process Scheduled Recording Event during OTA  ######## ++++++");
                var selectMsg = hx.msg('confirm', {
                    text : message, //The recording will start now. Do you want to continue updating the new software now?
                    auto_close : true,
                    timeout : 20000,
                    btn_title_arr : [yes, later],
                    btn_last_focus : 1,
                    dia_class : 'osd',
                    callback_fn : (_status) => {
                        if (_status === yes) {
                            hx.msg.close('confirm', selectMsg);
                            dsrvRecordings.removeScheduledRecording({
                                items : [aItem]
                            });
                        } else if (_status === later || _status === 'key_back' || _status === 'key_esc' || _status === 'auto_close') {
                            hx.msg.close('confirm', selectMsg);
                            this._stopDownload();
                        }
                    }
                });
            }
        }
    }
    _getCurrentRec() {
        var dsrvRecordings, currentRecordings;

        dsrvRecordings = hx.svc.RECORDINGS;
        currentRecordings = hx.svc.RECORDINGS.getCurrentRecordings();

        return currentRecordings;
    }
    _showDetectWaitingProgress() {
        hx.log("CPgSystem", hx.util.format('++++++ ######## _showDetectWaitingProgress : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        if (this._softwareDetect) {
            this._softwareDetect.showWaitingProgress();
        }
    }
    _closeDetectWaitingProgress() {
        hx.log("CPgSystem", hx.util.format('++++++ ######## _closeDetectWaitingProgress : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        if (this._softwareDetect) {
            this._softwareDetect.closeWaitingProgress();
        }
    }
    _tryGetNewSoftwareDetect() {
        var enumMultiOtaSpec = hx.svc.SW_UPDATE.TSWUSpec;
        var ulOtaSpec = enumMultiOtaSpec.ESPEC_NONE;

        hx.log("CPgSystem", hx.util.format('++++++ ######## _tryGetNewSoftwareDetect : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        if (hx.config.useOTA === 'DVBSSU') {
            ulOtaSpec = enumMultiOtaSpec.ESPEC_SATDVBSSU;
        } else if (hx.config.useOTA === 'ASTRA') {
            ulOtaSpec = enumMultiOtaSpec.ESPEC_ASTRA;
        } else {
            ulOtaSpec = enumMultiOtaSpec.ESPEC_IRDETO;
        }

        if (this._softwareDetect) {
            this._state = 'swdetect';
            this._softwareDetect.getNewSoftwareWithType(ulOtaSpec);
            this._updateCurrentOTATasking(true);
        }
    }
    _cancelDetect() {
        hx.log("CPgSystem", hx.util.format('++++++ ######## _cancelDetect : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        this._state = 'normal';
        this._softwareDetect.cancel();
        this._newSoftwareBtn.on();

        this._updateCurrentOTATasking(false);
    }
    _startDownload(aSource) {
        var currentRecordings, length: number;

        hx.log("CPgSystem", hx.util.format('++++++ ######## _startDownload : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        currentRecordings = this._getCurrentRec();
        length = currentRecordings.length;

        if (length) {
            var yes = hx.l('LOC_YES_ID'), later = hx.l('LOC_LATER_ID'), message = hx.l('LOC_OTA_RECORDING_IS_IN_PROCESS_ID');
            var selectMsg = hx.msg('confirm', {
                'text' : message, //The recording is in process. Do you want to continue updating the new software now?
                'auto_close' : true,
                'timeout' : 30000,
                'btn_title_arr' : [yes, later],
                'btn_last_focus' : 1,
                'dia_class' : 'osd',
                'callback_fn' : (_status: string) => {
                    hx.log("CPgSystem", hx.util.format('[startDownload] _status : {0}, source : {1}', _status, aSource));

                    if (_status === yes) {// OTA Check!
                        this._showDetectWaitingProgress();
                        this._stopCurrentRecordings(() => {
                            this._startDownload(aSource);
                        });
                    } else if (_status === later || _status === 'key_back' || _status === 'key_esc' || _status === 'auto_close') {
                        hx.msg.close('confirm', selectMsg);

                        this._show();
                        this._newSoftwareBtn.on();

                        this._updateCurrentOTATasking(false);
                    }
                }
            });
        } else {
            var has_HotScheduledItem, scheduleTimerId = null;

            has_HotScheduledItem = this._check_ScheduledItems_WillbeStarted();
            if (has_HotScheduledItem) {
                scheduleTimerId = setTimeout(() => {
                    clearTimeout(scheduleTimerId);
                    scheduleTimerId = null;
                    this._startDownload(aSource);
                }, 30000);
            } else {
                this._softwareDownload = new cSoftwareDownload(this, aSource);
                this._state = 'downloading';
            }
        }
    }
    _stopDownload() {
        hx.log("CPgSystem", hx.util.format('++++++ ######## _stopDownload : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        this._state = 'normal';
        this._otaSource = null;

        if (this._softwareDownload) {
            this._softwareDownload.stopDownload();
        }

        this._show();
        this._newSoftwareBtn.on();

        this._updateCurrentOTATasking(false);
    }
    _bindCurrentRecordingEvent() {
        hx.log("CPgSystem", hx.util.format('++++++ ######## _bindCurrentRecordingEvent : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));

        var dsrvRecordings = hx.svc.RECORDINGS;
        dsrvRecordings.addEventCb('ScheduledWillBeStart', this, (param) => {
            var item = param.item;

            hx.log("CPgSystem", hx.util.format('######## [ScheduledWillBeStart] state : {0} ########', this._state));

            if (this._state !== 'downloading') {
                return;
            }
            if (item.isRecordingOnGoing()) {
                this._willTheRecordingStartNowQuestion2(item);
            }
        });
    }
    _updateCurrentOTATasking(currentState) {
        hx.il.criticalTasking.crOTATasking = currentState;
        if (currentState) {
            // DMR uri set null ('bring in client' do not have to work)
            hx.svc.DMR.setMedia(null);
        }
    }
    _isOtaWorking() {
        return hx.il.criticalTasking.crOTATasking;
    }
    _getItemData() {
        var datas = [];
        var systemInfo = hx.svc.SETTING_UTIL.getSystemInfo();
        datas.push(systemInfo.modelName);
        datas.push(systemInfo.softwareVersion);
        datas.push(systemInfo.loaderVersion);
        datas.push(systemInfo.system_id);
        datas.push(systemInfo.update_date);
        datas.push(systemInfo.wiredMacAddress);
        datas.push(systemInfo.wirelessMacAddress);
        if (hx.config.netflix) {
            datas.push(systemInfo.esn);
        }
        return datas;
    }
    _checkHiddenMenuKeySeq(aKey: number): number {
        var ulActionHiddenType: number = 0, bEnable: boolean = false;

        ulActionHiddenType = this.TSystemHiddenAction.ACTION_HIDDEN_NONE;

        bEnable = this._checkHiddenMenu(aKey);
        if (bEnable === true) {
            ulActionHiddenType = this.TSystemHiddenAction.ACTION_HIDDEN_ALL;
            return ulActionHiddenType;
        }

        if (hx.config.useHiddenPWReset) {
            bEnable = this._checkHiddenMenuPasswordReset(aKey);
            if (bEnable === true) {
                ulActionHiddenType = this.TSystemHiddenAction.ACTION_HIDDEN_PWRESET;
                return ulActionHiddenType;
            }
        }
        return ulActionHiddenType;
    }
    _checkHiddenMenuPasswordReset(aKey: number): boolean {
        switch(aKey) {
            case hx.key.KEY_7:
                if (this._ulRestPwdKeySeq === 0) {
                    this._ulRestPwdKeySeq += 1;
                } else {
                    this._ulRestPwdKeySeq = 0;
                }
                break;
            case hx.key.KEY_4:
                if (this._ulRestPwdKeySeq === 1) {
                    this._ulRestPwdKeySeq += 1;
                } else {
                    this._ulRestPwdKeySeq = 0;
                }
                break;
            case hx.key.KEY_1:
                if (this._ulRestPwdKeySeq === 2) {
                    this._ulRestPwdKeySeq += 1;
                } else {
                    this._ulRestPwdKeySeq = 0;
                }
                break;
            case hx.key.KEY_2:
                if (this._ulRestPwdKeySeq === 3) {
                    this._ulRestPwdKeySeq += 1;
                } else {
                    this._ulRestPwdKeySeq = 0;
                }
                break;
            case hx.key.KEY_3:
                if (this._ulRestPwdKeySeq === 4) {
                    this._ulRestPwdKeySeq += 1;
                } else {
                    this._ulRestPwdKeySeq = 0;
                }
                break;
            case hx.key.KEY_5:
                if (this._ulRestPwdKeySeq === 5) {
                    this._ulRestPwdKeySeq += 1;
                } else {
                    this._ulRestPwdKeySeq = 0;
                }
                break;
            case hx.key.KEY_9:
                if (this._ulRestPwdKeySeq === 6) {
                    return true;
                }
                this._ulRestPwdKeySeq = 0;
                break;
            default:
                this._ulRestPwdKeySeq = 0;
                break;
        }
        return false;
    }
    _checkHiddenMenu(aKey: number): boolean {
        switch(aKey) {
            case hx.key.KEY_RED:
                if (this._ulhiddenKeySeq === 0) {
                    this._ulhiddenKeySeq += 1;
                } else {
                    this._ulhiddenKeySeq = 0;
                }
                break;
            case hx.key.KEY_GREEN:
                if ((this._ulhiddenKeySeq === 1) || (this._ulhiddenKeySeq === 4)) {
                    this._ulhiddenKeySeq += 1;
                } else {
                    this._ulhiddenKeySeq = 0;
                }
                break;
            case hx.key.KEY_YELLOW:
                if ((this._ulhiddenKeySeq === 2) || (this._ulhiddenKeySeq === 5)) {
                    this._ulhiddenKeySeq += 1;
                } else {
                    this._ulhiddenKeySeq = 0;
                }
                break;
            case hx.key.KEY_BLUE:
                if (this._ulhiddenKeySeq === 3) {
                    this._ulhiddenKeySeq += 1;
                } else if (this._ulhiddenKeySeq === 6) {
                    return true;
                } else {
                    this._ulhiddenKeySeq = 0;
                }
                break;
            default:
                this._ulhiddenKeySeq = 0;
                break;
        }
        return false;
    }
}
hx.al.CPgSystem = cPgSystem;
hx.al.CPgSystem.Menu = function(items) {
    return hx.al.component('table', {
        attr : {
            'class' : 'info box'
        },
        langID : false,
        items : items,
        describe : hx.config.settings.systeminfo.describe,
        count : hx.config.settings.systeminfo.describe.length
    });
};

hx.al.CPgSystem.OTAButton = function(langID, name) {
    return hx.al.component('button', {
        attr : {
            'class' : 'button on',
            'data-langID' : langID
        },
        usePointerKey : true,
        notSelfDestroy : true,
        name : name
    });
};
export = cPgSystem;
