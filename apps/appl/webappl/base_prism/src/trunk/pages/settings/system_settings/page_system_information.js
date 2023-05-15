var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "pages/settings/kernel_settings", "layer_domain/service/dl_dsrvNetflix"], function (require, exports, __PAGE_KERNEL__, __KERNEL_SETTINGS__, __NETFLIX__) {
    var cDetectNewSoftware = (function (_super) {
        __extends(cDetectNewSoftware, _super);
        function cDetectNewSoftware(aParentObject) {
            _super.call(this);
            this._parent = null;
            this._waiting = null;
            this._ssuDialog = null;
            this._copyWaiting = null;
            this._parent = aParentObject;
        }
        cDetectNewSoftware.prototype.showWaitingProgress = function () {
            var _this = this;
            if (this._waiting === null) {
                this._waiting = hx.msg('progress', {
                    'text': hx.l('LOC_CHECKING_NEW_SOFTWARE_UPDATE_MSG_ID'),
                    'auto_close': false,
                    'dia_class': '',
                    'callback_fn': function (status) {
                        hx.log("CPgSystem", hx.util.format('++++++ ######## showWaitingProgress : status {0} ######## ++++++', status));
                        if (status === 'key_back') {
                            _this.closeWaitingProgress();
                            _this.cancel();
                        }
                    }
                });
            }
            return this._waiting;
        };
        cDetectNewSoftware.prototype.closeWaitingProgress = function () {
            if (this._waiting) {
                hx.msg.close('progress', this._waiting);
                this._waiting = null;
            }
        };
        cDetectNewSoftware.prototype.closeUsbCopyWaitingProgress = function () {
            if (this._copyWaiting) {
                hx.msg.close('progress', this._copyWaiting);
                this._copyWaiting = null;
            }
        };
        cDetectNewSoftware.prototype.getNewSoftwareWithType = function (aParamOtaType) {
            hx.svc.AV_CTRL.stopCurChannel({
                'stopAV': 1,
                'stopTsr': 1
            });
            hx.svc.SW_UPDATE.startDetectWithType({
                'otaType': aParamOtaType
            });
        };
        cDetectNewSoftware.prototype.cancel = function () {
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
        };
        cDetectNewSoftware.prototype.showSuccessMessage = function (aCurrentVersion, aNewVersion, aCallbck) {
            var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID');
            this._ssuDialog = hx.msg('ssuDialog', {
                title: hx.l('LOC_SOFTWAREUPDATE_ID'),
                text: hx.l('LOC_NEWSWDETECTED_ID'),
                text1: hx.l('LOC_DOYOUWANTUPDATE_ID'),
                first_line_arr: [hx.l('LOC_CURSWVERSION_ID'), aCurrentVersion],
                second_line_arr: [hx.l('LOC_NEWSWVERSION_ID'), aNewVersion],
                auto_close: true,
                timeout: 30000,
                btn_title_arr: [yes, no],
                dia_class: 'osd',
                callback_fn: aCallbck
            });
            this.closeWaitingProgress();
            return this._ssuDialog;
        };
        cDetectNewSoftware.prototype.showSuccessUSBMessage = function (aSuccessCB) {
            var _this = this;
            var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID');
            var selectMsg = hx.msg('ssuDialog', {
                title: hx.l('LOC_SOFTWAREUPDATE_ID'),
                text: hx.l('LOC_USB_NEWSWDETECTED_ID'),
                text1: hx.l('LOC_DOYOUWANTUPDATE_NEW_ID'),
                auto_close: true,
                timeout: 30000,
                btn_title_arr: [yes, no],
                dia_class: 'osd',
                callback_fn: function (_status) {
                    if (_status === yes) {
                        _this._copyWaiting = hx.msg('progress', {
                            text: hx.l('LOC_COPYING_USB_SOFTWARE_ID'),
                            auto_close: false,
                            dia_class: '',
                            callback_fn: function (status) {
                                if (status === 'key_back') {
                                    hx.msg.close('progress', _this._copyWaiting);
                                    _this._copyWaiting = null;
                                    _this.cancel();
                                }
                            }
                        });
                        if (aSuccessCB) {
                            aSuccessCB(_this._copyWaiting);
                        }
                    }
                    else if (_status === no || _status === 'key_back' || _status === 'key_esc' || _status === 'auto_close') {
                        _this.cancel();
                        hx.msg.close('ssuDialog', selectMsg);
                    }
                }
            });
            this.closeWaitingProgress();
            return selectMsg;
        };
        cDetectNewSoftware.prototype.showFailMessage = function () {
            hx.msg('response', {
                text: hx.l('LOC_NEW_SOFTWARE_NOT_DETECTED_ID'),
                auto_close: true,
                close_time: 3000
            });
            this.closeWaitingProgress();
            this.cancel();
        };
        cDetectNewSoftware.prototype._changeToLastChannel = function () {
            // tune to available channel
            var result = hx.svc.AV_CTRL.lastChannel();
            if (result === false) {
                hx.svc.AV_CTRL.availableChannel();
            }
        };
        return cDetectNewSoftware;
    })(__PAGE_KERNEL__);
    var cSoftwareDownload = (function () {
        function cSoftwareDownload(aTargetObject, aSource) {
            this._parent = null;
            this._enumSource = hx.svc.SW_UPDATE.TSWUSource;
            this._SUCCESSED = 1;
            this._FAIL = 0;
            this._create(aTargetObject, aSource);
        }
        cSoftwareDownload.prototype._create = function (targetObject, source) {
            this._parent = targetObject;
            this._bStandbyOta = this._parent._bStandbyOtaInstallInProgress || false;
            if ((source === this._enumSource.ESourceRf || source === this._enumSource.ESourceIp || this._bStandbyOta)) {
                this.$ = $('<article>', {
                    'class': 'wrap alignC colum2'
                });
                this._$title = $('<h2>', {
                    'class': 'infoTitle',
                    text: hx.l('LOC_DOWNLOAD_PROGRESS_ID')
                });
                this._$info = $('<p>', {
                    'class': 'infoTxt',
                    html: hx.l('LOC_DOWNLOAD_IS_BEING_DOWNLOAD_ID') + '<br><em>' + hx.l('LOC_DO_NOT_TURN_OFF_ID') + '</em>'
                });
                this._progress = hx.al.component('searchProgress', {
                    tag: 'dl',
                    data: [' ', '0%', 'width:0%'],
                    attr: {
                        'class': 'progress type3 infoProgress'
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
                    otaSpec: enumMultiOtaSpec.ESPEC_HUMAX,
                    otaStep: enumOtaStep.EStepInstall
                });
            }
            else {
                enumMultiOtaSpec = hx.svc.SW_UPDATE.TSWUSpec;
                var ulOtaSpec = enumMultiOtaSpec.ESPEC_NONE;
                if (hx.config.useOTA === 'DVBSSU') {
                    ulOtaSpec = enumMultiOtaSpec.ESPEC_SATDVBSSU;
                }
                else if (hx.config.useOTA === 'ASTRA') {
                    ulOtaSpec = enumMultiOtaSpec.ESPEC_ASTRA;
                }
                else {
                    ulOtaSpec = enumMultiOtaSpec.ESPEC_IRDETO;
                }
                hx.svc.SW_UPDATE.startDownload({
                    'otaSpec': ulOtaSpec
                });
            }
        };
        cSoftwareDownload.prototype.destroy = function () {
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
        };
        cSoftwareDownload.prototype.stopDownload = function () {
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            hx.svc.SW_UPDATE.cancelDownload();
            dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
            if ((!hx.svc.PLAYBACK.isMpbOnGoing()) && (!hx.svc.TSR.isTsrOngoing()) && (!hx.svc.AV_CTRL.isMhegIcsOnGoing())) {
                this._changeToLastChannel();
            }
            this.destroy();
        };
        cSoftwareDownload.prototype.updateEvent = function (aParam) {
            var cmptProgress;
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            var SwUpdateEventInfo = aParam.SwUpdateEventInfo;
            var eventType = SwUpdateEventInfo.getInfo('event');
            var progress = SwUpdateEventInfo.getInfo('progress');
            var progressMax = SwUpdateEventInfo.getInfo('progress_max');
            var enumSWUEvent = hx.svc.SW_UPDATE.TSWUEvent;
            if ((eventType === enumSWUEvent.EEventDownloadFail) || (eventType === enumSWUEvent.EEventInstallFail)) {
                this.stopDownload();
            }
            else if ((eventType === enumSWUEvent.EEventDownloadProgress) || (eventType === enumSWUEvent.EEventInstallProgress)) {
                if (this._parent._otaSource === this._enumSource.ESourceRf || this._parent._otaSource === this._enumSource.ESourceIp || (this._bStandbyOta === true)) {
                    cmptProgress = this._progress;
                    cmptProgress.$.find('dd').eq(0).html(progress + '%');
                    cmptProgress.$.find('dd').eq(1).find('span').css('width', progress + '%');
                }
                hx.log("CPgSystem", "[updateEvent] progress : " + progress + ' / ' + progressMax);
                if (progress === progressMax) {
                    this._parent._state = 'complete';
                    dsrvFrontPanel.clearFrontPanelText();
                    setTimeout(function () {
                        hx.svc.SW_UPDATE.restartSystem();
                    }, 1000 * 2);
                }
                hx.svc.SETTING_UTIL.setFrontPanelString({
                    'text': hx.util.format('D{0}{1}{2}%', parseInt(progress) / 100, parseInt(progress) % 100 / 10, parseInt(progress) % 10)
                });
            }
            else {
                hx.log("CPgSystem", "[updateEvent]Ignore Other messages: " + eventType);
                dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
            }
        };
        cSoftwareDownload.prototype._changeToLastChannel = function () {
            // tune to available channel
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            var result = hx.svc.AV_CTRL.lastChannel();
            if (result === false) {
                hx.svc.AV_CTRL.availableChannel();
            }
            dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
        };
        return cSoftwareDownload;
    })();
    var cPgSystem = (function (_super) {
        __extends(cPgSystem, _super);
        function cPgSystem() {
            _super.call(this);
            this.aniConfig = {
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
                }
            };
            this._state = 'normal';
            this._FAIL = 0;
            this._otaSource = null;
            this._softwareDetect = null;
            this._softwareDownload = null;
            this._ssuDialog = null;
            this._usbOTAConfirmMsg = null;
            this._usbCopyWaiting = null;
            this._isChangePageInSetting = false;
            this._checkedPB = false; // to check playback status, only one time
            this._ulRestPwdKeySeq = 0;
            this._ulhiddenKeySeq = 0;
            this.TSystemHiddenAction = {
                ACTION_HIDDEN_NONE: 0,
                ACTION_HIDDEN_ALL: 1,
                ACTION_HIDDEN_PWRESET: 2
            };
            this._infoItems = [];
            this._pageUrl = null;
            this._enumSource = hx.svc.SW_UPDATE.TSWUSource;
        }
        cPgSystem.prototype.create = function (aEntity, aPageUrl) {
            var _this = this;
            _super.prototype.create.call(this);
            var i = 0;
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            this._pageUrl = aPageUrl;
            //this._TITLE = hx.l(hx.config.settings.systeminfo.title);
            this._TITLE = hx.l(this.productData.system_settings.system_information.title);
            this._FRONT_TITLE = this.productData.system_settings.system_information.title;
            for (i = 0; i < this.productData.system_settings.system_information.itemId.length; i += 1) {
                if (this.productData.system_settings.system_information.itemId[i] === 'macaddlen') {
                    this._infoItems[i] = hx.l(this.productData.system_settings.system_information.itemStr[i]) + ' (' + hx.l('LOC_LAN_ID') + ')';
                }
                else if (this.productData.system_settings.system_information.itemId[i] === 'macaddwlen') {
                    this._infoItems[i] = hx.l(this.productData.system_settings.system_information.itemStr[i]) + ' (' + hx.l('LOC_WLAN_ID') + ')';
                }
                else if (this.productData.system_settings.system_information.itemId[i] === 'macaddbluetooth') {
                    this._infoItems[i] = hx.l(this.productData.system_settings.system_information.itemStr[i]) + ' (' + hx.l('LOC_1_BLUETOOTH_ID') + ')';
                }
                else {
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
                'class': 'wrap'
            });
            this._systemInformation = hx.al.CPgSystem.Menu(this._infoItems);
            this._newSoftwareBtn = hx.al.CPgSystem.OTAButton('LOC_CHECKNEWSOFT_ID', 'newSoftware');
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = this._TITLE;
            }
            this._draw();
            hx.al.compose(this);
            $title.ani({
                style: this.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            this._$menuWrap.ani({
                style: this.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                },
                cb: function () {
                    _this.append(_this._newSoftwareBtn);
                    _this._newSoftwareBtn.on();
                    _this._isChangePageInSetting = false;
                    // to detect new software
                    _this._softwareDetect = new cDetectNewSoftware(_this);
                    // auto start downloading
                    if (!!hx.config.useWakeUpOTA || !!hx.config.useLiveIPOTA) {
                        if (!!_this._pageUrl && _this._pageUrl.param === "startBootupHumaxOta") {
                            _this._bBootupOtaInProgress = true;
                            _this._getNewSoftware();
                        }
                    }
                }
            });
            this._checkedPB = false;
            // Have you checked the playback status?
        };
        cPgSystem.prototype._removeEventListeners = function () {
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            hx.svc.SW_UPDATE.removeEventListener(this);
            hx.svc.STORAGE_MASS.clearEventCb(this);
            dsrvFrontPanel.clearFrontPanelText();
            var dsrvRecordings = hx.svc.RECORDINGS;
            dsrvRecordings.clearEventCb(this, 'ScheduledWillBeStart');
        };
        cPgSystem.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
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
            }
            else {
                this._isChangePageInSetting = false;
            }
            dsrvFrontPanel.clearFrontPanelText();
            this._$menuWrap = undefined;
            this._systemInformation = undefined;
            this._newSoftwareBtn = undefined;
            this._settingDlg = undefined;
            this._updateCurrentOTATasking(false);
        };
        cPgSystem.prototype._init = function () {
            var _this = this;
            if (hx.config.useHiddenFunction) {
                this._ulRestPwdKeySeq = 0;
                this._ulhiddenKeySeq = 0;
            }
            hx.svc.SW_UPDATE.addEventListener({
                'event': 'UpdateEvent',
                'objTarget': this
            });
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageDetached', this, function (aPhysicalStorage) {
                _this._on_PhysicalStorageDetached(aPhysicalStorage);
            });
            this._bindCurrentRecordingEvent();
        };
        cPgSystem.prototype._draw = function () {
            $('#set').append(this._$menuWrap.append(this._systemInformation.$).append(this._newSoftwareBtn.$));
            var datas = [], i, l;
            datas = this._getItemData();
            for (i = 0, l = datas.length; i < l; i += 1) {
                if (this._systemInformation.getDtItem(i)[0].innerHTML === "ESN") {
                    this._systemInformation.getDtItem(i)[0].style.width = 130;
                    this._systemInformation.getItem(i)[0].style.width = 582;
                }
                this._systemInformation.getItem(i).html(datas[i]);
            }
        };
        cPgSystem.prototype._hide = function () {
            this._$menuWrap[0].style.display = 'none';
        };
        cPgSystem.prototype._show = function () {
            this._$menuWrap[0].style.display = 'block';
        };
        cPgSystem.prototype._on_NotiSignalInfo = function (aParam) {
            var i;
            //not consider 2 conn
            var strength = '0';
            var quality = '0';
            if (aParam.signal) {
                for (i = 0; i < aParam.signal.length; i += 1) {
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
                type: 'signal',
                strength: parseInt(strength) * 0.8,
                quality: parseInt(quality) * 0.8
            });
        };
        cPgSystem.prototype._on_PhysicalStorageDetached = function (aPhysicalStorage) {
            var storageKind = aPhysicalStorage.kind;
            hx.log("CPgSystem", "========= [system_information][_on_PhysicalStorageDetached] =========");
            hx.log("CPgSystem", hx.util.format('otaWorking : {0}, otaSource : {1}, state : {2}', this._isOtaWorking(), this._otaSource, this._state));
        };
        cPgSystem.prototype._on_UpdateEvent = function (aParam) {
            var _this = this;
            var SwUpdateEventInfo = aParam.SwUpdateEventInfo;
            var detectSW = this._softwareDetect;
            var yes = hx.l('LOC_YES_ID'), no = hx.l('LOC_NO_ID');
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
                        return;
                    }
                }
            }
            var callback_fn = function (aStatus) {
                switch (aStatus) {
                    case 'key_back':
                    case 'key_power':
                    case 'key_esc':
                    case 'auto_close':
                    case no:
                        _this._cancelDetect();
                        break;
                    case yes:
                        _this._hide();
                        _this._startDownload(source);
                        break;
                }
            };
            if (eventType === enumSWUEvent.EEventDetectSuccess) {
                hx.log("CPgSystem", 'Event Detect Success!!!');
                this._state = 'showDialog';
                this._otaSource = source;
                var curver = SwUpdateEventInfo.getInfo('current_version');
                var foundver = SwUpdateEventInfo.getInfo('found_version');
                if (source === this._enumSource.ESourceRf || source === this._enumSource.ESourceIp) {
                    hx.log("CPgSystem", 'source : ' + source);
                    this._ssuDialog = detectSW.showSuccessMessage(curver, foundver, callback_fn);
                }
                else if (source === this._enumSource.ESourceUsb) {
                    this._usbOTAConfirmMsg = detectSW.showSuccessUSBMessage(function (waitingHandle) {
                        _this._usbCopyWaiting = waitingHandle;
                        _this._startDownload(source);
                    });
                }
            }
            else if (eventType === enumSWUEvent.EEventDetectFail) {
                hx.log("CPgSystem", 'Event Detect Fail!!!');
                this._state = 'normal';
                this._updateCurrentOTATasking(false);
                this._closeDetectWaitingProgress();
                detectSW.showFailMessage();
                hx.log("CPgSystem", hx.util.format('ssuDialog handle : {0}, usbOTAConfirmMsg handle : {1}', this._ssuDialog, this._usbOTAConfirmMsg));
            }
            else if (this._softwareDownload) {
                var progress = SwUpdateEventInfo.getInfo('progress');
                var progressMax = SwUpdateEventInfo.getInfo('progress_max');
                if ((eventType === enumSWUEvent.EEventDownloadFail) || (eventType === enumSWUEvent.EEventInstallFail)) {
                    var text = hx.l('LOC_DOWNLOAD_FAIL_ID');
                    var ok_btn = hx.l('LOC_OK_ID');
                    var confirmMsg = hx.msg('confirm', {
                        'text': text,
                        'auto_close': false,
                        'btn_title_arr': [ok_btn],
                        'callback_fn': function (_status) {
                            // hx.log("CPgSystem", hx.util.format('[_on_UpdateProgress] _status : {0}', _status));
                            if (_status === ok_btn || _status === 'key_back' || _status === 'key_esc') {
                                _this._state = 'normal';
                                _this._show();
                                _this._newSoftwareBtn.on();
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
        };
        cPgSystem.prototype._on_alClicked = function (aPAram) {
            var bConsumed = false, func;
            function clickedFunc(aName) {
                return {
                    'newSoftware': 'getNewSoftware',
                    'download': 'stopDownload'
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
        };
        cPgSystem.prototype._on_alExit = function (aParam) {
            var _this = this;
            var bConsumed = false;
            var dsrvFrontPanel = hx.svc.FRONT_PANEL;
            hx.log("CPgSystem", hx.util.format('++++++ ######## _on_alExit : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
            if (this._state === 'downloading') {
                bConsumed = true;
                this._stopDownload();
                dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
            }
            else {
                if (aParam.alData) {
                    bConsumed = true;
                    this._isChangePageInSetting = true;
                    hx.al.decompose(this);
                    dsrvFrontPanel.setFrontPanelText('-SETTINGS-');
                    this._$menuWrap.ani({
                        style: this.aniConfig.menu.changeOut3,
                        trans: {
                            dur: 200,
                            delay: 100
                        },
                        cb: function () {
                            _this.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgSettings',
                                me: _this
                            });
                        }
                    });
                }
            }
            return bConsumed;
        };
        cPgSystem.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
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
                case hx.key.KEY_POWER:
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
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: this
                        });
                    }
                    break;
                default:
                    if (hx.config.useHiddenFunction) {
                        ulHiddenAction = this._checkHiddenMenuKeySeq(aParam.alKey);
                        if (ulHiddenAction !== this.TSystemHiddenAction.ACTION_HIDDEN_NONE) {
                            this._isChangePageInSetting = true;
                            this.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgHiddenSetting?' + ulHiddenAction,
                                me: this
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cPgSystem.prototype._check_ScheduledItems_WillbeStarted = function () {
            var dsrvRecordings, scheduleList;
            var i, ti, item, sooner, nowTime, currentTime, hasItem = false;
            dsrvRecordings = hx.svc.RECORDINGS;
            scheduleList = dsrvRecordings.getScheduledList();
            if (scheduleList.length) {
                sooner = scheduleList[0];
                for (i = 1, ti = scheduleList.length; i < ti; i += 1) {
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
        };
        cPgSystem.prototype._getNewSoftware = function () {
            var _this = this;
            var currentRecordings, length;
            var bPbGoing = false;
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
                setTimeout(function () {
                    _this._getNewSoftware();
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
                for (i = 0; i < scheduleList.length; i += 1) {
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
                    'text': message,
                    'auto_close': true,
                    'timeout': 30000,
                    'btn_title_arr': [yes, later],
                    'btn_last_focus': 1,
                    'dia_class': 'osd',
                    'callback_fn': function (_status) {
                        hx.log("CPgSystem", hx.util.format('[confirm][_getNewSoftware] status', _status));
                        if (_status === yes) {
                            _this._showDetectWaitingProgress();
                            var cb = function () {
                                hx.log("CPgSystem", '------> GO! _tryGetNewSoftwareDetect');
                                _this._tryGetNewSoftwareDetect();
                            };
                            if (liveStreamingItem) {
                                dsrvRecordings.removeScheduledRecording({
                                    'items': [liveStreamingItem],
                                    'finished': function (result) {
                                        if (result) {
                                            cb();
                                        }
                                    }
                                });
                            }
                            else {
                                _this._stopCurrentRecordings(cb);
                            }
                        }
                        else if (_status === later || _status === 'key_back' || _status === 'key_esc' || _status === 'auto_close') {
                            hx.msg.close('confirm', selectMsg);
                            _this._closeDetectWaitingProgress();
                        }
                    }
                });
            }
            else {
                this._showDetectWaitingProgress();
                //show Progress : 'Searching for new software...'
                var scheduleTimerId = null, has_HotScheduledItem;
                has_HotScheduledItem = this._check_ScheduledItems_WillbeStarted();
                if (has_HotScheduledItem) {
                    scheduleTimerId = setTimeout(function () {
                        clearTimeout(scheduleTimerId);
                        scheduleTimerId = null;
                        _this._getNewSoftware();
                    }, 30000);
                }
                if (scheduleTimerId === null) {
                    this._tryGetNewSoftwareDetect();
                }
            }
        };
        cPgSystem.prototype._stopCurrentRecordings = function (aCallback) {
            hx.log("CPgSystem", hx.util.format('++++++ ######## _stopCurrentRecordings : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
            var dsrvRecordings = hx.svc.RECORDINGS;
            var currentRecordingList = dsrvRecordings.getCurrentRecordings();
            if (currentRecordingList && currentRecordingList.length) {
                dsrvRecordings.stopRecord({
                    'items': currentRecordingList,
                    'finished': function (result) {
                        hx.log("CPgSystem", hx.util.format('Recordings STOP OK! . result - {0}', result));
                        if (result) {
                            aCallback();
                        }
                    }
                });
            }
        };
        cPgSystem.prototype._doScheduledEvent = function (aParam) {
            var _this = this;
            var paramItem = aParam.item;
            var paramRes = aParam.res;
            var setResult = function (item, res) {
                var channel, bChangeable = false;
                var dsrvRecordings = hx.svc.RECORDINGS;
                dsrvRecordings.recordConfirm({
                    scheduledRecording: item,
                    type: 0,
                    check: res
                });
                if ((res === 1) && item.isReminder()) {
                    hx.log('CPgSystem', '[ScheduleEvent] reminder result : yes');
                    channel = item.getChannelInstance();
                    //if (this.getCenterPageName() === 'CPgTvApps') {}
                    ///////////////////////////////
                    // check tunable
                    bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                        channel: channel
                    });
                    if (bChangeable === false) {
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
                    ///////////////////////////////
                    // set last channel
                    var o = hx.svc.CH_LIST.getAvailableGroupForChannel({
                        'channel': channel
                    });
                    if (o.channel !== null && o.group !== null) {
                        hx.svc.WEB_STORAGE.updateLastService({
                            'ccid': o.channel.ccid,
                            'group': o.group
                        });
                    }
                    ///////////////////////////////
                    // exit page
                    hx.log('CPgSystem', '[ScheduleEvent] reminder result : page change to liveController');
                    // the following statement doesn't work, is not needed since it is done just after OTA stopped.
                    _this.sendEventToEntity({
                        'alEvent': 'alChangePage',
                        'target': 'CPgLiveController',
                        'me': _this
                    });
                    ///////////////////////////////
                    // channel change
                    if (o.channel !== null && o.group !== null) {
                        var fnChChange = function () {
                            hx.log('CPgSystem', '[ScheduleEvent] reminder result : channel change!!!!');
                            var VBcurrentChannel = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
                            if (VBcurrentChannel) {
                                if (VBcurrentChannel.ccid === o.channel.ccid) {
                                    hx.log('CPgSystem', '[ScheduleEvent] reminder result : return!! same channel!!!');
                                    return;
                                }
                            }
                            hx.svc.AV_CTRL.channelChange({
                                channel: o.channel,
                                group: o.group
                            });
                        };
                        setTimeout(fnChChange, 0);
                    }
                }
                else if ((res === 1) && !item.isReminder()) {
                    var chInfo;
                    channel = item.getChannelInstance();
                    chInfo = hx.svc.CH_LIST.getAvailableGroupForChannel({
                        'channel': channel
                    });
                    if ((chInfo.channel !== null) && (chInfo.chInfo !== null)) {
                        hx.svc.AV_CTRL.channelChange({
                            channel: chInfo.channel,
                            group: chInfo.group
                        });
                    }
                }
            };
            setResult(paramItem, paramRes);
        };
        cPgSystem.prototype._willTheRecordingStartNowQuestion2 = function (aItem) {
            var _this = this;
            var yes = hx.l('LOC_YES_ID'), later = hx.l('LOC_LATER_ID'), message = hx.l('LOC_OTA_RECORDING_WILL_START_ID');
            var dsrvRecordings = hx.svc.RECORDINGS;
            hx.log("CPgSystem", hx.util.format('++++++ ######## _willTheRecordingStartNowQuestion : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
            if (this._state === 'downloading') {
                if (aItem.isReminder()) {
                    hx.log("CPgSystem", "++++++ ######## The scheduled Reminder is canceld, becasue STB is downloading SW  ######## ++++++");
                    dsrvRecordings.removeScheduledRecording({
                        items: [aItem]
                    });
                }
                else {
                    hx.log("CPgSystem", "++++++ ######## Process Scheduled Recording Event during OTA  ######## ++++++");
                    var selectMsg = hx.msg('confirm', {
                        text: message,
                        auto_close: true,
                        timeout: 20000,
                        btn_title_arr: [yes, later],
                        btn_last_focus: 1,
                        dia_class: 'osd',
                        callback_fn: function (_status) {
                            if (_status === yes) {
                                hx.msg.close('confirm', selectMsg);
                                dsrvRecordings.removeScheduledRecording({
                                    items: [aItem]
                                });
                            }
                            else if (_status === later || _status === 'key_back' || _status === 'key_esc' || _status === 'auto_close') {
                                hx.msg.close('confirm', selectMsg);
                                _this._stopDownload();
                            }
                        }
                    });
                }
            }
        };
        cPgSystem.prototype._getCurrentRec = function () {
            var dsrvRecordings, currentRecordings;
            dsrvRecordings = hx.svc.RECORDINGS;
            currentRecordings = hx.svc.RECORDINGS.getCurrentRecordings();
            return currentRecordings;
        };
        cPgSystem.prototype._showDetectWaitingProgress = function () {
            hx.log("CPgSystem", hx.util.format('++++++ ######## _showDetectWaitingProgress : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
            if (this._softwareDetect) {
                this._softwareDetect.showWaitingProgress();
            }
        };
        cPgSystem.prototype._closeDetectWaitingProgress = function () {
            hx.log("CPgSystem", hx.util.format('++++++ ######## _closeDetectWaitingProgress : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
            if (this._softwareDetect) {
                this._softwareDetect.closeWaitingProgress();
            }
        };
        cPgSystem.prototype._tryGetNewSoftwareDetect = function () {
            var enumMultiOtaSpec = hx.svc.SW_UPDATE.TSWUSpec;
            var ulOtaSpec = enumMultiOtaSpec.ESPEC_NONE;
            hx.log("CPgSystem", hx.util.format('++++++ ######## _tryGetNewSoftwareDetect : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
            if (hx.config.useOTA === 'DVBSSU') {
                ulOtaSpec = enumMultiOtaSpec.ESPEC_SATDVBSSU;
            }
            else if (hx.config.useOTA === 'ASTRA') {
                ulOtaSpec = enumMultiOtaSpec.ESPEC_ASTRA;
            }
            else {
                ulOtaSpec = enumMultiOtaSpec.ESPEC_IRDETO;
            }
            if (this._softwareDetect) {
                this._state = 'swdetect';
                this._softwareDetect.getNewSoftwareWithType(ulOtaSpec);
                this._updateCurrentOTATasking(true);
            }
        };
        cPgSystem.prototype._cancelDetect = function () {
            hx.log("CPgSystem", hx.util.format('++++++ ######## _cancelDetect : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
            this._state = 'normal';
            this._softwareDetect.cancel();
            this._newSoftwareBtn.on();
            this._updateCurrentOTATasking(false);
        };
        cPgSystem.prototype._startDownload = function (aSource) {
            var _this = this;
            var currentRecordings, length;
            hx.log("CPgSystem", hx.util.format('++++++ ######## _startDownload : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
            currentRecordings = this._getCurrentRec();
            length = currentRecordings.length;
            if (length) {
                var yes = hx.l('LOC_YES_ID'), later = hx.l('LOC_LATER_ID'), message = hx.l('LOC_OTA_RECORDING_IS_IN_PROCESS_ID');
                var selectMsg = hx.msg('confirm', {
                    'text': message,
                    'auto_close': true,
                    'timeout': 30000,
                    'btn_title_arr': [yes, later],
                    'btn_last_focus': 1,
                    'dia_class': 'osd',
                    'callback_fn': function (_status) {
                        hx.log("CPgSystem", hx.util.format('[startDownload] _status : {0}, source : {1}', _status, aSource));
                        if (_status === yes) {
                            _this._showDetectWaitingProgress();
                            _this._stopCurrentRecordings(function () {
                                _this._startDownload(aSource);
                            });
                        }
                        else if (_status === later || _status === 'key_back' || _status === 'key_esc' || _status === 'auto_close') {
                            hx.msg.close('confirm', selectMsg);
                            _this._show();
                            _this._newSoftwareBtn.on();
                            _this._updateCurrentOTATasking(false);
                        }
                    }
                });
            }
            else {
                var has_HotScheduledItem, scheduleTimerId = null;
                has_HotScheduledItem = this._check_ScheduledItems_WillbeStarted();
                if (has_HotScheduledItem) {
                    scheduleTimerId = setTimeout(function () {
                        clearTimeout(scheduleTimerId);
                        scheduleTimerId = null;
                        _this._startDownload(aSource);
                    }, 30000);
                }
                else {
                    this._softwareDownload = new cSoftwareDownload(this, aSource);
                    this._state = 'downloading';
                }
            }
        };
        cPgSystem.prototype._stopDownload = function () {
            hx.log("CPgSystem", hx.util.format('++++++ ######## _stopDownload : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
            this._state = 'normal';
            this._otaSource = null;
            if (this._softwareDownload) {
                this._softwareDownload.stopDownload();
            }
            this._show();
            this._newSoftwareBtn.on();
            this._updateCurrentOTATasking(false);
        };
        cPgSystem.prototype._bindCurrentRecordingEvent = function () {
            var _this = this;
            hx.log("CPgSystem", hx.util.format('++++++ ######## _bindCurrentRecordingEvent : isOtaWorking {0}, state : {1} ######## ++++++', this._isOtaWorking(), this._state));
            var dsrvRecordings = hx.svc.RECORDINGS;
            dsrvRecordings.addEventCb('ScheduledWillBeStart', this, function (param) {
                var item = param.item;
                hx.log("CPgSystem", hx.util.format('######## [ScheduledWillBeStart] state : {0} ########', _this._state));
                if (_this._state !== 'downloading') {
                    return;
                }
                if (item.isRecordingOnGoing()) {
                    _this._willTheRecordingStartNowQuestion2(item);
                }
            });
        };
        cPgSystem.prototype._updateCurrentOTATasking = function (currentState) {
            hx.il.criticalTasking.crOTATasking = currentState;
            if (currentState) {
                // DMR uri set null ('bring in client' do not have to work)
                hx.svc.DMR.setMedia(null);
            }
        };
        cPgSystem.prototype._isOtaWorking = function () {
            return hx.il.criticalTasking.crOTATasking;
        };
        cPgSystem.prototype._getItemData = function () {
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
        };
        cPgSystem.prototype._checkHiddenMenuKeySeq = function (aKey) {
            var ulActionHiddenType = 0, bEnable = false;
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
        };
        cPgSystem.prototype._checkHiddenMenuPasswordReset = function (aKey) {
            switch (aKey) {
                case hx.key.KEY_7:
                    if (this._ulRestPwdKeySeq === 0) {
                        this._ulRestPwdKeySeq += 1;
                    }
                    else {
                        this._ulRestPwdKeySeq = 0;
                    }
                    break;
                case hx.key.KEY_4:
                    if (this._ulRestPwdKeySeq === 1) {
                        this._ulRestPwdKeySeq += 1;
                    }
                    else {
                        this._ulRestPwdKeySeq = 0;
                    }
                    break;
                case hx.key.KEY_1:
                    if (this._ulRestPwdKeySeq === 2) {
                        this._ulRestPwdKeySeq += 1;
                    }
                    else {
                        this._ulRestPwdKeySeq = 0;
                    }
                    break;
                case hx.key.KEY_2:
                    if (this._ulRestPwdKeySeq === 3) {
                        this._ulRestPwdKeySeq += 1;
                    }
                    else {
                        this._ulRestPwdKeySeq = 0;
                    }
                    break;
                case hx.key.KEY_3:
                    if (this._ulRestPwdKeySeq === 4) {
                        this._ulRestPwdKeySeq += 1;
                    }
                    else {
                        this._ulRestPwdKeySeq = 0;
                    }
                    break;
                case hx.key.KEY_5:
                    if (this._ulRestPwdKeySeq === 5) {
                        this._ulRestPwdKeySeq += 1;
                    }
                    else {
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
        };
        cPgSystem.prototype._checkHiddenMenu = function (aKey) {
            switch (aKey) {
                case hx.key.KEY_RED:
                    if (this._ulhiddenKeySeq === 0) {
                        this._ulhiddenKeySeq += 1;
                    }
                    else {
                        this._ulhiddenKeySeq = 0;
                    }
                    break;
                case hx.key.KEY_GREEN:
                    if ((this._ulhiddenKeySeq === 1) || (this._ulhiddenKeySeq === 4)) {
                        this._ulhiddenKeySeq += 1;
                    }
                    else {
                        this._ulhiddenKeySeq = 0;
                    }
                    break;
                case hx.key.KEY_YELLOW:
                    if ((this._ulhiddenKeySeq === 2) || (this._ulhiddenKeySeq === 5)) {
                        this._ulhiddenKeySeq += 1;
                    }
                    else {
                        this._ulhiddenKeySeq = 0;
                    }
                    break;
                case hx.key.KEY_BLUE:
                    if (this._ulhiddenKeySeq === 3) {
                        this._ulhiddenKeySeq += 1;
                    }
                    else if (this._ulhiddenKeySeq === 6) {
                        return true;
                    }
                    else {
                        this._ulhiddenKeySeq = 0;
                    }
                    break;
                default:
                    this._ulhiddenKeySeq = 0;
                    break;
            }
            return false;
        };
        return cPgSystem;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgSystem = cPgSystem;
    hx.al.CPgSystem.Menu = function (items) {
        return hx.al.component('table', {
            attr: {
                'class': 'info box'
            },
            langID: false,
            items: items,
            describe: hx.config.settings.systeminfo.describe,
            count: hx.config.settings.systeminfo.describe.length
        });
    };
    hx.al.CPgSystem.OTAButton = function (langID, name) {
        return hx.al.component('button', {
            attr: {
                'class': 'button on',
                'data-langID': langID
            },
            usePointerKey: true,
            notSelfDestroy: true,
            name: name
        });
    };
    return cPgSystem;
});
