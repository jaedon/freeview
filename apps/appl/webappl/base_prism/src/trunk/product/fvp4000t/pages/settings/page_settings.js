var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    ///<reference path="../../../../def/sugar.d.ts" />
    /** settings main menu initializer */
    var CPgSettings = (function (_super) {
        __extends(CPgSettings, _super);
        function CPgSettings(aPageName) {
            _super.call(this, aPageName);
            this.styleMap = {
                fadeIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
                    }
                },
                fadeOut: {
                    start: {
                        'opacity': 1
                    },
                    end: {
                        'opacity': 0
                    }
                },
                menu: {
                    changeOut: {
                        start: {
                            'opacity': 1,
                            'left': 0
                        },
                        end: {
                            'opacity': 0,
                            'left': '-10px'
                        }
                    },
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
                    changeOut2: {
                        start: {
                            'opacity': 1,
                            'left': 0
                        },
                        end: {
                            'opacity': 0,
                            'left': '10px'
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
                button: {
                    changeIn: {
                        start: {
                            'opacity': 0,
                            'left': '406px'
                        },
                        end: {
                            'opacity': 1,
                            'left': '386px'
                        }
                    },
                    changeOut: {
                        start: {
                            'opacity': 1,
                            'left': '386px'
                        },
                        end: {
                            'opacity': 0,
                            'left': '406px'
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
                    },
                    fadeOut2: {
                        start: {
                            'opacity': 0.5,
                            'top': 0
                        },
                        end: {
                            'opacity': 0,
                            'top': '20px'
                        }
                    }
                },
                dep1: {
                    fadeIn: {
                        start: {
                            'opacity': 0,
                            'left': '100px'
                        },
                        end: {
                            'opacity': 1,
                            'left': '0px'
                        }
                    },
                    fadeOut: {
                        start: {
                            'opacity': 1
                        },
                        end: {
                            'opacity': 0
                        }
                    },
                    depthOut: {
                        start: {},
                        end: {}
                    },
                    depthIn: {
                        start: {},
                        end: {}
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
                    },
                    setFadeIn: {
                        start: {
                            'opacity': 0,
                            'left': '794px'
                        },
                        end: {
                            'opacity': 0.5,
                            'left': '784px'
                        }
                    },
                    setFadeOut: {
                        end: {
                            'opacity': 0
                        }
                    },
                    depthOut: {
                        start: {
                            'opacity': 1,
                            'left': '397px',
                            'width': '763px'
                        },
                        end: {
                            'opacity': 0.5,
                            'left': '784px',
                            'width': '386px'
                        }
                    },
                    depthIn: {
                        start: {
                            'opacity': 0.5,
                            'left': '784px',
                            'width': '386px'
                        },
                        end: {
                            'opacity': 1,
                            'left': '397px',
                            'width': '763px'
                        }
                    }
                },
                opt: {
                    fadeIn: {
                        start: {
                            'opacity': 0,
                            'top': '10px'
                        },
                        end: {
                            'opacity': 1,
                            'top': 0
                        }
                    },
                    fadeOut: {
                        start: {
                            'opacity': 1
                        },
                        end: {
                            'opacity': 0
                        }
                    }
                }
            };
            this.aniConfig = {
                fadeIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
                    }
                },
                fadeOut: {
                    start: {
                        'opacity': 1
                    },
                    end: {
                        'opacity': 0
                    }
                },
                menu: {
                    changeOut: {
                        start: {
                            'opacity': 1,
                            'left': 0
                        },
                        end: {
                            'opacity': 0,
                            'left': '-10px'
                        }
                    },
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
                    changeOut2: {
                        start: {
                            'opacity': 1,
                            'left': 0
                        },
                        end: {
                            'opacity': 0,
                            'left': '10px'
                        }
                    }
                },
                dep1: {
                    fadeIn: {
                        start: {
                            'opacity': 0,
                            'left': '100px'
                        },
                        end: {
                            'opacity': 1,
                            'left': '0px'
                        }
                    },
                    depthOut: {
                        start: {},
                        end: {}
                    },
                    depthIn: {
                        start: {},
                        end: {}
                    }
                },
                dep2: {
                    setFadeIn: {
                        start: {
                            'opacity': 0,
                            'left': '794px'
                        },
                        end: {
                            'opacity': 0.5,
                            'left': '784px'
                        }
                    },
                    depthOut: {
                        start: {
                            'opacity': 1,
                            'left': '397px',
                            'width': '763px'
                        },
                        end: {
                            'opacity': 0.5,
                            'left': '784px',
                            'width': '386px'
                        }
                    },
                    depthIn: {
                        start: {
                            'opacity': 0.5,
                            'left': '784px',
                            'width': '386px'
                        },
                        end: {
                            'opacity': 1,
                            'left': '397px',
                            'width': '763px'
                        }
                    }
                }
            };
            if (hx.config.voiceAssistant) {
                CPgSettings.items = {
                    itemIdfirst: ['generalsetting', 'networksetting', 'pvrsetting', 'channelsetting', 'advancedsetting', 'systemsetting'],
                    first: ['LOC_GENERALSETTINGS_ID', 'LOC_NETWORKSETTINGS_ID', 'LOC_PVRSETTINGS_ID', 'LOC_CHANNELSETTINGS_ID', 'LOC_ADVANCEDSETTINGS_ID', 'LOC_SYSTEMSETTINGS_ID'],
                    itemIdsecond: [
                        ['parentalcontrol', 'language', 'video', 'audio', 'onscreendisplay', 'privacypolicy'],
                        ['configurelan', 'configurewlan', 'networkconnectionstatus'],
                        ['recordingoption', 'playbackoption', 'storage'],
                        ['favouritechannels', 'editchlist', 'signalTest', 'automaticSearch', 'manualSearch'],
                        ['servermanagement', 'fileshare', 'recorderregistration', 'remoterecording', 'voiceassistant'],
                        ['systeminfo', 'powertimer', 'powermanagement', 'hdmicec', 'facdefault']
                    ],
                    second: [
                        ['LOC_PARENTALCONTROL_ID', 'LOC_LANGUAGE_ID', 'LOC_VIDEO_ID', 'LOC_AUDIO_ID', 'LOC_ONSCREENDISPLAY_ID', 'LOC_PRIVACY_POLICY_TERMS_AND_CONDITIONS_LOWER_ID'],
                        ['LOC_CONFIGURELAN_ID', 'LOC_CONFIGURE_WLAN_ID', 'LOC_NETWORK_CONNECTION_STATUS_ID'],
                        ['LOC_RECORDING_OPTIONS_ID', 'LOC_PLAYBACKOPTIONS_ID', 'LOC_STORAGE_ID'],
                        ['LOC_FAVOURITECHANNELS_ID', 'LOC_EDITCHLIST_ID', 'LOC_SIGNALTEST_ID', 'LOC_AUTOMATIC_CHANNEL_SEARCH_ID', 'LOC_MANUAL_CHANNEL_SEARCH_ID'],
                        ['LOC_SERVER_MANAGEMENT_ID', 'LOC_FILE_SHARE_ID', 'LOC_RECORDER_REGISTRATION_ID', 'LOC_REMOTERECORDING_ID', 'LOC_VOICE_ASSISTANT_ID'],
                        ['LOC_SYSTEMINFO_ID', 'LOC_POWERTIMER_ID', 'LOC_POWERMANAGEMENT_ID', 'LOC_HDMI_CEC_ID', 'LOC_FACDEFAULT_ID']
                    ],
                    secondLink: [
                        ['CPgParental', 'CPgLanguage', 'CPgSettingsVideo', 'CPgAudio', 'CPgScreenDisplay', 'CPgPrivacyPolicy'],
                        ['CPgConfigureLAN', 'CPgConfigureWiFi', 'CPgNetworkConnectionStatus'],
                        ['CPgRecording', 'CPgPlaybackOptions', 'CPgStorage'],
                        ['CPgFavoriteChannels', 'CPgEditChannels', 'CPgSignalTest', 'CPgAutomaticSearch', 'CPgManualSearch'],
                        ['CPgNetworkServer', 'CPgFileShare', 'CPgRecorderRegistration', 'CPgRemoteRecord', 'CPgVoiceAssistant'],
                        ['CPgSystem', 'CPgPowerTimer', 'CPgPowerManagement', 'CPgHdmiCec', 'CPgFactoryDefault']
                    ],
                };
            }
            else {
                CPgSettings.items = {
                    itemIdfirst: ['generalsetting', 'networksetting', 'pvrsetting', 'channelsetting', 'advancedsetting', 'systemsetting'],
                    first: ['LOC_GENERALSETTINGS_ID', 'LOC_NETWORKSETTINGS_ID', 'LOC_PVRSETTINGS_ID', 'LOC_CHANNELSETTINGS_ID', 'LOC_ADVANCEDSETTINGS_ID', 'LOC_SYSTEMSETTINGS_ID'],
                    itemIdsecond: [
                        ['parentalcontrol', 'language', 'video', 'audio', 'onscreendisplay', 'privacypolicy'],
                        ['configurelan', 'configurewlan', 'networkconnectionstatus'],
                        ['recordingoption', 'playbackoption', 'storage'],
                        ['favouritechannels', 'editchlist', 'signalTest', 'automaticSearch', 'manualSearch'],
                        ['servermanagement', 'fileshare', 'recorderregistration', 'remoterecording'],
                        ['systeminfo', 'powertimer', 'powermanagement', 'hdmicec', 'facdefault']
                    ],
                    second: [
                        ['LOC_PARENTALCONTROL_ID', 'LOC_LANGUAGE_ID', 'LOC_VIDEO_ID', 'LOC_AUDIO_ID', 'LOC_ONSCREENDISPLAY_ID', 'LOC_PRIVACY_POLICY_TERMS_AND_CONDITIONS_LOWER_ID'],
                        ['LOC_CONFIGURELAN_ID', 'LOC_CONFIGURE_WLAN_ID', 'LOC_NETWORK_CONNECTION_STATUS_ID'],
                        ['LOC_RECORDING_OPTIONS_ID', 'LOC_PLAYBACKOPTIONS_ID', 'LOC_STORAGE_ID'],
                        ['LOC_FAVOURITECHANNELS_ID', 'LOC_EDITCHLIST_ID', 'LOC_SIGNALTEST_ID', 'LOC_AUTOMATIC_CHANNEL_SEARCH_ID', 'LOC_MANUAL_CHANNEL_SEARCH_ID'],
                        ['LOC_SERVER_MANAGEMENT_ID', 'LOC_FILE_SHARE_ID', 'LOC_RECORDER_REGISTRATION_ID', 'LOC_REMOTERECORDING_ID'],
                        ['LOC_SYSTEMINFO_ID', 'LOC_POWERTIMER_ID', 'LOC_POWERMANAGEMENT_ID', 'LOC_HDMI_CEC_ID', 'LOC_FACDEFAULT_ID']
                    ],
                    secondLink: [
                        ['CPgParental', 'CPgLanguage', 'CPgSettingsVideo', 'CPgAudio', 'CPgScreenDisplay', 'CPgPrivacyPolicy'],
                        ['CPgConfigureLAN', 'CPgConfigureWiFi', 'CPgNetworkConnectionStatus'],
                        ['CPgRecording', 'CPgPlaybackOptions', 'CPgStorage'],
                        ['CPgFavoriteChannels', 'CPgEditChannels', 'CPgSignalTest', 'CPgAutomaticSearch', 'CPgManualSearch'],
                        ['CPgNetworkServer', 'CPgFileShare', 'CPgRecorderRegistration', 'CPgRemoteRecord'],
                        ['CPgSystem', 'CPgPowerTimer', 'CPgPowerManagement', 'CPgHdmiCec', 'CPgFactoryDefault']
                    ],
                };
            }
        }
        CPgSettings.prototype.create = function (aEntity, aPageUrl) {
            _super.prototype.create.call(this);
            this.pageUrl = aPageUrl;
            this.isShow = false;
            this.lastIdx = 0;
            this._whoIsCallVerifyPassword = '';
            //hx.logadd('settings');
            hx.log('settings', 'settings - init');
            if ($('#set .title')[0] === undefined) {
                this._index = {
                    m: 0,
                    s: -1
                };
                if (this.lastSettingMenu) {
                    this._index.m = this.lastIdx = this.lastSettingMenu;
                }
                else {
                    this._index.m = this.lastIdx = this.lastSettingMenu = 0;
                }
                this._produce();
                this._init('first', this.lastIdx, ['clock', 'setFrontPanel']);
                this.showAndHide();
            }
            else {
                this.backToMain();
            }
            // channel import export
            if (hx.config.useImportExportChannels === 1) {
                hx.svc.CHANNEL_IMPORT_EXPORT.addEventListener({
                    'event': 'ChannelImportExport',
                    'objTarget': this
                });
            }
            //terminate all apps
            var dsrvAppMgr = hx.svc.APP_MGR;
            dsrvAppMgr.destroyHbbTvDeeplinkApplication();
        };
        CPgSettings.prototype.destroy = function () {
            var that = this;
            _super.prototype.destroy.call(that);
            hx.svc.SETTING_UTIL.clearEventCb(that);
            hx.svc.STORAGE_MASS.clearEventCb(that);
            hx.svc.CLOCK.stopClock();
            that._destroyConfigService();
            if (that.$menuWrap) {
                if (that.$menuWrap.children().length) {
                    that.$menuWrap.children().remove();
                }
                if (that.$menuWrap.css("visibility") === 'visible') {
                    that.$menuWrap.remove();
                    that.$menuWrap = null;
                    if (that.$section) {
                        that.$section.remove();
                        that.$section = null;
                    }
                }
            }
            if (that.$msgWrap) {
                that.$msgWrap.remove();
                that.$msgWrap = null;
            }
            if (that._pinMsg) {
                hx.msg.close('pin', that._pinMsg);
                that._pinMsg = that;
            }
            if (that._confirmMsg) {
                hx.msg.close('confirm', that._confirmMsg);
                that._confirmMsg = null;
            }
            if (that._1stItems) {
                that._1stItems.destroy();
                that._1stItems = null;
            }
            if (that._2ndItems) {
                that._2ndItems.destroy();
                that._2ndItems = null;
            }
            hx.logremove('settings');
        };
        CPgSettings.prototype._init = function (o, i, srv) {
            var that = this //
            , cRecList //
            , recordingFileOpInfo //
            , dsrvRecordings;
            if (that.pageUrl) {
                if (that.pageUrl.param !== 'directSch') {
                    if (o === 'first') {
                        that._1stItems.$.removeClass('his');
                        that._1stItems.getItem(that._index.m).removeClass('on');
                        that._2ndItems.$.removeClass('active');
                        that._1stItems.setItemFocus(i);
                        that._1stItems.on();
                    }
                    else {
                        that._focusProcess(o, i);
                    }
                }
            }
            else {
                if (o === 'first') {
                    that._1stItems.$.removeClass('his');
                    that._1stItems.getItem(that._index.m).removeClass('on');
                    that._2ndItems.$.removeClass('active');
                    that._1stItems.setItemFocus(i);
                    that._1stItems.on();
                }
                else {
                    that._focusProcess(o, i);
                }
            }
            that._createService(srv);
            hx.svc.SETTING_UTIL.addEventCb('tryConnection', that, function (aResult) {
                that._on_tryConnection(aResult);
            });
            hx.svc.SETTING_UTIL.addEventCb('wirelessLinkStatus', that, function (aResult) {
                that._on_wirelessLinkStatus(aResult);
            });
            that._isPbGoing = hx.svc.TSR.isTsrOngoing() || hx.svc.TSR.isChaseOngoing() || hx.svc.PLAYBACK.isMpbOnGoing();
            that._isMedGoing = hx.svc.PLAYBACK.isMpbOnGoing();
            that._isPrivacyPolicyAccepted = hx.svc.TV_PORTAL.isPrivacyPolicyAccepted();
            //if isRecording, disable (Recording Options, Storage, Factory Default, Edit Channel List, Search
            // Channels)
            // Storage - Due to set menu enable or disable
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageAttached', that, function (aPhysicalStorage) {
                that._on_PhysicalStorageAttached(aPhysicalStorage);
            });
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageDetached', that, function (aPhysicalStorage) {
                that._on_PhysicalStorageDetached(aPhysicalStorage);
            });
            dsrvRecordings = hx.svc.RECORDINGS;
            cRecList = dsrvRecordings.getCurrentRecordings();
            recordingFileOpInfo = dsrvRecordings.getFileOperationInfo();
            if (recordingFileOpInfo || (cRecList && cRecList.length)) {
                that._isRecording = true;
            }
            else {
                that._isRecording = false;
            }
            /* TODO
             * find getFileOpStatus
             * bglee@humaxdigital.com, 16:47, 2014. 4. 21.
             */
            //        that._fileOpStatus = hx.dl.dsrvMedia.getFileOpStatus();
            that._fileOpStatus = hx.svc.MEDIA_ITEM_MANAGER.isFileOp();
        };
        CPgSettings.prototype._produce = function () {
            this.$bg = $('.prism-page.setting');
            this.$section = $('<section>', {
                id: 'set'
            });
            this.$hgroup = $('<hgroup>', {
                'class': 'title'
            });
            this._title = this.title();
            hx.svc.CLOCK.startClock();
            this._dateTime = this.showDateTime();
            this.$menuWrap = $('<div>', {
                'class': 'menu root'
            });
            this._1stItems = this.firstItems();
            this._2ndItems = this.secondItems(this.lastIdx);
            this.$section.css('opacity', 0);
            this.$hgroup.css('opacity', 0);
            this._1stItems.$.css('opacity', 0);
            this._2ndItems.$.css('opacity', 0);
            this.$bg.append(this.$section.append(this.$hgroup.append(this._title.$).append(this._dateTime)).append(this.$menuWrap.append(this._1stItems.$).append(this._2ndItems.$)));
            hx.al.compose(this).append(this._1stItems).append(this._2ndItems);
        };
        CPgSettings.prototype.activateMainDep2 = function (aIndex) {
            var m = this._index.m;
            this._keyOff();
            this.lastSettingMenu = m;
            this._1stItems.$.addClass('his');
            this._1stItems.getItem(m).addClass('on');
            this._2ndItems.$.addClass('active');
            this._2ndItems.setItemFocus(aIndex);
            this._2ndItems.on();
            this._disableSubMenu();
            if (this._2ndItems.getItem(aIndex).hasClass('dis')) {
                this._2ndItems.setItemFocus(this._findFocus('second', m));
            }
        };
        CPgSettings.prototype.activateMainDep1 = function (aIndex) {
            var m = this._index.m;
            this._keyOff();
            this._1stItems.$.removeClass('his');
            this._1stItems.getItem(this._index.m).removeClass('on');
            this._2ndItems.$.removeClass('active');
            this._2ndItems.$.removeAttr('style'); // to solve an issue which 2nd Items look a little more dark.
            this._1stItems.setItemFocus(aIndex);
            this._1stItems.on();
        };
        CPgSettings.prototype.setFocusToSecondItem = function (firstItem, secondItem) {
            var that = this;
            that._1stItems.getFocusedItem().removeClass('on');
            that._index.m = that._conv_itemID_to_index('first', 0, firstItem);
            that._make2ndItems(that._index.m);
            that._keyOff();
            that._1stItems.$.addClass('his');
            that._1stItems.getItem(that._index.m).addClass('on');
            that._2ndItems.$.addClass('active');
            that._index.s = that._conv_itemID_to_index('second', that._index.m, secondItem);
            that._2ndItems.setItemFocus(that._index.s);
            that._2ndItems.on();
        };
        CPgSettings.prototype.showAndHide = function () {
            var that = this;
            if (!that.isShow) {
                //currentMenu = 'dep1';
                that.$hgroup.ani({
                    style: that.aniConfig.fadeIn,
                    trans: {
                        dur: 230
                    }
                });
                that.$section.ani({
                    style: that.aniConfig.fadeIn,
                    trans: {
                        dur: 230
                    }
                });
                that._1stItems.$.ani({
                    style: that.aniConfig.dep1.fadeIn,
                    trans: {
                        dur: 250,
                        delay: 30
                    }
                });
                that._2ndItems.$.ani({
                    style: that.aniConfig.dep2.setFadeIn,
                    trans: {
                        dur: 270,
                        delay: 50
                    },
                    cb: function () {
                        if (that.pageUrl) {
                            if (that.pageUrl.param === 'directSch') {
                                that.setFocusToSecondItem('channelsetting', 'automaticSearch');
                            }
                            else if (that.pageUrl.param === 'startBootupHumaxOta') {
                                that.setFocusToSecondItem('systemsetting', 'systeminfo');
                                that._gotoSubPage('CPgSystem?set=startBootupHumaxOta');
                            }
                            else if (that.pageUrl.param === 'startInstallStandbyOta') {
                                that.setFocusToSecondItem('systemsetting', 'systeminfo');
                                that._gotoSubPage('CPgSystem?set=startInstallStandbyOta');
                            }
                            else if (that.pageUrl.param === 'startAutomaticSearch') {
                                that.setFocusToSecondItem('channelsetting', 'automaticSearch');
                                that._gotoSubPage('CPgAutomaticSearch');
                            }
                            else if (that.pageUrl.param === 'privacypolicy') {
                                that.setFocusToSecondItem('generalsetting', 'privacypolicy');
                                that._gotoSubPage('CPgPrivacyPolicy');
                            }
                            else {
                                that._2ndItems.$.get(0).style.cssText = '';
                                that._1stItems.setItemFocus(that._index.m);
                                that._1stItems.on();
                            }
                        }
                        else {
                            that._2ndItems.$.get(0).style.cssText = '';
                            that._1stItems.setItemFocus(that._index.m);
                            that._1stItems.on();
                        }
                        that._disableSubMenu();
                    }
                });
            }
            that.isShow = !that.isShow;
            if (that.isShow) {
                that.$section.addClass("show");
            }
            else {
                that.$section.removeClass("show");
            }
        };
        CPgSettings.prototype.backToMain = function () {
            var _this = this;
            var outStyle = this.aniConfig.menu.changeOut2;
            this.$hgroup.ani({
                style: this.aniConfig.fadeOut,
                trans: {
                    dur: 100
                }
            });
            this.$menuWrap.ani({
                style: outStyle,
                trans: {
                    dur: 100,
                    delay: 50
                },
                cb: function () {
                    _this.$hgroup[0].children[0].innerHTML = hx.l('LOC_CSETTINGS_ID');
                    _this.$menuWrap.css('visibility', 'visible');
                    $.extend(_this, {
                        _1stItems: _this.firstItems(),
                        _2ndItems: _this.secondItems(_this._index.m)
                    });
                    _this.$menuWrap.append(_this._1stItems.$.after(_this._2ndItems.$));
                    hx.al.compose(_this).append(_this._1stItems).append(_this._2ndItems);
                    _this.$hgroup.ani({
                        style: _this.aniConfig.fadeIn,
                        trans: {
                            dur: 50,
                            delay: 150
                        }
                    });
                    _this.$menuWrap.ani({
                        style: _this.aniConfig.menu.changeIn,
                        trans: {
                            dur: 100,
                            delay: 200
                        },
                        cb: function () {
                            _this._init('second', _this._index.s, ['setFrontPanel']);
                            _this._disableSubMenu();
                        }
                    });
                }
            });
        };
        CPgSettings.prototype._createService = function (aParam) {
            var that = this, n, target;
            for (n = 0; n < aParam.length; n += 1) {
                target = aParam[n];
                switch (target) {
                    case 'setFrontPanel':
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text: '-SETTINGS-'
                        });
                        break;
                    case 'clearFrontPanel':
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text: ''
                        });
                        break;
                    default:
                        break;
                }
            }
        };
        CPgSettings.prototype._destroyConfigService = function () {
        };
        CPgSettings.prototype._focusProcess = function (aSelected, aIndex) {
            switch (aSelected) {
                case 'first':
                    this.activateMainDep1(aIndex);
                    break;
                case 'second':
                    this.activateMainDep2(aIndex);
                    break;
                default:
                    break;
            }
        };
        CPgSettings.prototype._make2ndItems = function (aIndex) {
            if (this._2ndItems) {
                this._2ndItems.$.remove();
                this.pickout();
            }
            this._2ndItems = this.secondItems(aIndex);
            this.append(this._2ndItems);
            this.$menuWrap.append(this._2ndItems.$);
            this._disableSubMenu();
        };
        CPgSettings.prototype._exit = function () {
            var _this = this;
            this._createService(['clearFrontPanel']);
            this._keyOff();
            this.$section.ani({
                style: this.aniConfig.fadeOut,
                trans: { dur: 300 },
                cb: function () {
                    _this.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgLiveController',
                        me: _this
                    });
                }
            });
        };
        /* service Event */
        CPgSettings.prototype._on_PhysicalStorageAttached = function (aPhysicalStorage) {
            var ulItemIdx = -1, ulmainItemIdx = -1;
            hx.log('settings', '_on_PhysicalStorageAttached');
            ulmainItemIdx = this._conv_itemID_to_index('first', 0, 'pvrsetting');
            if (ulmainItemIdx === -1) {
                hx.log('settings', '_on_PhysicalStorageAttached - not found');
            }
            ulItemIdx = this._conv_itemID_to_index('second', ulmainItemIdx, 'remoterecording');
            if (ulItemIdx != -1) {
                this._2ndItems.getItem(ulItemIdx).removeClass('dis');
            }
            ulItemIdx = this._conv_itemID_to_index('second', ulmainItemIdx, 'recordingoption');
            if (ulItemIdx != -1) {
                this._2ndItems.getItem(ulItemIdx).removeClass('dis');
            }
            ulItemIdx = this._conv_itemID_to_index('second', ulmainItemIdx, 'playbackoption');
            if (ulItemIdx != -1) {
                this._2ndItems.getItem(ulItemIdx).removeClass('dis');
            }
            ulItemIdx = this._conv_itemID_to_index('second', ulmainItemIdx, 'storage');
            if (ulItemIdx != -1) {
                this._2ndItems.getItem(ulItemIdx).removeClass('dis');
            }
            this._disableSubMenu();
        };
        CPgSettings.prototype._on_PhysicalStorageDetached = function (aPhysicalStorage) {
            var szItemID, ulFocus = 0;
            hx.log('settings', '_on_PhysicalStorageDetached');
            this._disableSubMenu();
            szItemID = this._conv_index_to_itemID('first', 0, this._index.m);
            if (szItemID === 'pvrsetting') {
                szItemID = this._conv_index_to_itemID('second', this._index.m, this._index.s);
                if (szItemID === 'storage') {
                    ulFocus = this._findFocus('second', this._index.m);
                    this._2ndItems.setItemFocus(ulFocus);
                }
            }
        };
        CPgSettings.prototype._on_tryConnection = function (aResult) {
            var that = this;
            var szItemID, ulItemIdx = 0;
            hx.logadd('connectionResult');
            hx.log('connectionResult', 'connectionResult : ' + aResult);
            szItemID = that._conv_index_to_itemID('first', 0, that._index.m);
            if (szItemID === 'networksetting') {
                ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurelan');
                if (that._2ndItems.getItem(ulItemIdx).find('em').length >= 1) {
                    if (aResult === 'success') {
                        that._wireDevState = 'Connected';
                        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurelan');
                        that._2ndItems.getItem(ulItemIdx).find('em').html(hx.l('LOC_CONNECTED_ID'));
                        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurewlan');
                        if (!hx.config.testWifiOnDebug) {
                            that._2ndItems.getItem(ulItemIdx).addClass('dis');
                        }
                        var _$secondItemEm = that._2ndItems.getItem(ulItemIdx).find('em');
                        if (_$secondItemEm) {
                            if (_$secondItemEm.hasClass('signal')) {
                                _$secondItemEm.removeClass('signal');
                                _$secondItemEm.removeClass('int0');
                                _$secondItemEm.removeClass('int1');
                                _$secondItemEm.removeClass('int2');
                                _$secondItemEm.removeClass('int3');
                                _$secondItemEm.removeClass('int4');
                            }
                        }
                    }
                    else if (aResult === 'fail' || aResult === 'discon') {
                        that._wireDevState = 'Disconnected';
                        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurewlan');
                        that._2ndItems.getItem(ulItemIdx).removeClass('dis');
                        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurelan');
                        that._2ndItems.getItem(ulItemIdx).find('em').html(hx.l('LOC_DISCONNECTED_ID'));
                    }
                }
                else {
                    if (aResult === 'success') {
                        that._wireDevState = 'Connected';
                        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurelan');
                        that._2ndItems.getItem(ulItemIdx).append($('<em>').html(hx.l('LOC_CONNECTED_ID')));
                        that._2ndItems.getItem(ulItemIdx).find('em')[0].style.fontSize = '24px';
                        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurewlan');
                        if (!hx.config.testWifiOnDebug) {
                            that._2ndItems.getItem(ulItemIdx).addClass('dis');
                        }
                        var _$secondItemEm = that._2ndItems.getItem(ulItemIdx).find('em');
                        if (_$secondItemEm) {
                            if (_$secondItemEm.hasClass('signal')) {
                                _$secondItemEm.removeClass('signal');
                                _$secondItemEm.removeClass('int0');
                                _$secondItemEm.removeClass('int1');
                                _$secondItemEm.removeClass('int2');
                                _$secondItemEm.removeClass('int3');
                                _$secondItemEm.removeClass('int4');
                            }
                        }
                    }
                    else if (aResult === 'fail' || aResult === 'discon') {
                        that._wireDevState = 'Disconnected';
                        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurewlan');
                        that._2ndItems.getItem(ulItemIdx).removeClass('dis');
                        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurelan');
                        that._2ndItems.getItem(ulItemIdx).append($('<em>').html(hx.l('LOC_DISCONNECTED_ID')));
                        that._2ndItems.getItem(ulItemIdx).find('em')[0].style.fontSize = '24px';
                    }
                }
            }
        };
        CPgSettings.prototype._on_wirelessLinkStatus = function (aResult) {
            var that = this, szItemID, ulItemIdx = 0;
            var wifiSignalClass = '';
            hx.logadd('wirelessLinkStatus');
            hx.log('wirelessLinkStatus', 'wirelessLinkStatus - Result : ' + aResult);
            that._wireDevState = 'Disconnected';
            szItemID = that._conv_index_to_itemID('first', 0, that._index.m);
            if (szItemID === 'networksetting') {
                if (aResult === 'success') {
                    that._wirelessDevState = 'Connected';
                    if (that._wireDevState === 'Disconnected') {
                        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurelan');
                        if (that._2ndItems.getItem(ulItemIdx).find('em').length >= 1) {
                            that._2ndItems.getItem(ulItemIdx).find('em').html('');
                        }
                        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'configurewlan');
                        that._2ndItems.getItem(ulItemIdx).removeClass('dis');
                        var _$secondItemEm = that._2ndItems.getItem(ulItemIdx).find('em');
                        if (_$secondItemEm) {
                            if (_$secondItemEm.hasClass('signal')) {
                                _$secondItemEm.removeClass('signal');
                                _$secondItemEm.removeClass('int0');
                                _$secondItemEm.removeClass('int1');
                                _$secondItemEm.removeClass('int2');
                                _$secondItemEm.removeClass('int3');
                                _$secondItemEm.removeClass('int4');
                            }
                        }
                        var netConnectedInfo = hx.svc.SETTING_UTIL.getNetConnectedInfo();
                        hx.log('wirelessLinkStatus', 'netConnectedInfo.waveStrength : ' + netConnectedInfo.waveStrength);
                        if (netConnectedInfo.waveStrength >= 0 && netConnectedInfo.waveStrength <= 20) {
                            wifiSignalClass = 'signal int1'; //ECO
                        }
                        else if (netConnectedInfo.waveStrength > 20 && netConnectedInfo.waveStrength <= 40) {
                            wifiSignalClass = 'signal int1';
                        }
                        else if (netConnectedInfo.waveStrength > 40 && netConnectedInfo.waveStrength <= 60) {
                            wifiSignalClass = 'signal int2';
                        }
                        else if (netConnectedInfo.waveStrength > 60 && netConnectedInfo.waveStrength <= 80) {
                            wifiSignalClass = 'signal int3';
                        }
                        else if (netConnectedInfo.waveStrength > 80 && netConnectedInfo.waveStrength <= 100) {
                            wifiSignalClass = 'signal int4';
                        }
                        else {
                            wifiSignalClass = '';
                        }
                        if (that._2ndItems.getItem(ulItemIdx).find('em').length >= 1) {
                            that._2ndItems.getItem(ulItemIdx).find('em').html('');
                            that._2ndItems.getItem(ulItemIdx).find('em').addClass(wifiSignalClass);
                        }
                        else {
                            that._2ndItems.getItem(ulItemIdx).append($('<em>').html(''));
                            that._2ndItems.getItem(ulItemIdx).append($('<em>').addClass(wifiSignalClass));
                        }
                    }
                }
                else if (aResult === 'fail') {
                    that._wirelessDevState = 'Disconnected';
                }
            }
        };
        CPgSettings.prototype._on_dlVerifyPassword = function (aParam, aIndex) {
            var that = this, ulItemIdx = 0;
            function _pinfn(_status, _input) {
                if (_status === 'stop') {
                    that._settedPassword = _input;
                    hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                        password: _input,
                        cb: function (retObj) {
                            that._on_dlVerifyPassword(retObj, aIndex);
                        }
                    });
                }
                else if (_status === 'key_back') {
                    hx.msg.close('pin', that._pinMsg);
                    that._pinMsg = null;
                    switch (that._whoIsCallVerifyPassword) {
                        case 'channelsetting':
                            that._1stItems.setItemFocus(aIndex);
                            that._1stItems.on();
                            break;
                        case 'parentalcontrol':
                        case 'facdefault':
                        case 'automaticSearch':
                        case 'manualSearch':
                        case 'editchlist':
                            ulItemIdx = that._conv_itemID_to_index('second', that._index.m, that._whoIsCallVerifyPassword);
                            if (ulItemIdx != -1) {
                                that._2ndItems.setItemFocus(ulItemIdx);
                                that._2ndItems.on();
                            }
                            break;
                    }
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('pin', that._pinMsg);
                    that._pinMsg = null;
                    that._exit();
                }
            }
            // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked
            if (aParam.result === 0) {
                hx.msg.close('pin', that._pinMsg);
                that._pinMsg = null;
                switch (that._whoIsCallVerifyPassword) {
                    case 'searchchs':
                        hx.svc.WEB_STORAGE.getAntIdListFromLocal();
                        var typeObj = hx.svc.WEB_STORAGE.getLastSearchType();
                        var searchType = typeObj.searchType;
                        var targetPage;
                        hx.log('error', '');
                        hx.log('error', '');
                        hx.log('error', '######################## Get Antenna Type Start #######################');
                        hx.log('error', 'get Antenna Type in setting.js  type : ' + searchType);
                        hx.log('error', '######################## Get Antenna Type Result End #######################');
                        hx.log('error', '');
                        hx.log('error', '');
                        if ((searchType === 'lnb') || (searchType === null) || (searchType === undefined)) {
                            targetPage = 'CPgLnbSearch';
                        }
                        else if (searchType === 'diseqc') {
                            targetPage = 'CPgDiseqcSearch';
                        }
                        else if (searchType === 'scd') {
                            targetPage = 'CPgScdSearch';
                        }
                        else if (searchType === 'motor') {
                            targetPage = 'CPgMotorSearch';
                        }
                        that._gotoSubPage(targetPage);
                        break;
                    case 'editchlist':
                        that._gotoSubPage('CPgEditChannels');
                        break;
                    case 'automaticSearch':
                        that._gotoSubPage('CPgAutomaticSearch');
                        break;
                    case 'manualSearch':
                        that._gotoSubPage('CPgManualSearch');
                        break;
                    case 'channelsetting':
                        that._index.m = aIndex;
                        var ulFocus = 0;
                        if (that._2ndItems.getItem(0).hasClass('dis')) {
                            ulFocus = that._findFocus('second', aIndex);
                            that._focusProcess('second', ulFocus);
                        }
                        else {
                            that._focusProcess('second', ulFocus);
                        }
                        break;
                    case 'parentalcontrol':
                        that._gotoSubPage('CPgParental?' + that._settedPassword);
                        break;
                    case 'facdefault':
                        var fnDoFactoryReset = function () {
                            hx.msg('progress', {
                                text: hx.l('LOC_INITIALISING_ALL_SETTING_VALUES_ID'),
                                auto_close: false,
                                //timeout : 5000,
                                dia_class: 'osd'
                            });
                            hx.svc.SETTING_UTIL.doFactoryDefault();
                        };
                        if (hx.svc.SETTING_UTIL.getNetworkAvailable()) {
                            hx.svc.TV_PORTAL.deviceReset({
                                fn: function () {
                                    fnDoFactoryReset();
                                },
                                fnError: function () {
                                    //hx.msg('confirm', {
                                    //    text: hx.l('LOC_FAIL_INITIALISE_TV_APPS_ACCOUNT_ID'),
                                    //    auto_close : false,
                                    //    dia_class : 'osd',
                                    //    btn_title_arr : [hx.l('LOC_OK_ID')],
                                    //    callback_fn : function() {
                                    //        ulItemIdx = that._conv_itemID_to_index('second', that._index.m, that._whoIsCallVerifyPassword);
                                    //        if (ulItemIdx != -1) {
                                    //            that._2ndItems.setItemFocus(ulItemIdx);
                                    //            that._2ndItems.on();
                                    //        }
                                    //    }
                                    //});
                                    fnDoFactoryReset();
                                }
                            });
                        }
                        else {
                            //hx.msg('confirm', {
                            //    text : hx.l('LOC_NOT_CONNECT_TO_NETWORK_ID'),
                            //    auto_close : false,
                            //    dia_class : 'osd',
                            //    btn_title_arr : [hx.l('LOC_OK_ID')],
                            //    callback_fn : function() {
                            //       ulItemIdx = that._conv_itemID_to_index('second', that._index.m, that._whoIsCallVerifyPassword);
                            //        if (ulItemIdx != -1) {
                            //            that._2ndItems.setItemFocus(ulItemIdx);
                            //            that._2ndItems.on();
                            //        }
                            //    }
                            //});
                            fnDoFactoryReset();
                        }
                        break;
                }
            }
            else if (aParam.result === 1) {
                hx.msg.close('pin', that._pinMsg);
                that._pinMsg = hx.msg('pin', {
                    text: hx.l('LOC_ENTER_PASSWORD_ID'),
                    auto_close: false,
                    dia_class: 'osd',
                    enable: true,
                    callback_fn: _pinfn
                });
            }
        };
        CPgSettings.prototype._on_ChannelImportExport = function (aParam) {
            var that = this;
            var type = aParam.type, result = aParam.result;
            var enumErrorType = hx.svc.CHANNEL_IMPORT_EXPORT.TErrorType;
            hx.log("settings", hx.util.format('++++++ ######## _on_ChannelImportExport : type: {0}, result : {1} ######## ++++++', type, result));
            if (that._imexport_prog) {
                hx.msg.close('progress', that._imexport_prog);
                that._imexport_prog = null;
            }
            if (result === enumErrorType.EErrorNone) {
                hx.log("settings", "result success " + result);
                if (type === 1) {
                    var btnOk = hx.l('LOC_OK_ID');
                    var confirmMsg = hx.msg('confirm', {
                        text: hx.l('LOC_MSG_PROCESS_COMPLETE_ID') + hx.l('LOC_MSG_SYSTEM_WILL_RESTART_NOW_ID'),
                        btn_title_arr: [btnOk],
                        auto_close: true,
                        timeout: 30000,
                        callback_fn: function (_input) {
                            if (_input === btnOk || _input === 'auto_close' || _input === 'key_esc' || _input === 'key_back') {
                                //reboot system to update imported DB to system.
                                hx.msg.close('confirm', confirmMsg);
                                setTimeout(function () {
                                    hx.svc.SETTING_UTIL.doReboot();
                                }, 1);
                            }
                        }
                    });
                }
                else {
                    hx.msg('response', {
                        text: hx.l('LOC_MSG_PROCESS_COMPLETE_ID'),
                        auto_close: true,
                        timeout: 3000
                    });
                }
            }
            else if (result === enumErrorType.EErrorFailure) {
                hx.log("settings", "result fail " + result);
                if (type === 1) {
                    hx.msg('response', {
                        text: hx.l('LOC_IMPORT_MSG_IMPORTING_FAILURE_ID'),
                        auto_close: true,
                        close_time: 3000
                    });
                }
                else {
                    hx.msg('response', {
                        text: hx.l('LOC_EXPORT_MSG_EXPORTING_FAILURE_ID'),
                        auto_close: true,
                        close_time: 3000
                    });
                }
            }
            else if (result === enumErrorType.EErrorNoUsb) {
                hx.msg('response', {
                    text: hx.l('LOC_MSG_USB_STORAGE_NOT_DETECTED_ID'),
                    auto_close: true,
                    close_time: 3000
                });
            }
        };
        /* al Event */
        CPgSettings.prototype._on_alChangeFocus = function (aParam) {
            var i;
            if (aParam.alData.me === this._1stItems) {
                i = aParam.alData.index;
                this._index.m = i;
                this._make2ndItems(i);
                return true;
            }
            else if (aParam.alData.me === this._2ndItems) {
                this._index.s = aParam.alData.index;
                return true;
            }
            return true;
        };
        CPgSettings.prototype._on_alScrolldown = function (aParam) {
            if (aParam.alData.me === this._2ndItems) {
                if (aParam.alData.index >= this._2ndItems.getListLength() - 2) {
                    this._2ndItems.getItem(0).addClass('on');
                }
            }
            return true;
        };
        CPgSettings.prototype._on_alScrollLeft = function (aParam) {
            if (aParam.alData.me === this._2ndItems) {
                this._2ndItems.getItem(aParam.alData.index).removeClass('on');
                this._index.s = -1;
                this._focusProcess('first', this._index.m);
            }
            return true;
        };
        CPgSettings.prototype._on_alExit = function (aParam) {
            if (aParam.alData) {
                if (aParam.alData.me === this._1stItems) {
                    this._exit();
                }
                else if (aParam.alData.me === this._2ndItems) {
                    this._2ndItems.getItem(aParam.alData.index).removeClass('on');
                    this._index.s = -1;
                    this._focusProcess('first', this._index.m);
                }
            }
            return true;
        };
        CPgSettings.prototype._on_alKey = function (param) {
            switch (param.alKey) {
                case hx.key.KEY_ESCAPE:
                    if (param.alData) {
                        if (param.alData.me === this._1stItems || param.alData.me === this._2ndItems) {
                            this._exit();
                        }
                        return true;
                    }
                    break;
            }
            return false;
        };
        CPgSettings.prototype._on_alClicked = function (aParam) {
            var i = aParam.alData.index, that = this, m = that._index.m, typeObj;
            var s = that._index.s, pinSettingValue, searchType, targetPage;
            var ulItemIdx = 0, szItemID;
            var szMainItemId, szSubItemId;
            function _pinfn(_status, _input) {
                if (_status === 'stop') {
                    that._settedPassword = _input;
                    hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                        password: _input,
                        cb: function (retObj) {
                            that._on_dlVerifyPassword(retObj, i);
                        }
                    });
                }
                else if (_status === 'key_back') {
                    hx.msg.close('pin', that._pinMsg);
                    that._pinMsg = null;
                    switch (that._whoIsCallVerifyPassword) {
                        case 'channelsetting':
                            that._1stItems.setItemFocus(i);
                            that._1stItems.on();
                            break;
                        case 'parentalcontrol':
                        case 'facdefault':
                        case 'automaticSearch':
                        case 'manualSearch':
                        case 'editchlist':
                            ulItemIdx = that._conv_itemID_to_index('second', that._index.m, that._whoIsCallVerifyPassword);
                            if (ulItemIdx != -1) {
                                that._2ndItems.setItemFocus(ulItemIdx);
                                that._2ndItems.on();
                            }
                            break;
                    }
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('pin', that._pinMsg);
                    that._pinMsg = null;
                    that._exit();
                }
            }
            if (aParam.alData.me === that._1stItems) {
                szItemID = that._conv_index_to_itemID('first', 0, i);
                if (szItemID === 'channelsetting') {
                    pinSettingValue = hx.svc.PARENTAL_CTRL.getChannelsetupPin();
                    if (pinSettingValue) {
                        that._whoIsCallVerifyPassword = szItemID;
                        that._pinMsg = hx.msg('pin', {
                            text: hx.l('LOC_ENTER_PASSWORD_ID'),
                            auto_close: false,
                            dia_class: 'osd',
                            enable: true,
                            callback_fn: _pinfn
                        });
                    }
                    else {
                        that._index.m = i;
                        var ulFocus = 0;
                        if (that._2ndItems.getItem(0).hasClass('dis')) {
                            ulFocus = that._findFocus('second', i);
                            that._focusProcess('second', ulFocus);
                        }
                        else {
                            that._focusProcess('second', ulFocus);
                        }
                    }
                }
                else {
                    that._index.m = i;
                    that._focusProcess('second', 0);
                }
            }
            else if (aParam.alData.me === that._2ndItems) {
                that._index.s = i;
                szMainItemId = that._conv_index_to_itemID('first', 0, m);
                szSubItemId = that._conv_index_to_itemID('second', m, i);
                if ((szMainItemId === 'generalsetting') && (szSubItemId === 'parentalcontrol')) {
                    that._whoIsCallVerifyPassword = szSubItemId;
                    that._pinMsg = hx.msg('pin', {
                        text: hx.l('LOC_ENTER_PASSWORD_ID'),
                        auto_close: false,
                        dia_class: 'osd',
                        enable: true,
                        callback_fn: _pinfn
                    });
                    return;
                }
                else if ((hx.config.useImportExportChannels === 1) && (szMainItemId === 'channelsetting') && (szSubItemId === 'importchannels' || szSubItemId === 'exportchannels')) {
                    if (szSubItemId === 'importchannels') {
                        that._actionSimpleImportChannels();
                    }
                    else {
                        that._actionSimpleExportChannels();
                    }
                    return;
                }
                that._gotoSubPage(aParam.alData.page);
            }
        };
        CPgSettings.prototype._conv_itemID_to_index = function (szDeps, ulmainDeps, szItemID) {
            var i = 0, ulIndex = -1;
            if (szDeps === 'first') {
                for (i = 0; i < CPgSettings.items.itemIdfirst.length; i++) {
                    if (szItemID === CPgSettings.items.itemIdfirst[i]) {
                        ulIndex = i;
                        break;
                    }
                }
            }
            else if (szDeps === 'second') {
                for (i = 0; i < CPgSettings.items.itemIdsecond[ulmainDeps].length; i++) {
                    if (szItemID === CPgSettings.items.itemIdsecond[ulmainDeps][i]) {
                        ulIndex = i;
                        break;
                    }
                }
            }
            else {
                hx.log('error', 'settings -_conv_itemID_to_index Unknow deps param : [' + szDeps + ']');
                ulIndex = 0;
            }
            return ulIndex;
        };
        CPgSettings.prototype._conv_index_to_itemID = function (szDeps, ulmainDeps, ulItemIndex) {
            var i = 0, szItemId = 'undefined';
            if (szDeps === 'first') {
                for (i = 0; i < CPgSettings.items.itemIdfirst.length; i++) {
                    if (i === ulItemIndex) {
                        szItemId = CPgSettings.items.itemIdfirst[i];
                        break;
                    }
                }
            }
            else if (szDeps === 'second') {
                for (i = 0; i < CPgSettings.items.itemIdsecond[ulmainDeps].length; i++) {
                    if (i === ulItemIndex) {
                        szItemId = CPgSettings.items.itemIdsecond[ulmainDeps][i];
                        break;
                    }
                }
            }
            else {
                hx.log('error', 'settings - _conv_index_to_itemID Unknow deps param : [' + szDeps + ']');
                szItemId = CPgSettings.items.itemIdsecond[0][0];
            }
            return szItemId;
        };
        CPgSettings.prototype._findFocus = function (szDeps, ulmainDeps) {
            var i = 0, ulFocus = 0;
            if (szDeps === 'first') {
                for (i = 0; i < CPgSettings.items.itemIdfirst.length; i++) {
                    if (this._2ndItems.getItem(i).hasClass('dis')) {
                        continue;
                    }
                    else {
                        ulFocus = i;
                        break;
                    }
                }
            }
            else if (szDeps === 'second') {
                for (i = 0; i < CPgSettings.items.itemIdsecond[ulmainDeps].length; i++) {
                    if (this._2ndItems.getItem(i).hasClass('dis')) {
                        continue;
                    }
                    else {
                        ulFocus = i;
                        break;
                    }
                }
            }
            else {
                hx.log('error', 'settings -_findFocus Unknow deps param : [' + szDeps + ':' + ulmainDeps + ']');
                ulFocus = 0;
            }
            return ulFocus;
        };
        CPgSettings.prototype._disableSubMenu = function () {
            var that = this, m = that._index.m;
            var szItemID, ulItemIdx = 0, retArray;
            var wifiSignalClass = '';
            var i = 0, bFindStroage = false;
            szItemID = that._conv_index_to_itemID('first', 0, m);
            if (szItemID === 'networksetting') {
                var netConnectedInfo = hx.svc.SETTING_UTIL.getNetConnectedInfo();
                that._wireDevState = netConnectedInfo.wireDevState;
                that._wirelessDevState = netConnectedInfo.wirelessDevState;
                ulItemIdx = that._conv_itemID_to_index('second', m, 'configurelan');
                if (ulItemIdx != -1) {
                    if (that._2ndItems.getItem(ulItemIdx).find('em').length >= 1) {
                        if (netConnectedInfo.wireDevState === 'Connected') {
                            that._2ndItems.getItem(ulItemIdx).find('em').html(hx.l('LOC_CONNECTED_ID'));
                            ulItemIdx = that._conv_itemID_to_index('second', m, 'configurewlan');
                            if (ulItemIdx != -1) {
                                if (!hx.config.testWifiOnDebug) {
                                    that._2ndItems.getItem(ulItemIdx).addClass('dis');
                                    that._2ndItems.getItem(ulItemIdx).find('em').html(hx.l('LOC_NOT_CONFIGURED_ID'));
                                    that._2ndItems.getItem(ulItemIdx).find('em')[0].style.fontSize = '24px';
                                }
                            }
                            return;
                        }
                        else {
                            ulItemIdx = that._conv_itemID_to_index('second', m, 'configurewlan');
                            if (ulItemIdx != -1) {
                                that._2ndItems.getItem(ulItemIdx).removeClass('dis');
                            }
                            ulItemIdx = that._conv_itemID_to_index('second', m, 'configurelan');
                            if (ulItemIdx != -1) {
                                if (netConnectedInfo.wirelessDevState !== 'Connected') {
                                    that._2ndItems.getItem(ulItemIdx).find('em').html(hx.l('LOC_DISCONNECTED_ID'));
                                    return;
                                }
                                else {
                                    that._2ndItems.getItem(ulItemIdx).find('em').html('');
                                }
                            }
                        }
                    }
                    else {
                        if (netConnectedInfo.wireDevState === 'Connected') {
                            that._2ndItems.getItem(ulItemIdx).append($('<em>').html(hx.l('LOC_CONNECTED_ID')));
                            that._2ndItems.getItem(ulItemIdx).find('em')[0].style.fontSize = '24px';
                            ulItemIdx = that._conv_itemID_to_index('second', m, 'configurewlan');
                            if (ulItemIdx != -1) {
                                if (!hx.config.testWifiOnDebug) {
                                    that._2ndItems.getItem(ulItemIdx).addClass('dis');
                                    that._2ndItems.getItem(ulItemIdx).append($('<em>').html(hx.l('LOC_NOT_CONFIGURED_ID')));
                                    that._2ndItems.getItem(ulItemIdx).find('em')[0].style.fontSize = '24px';
                                }
                            }
                            return;
                        }
                        else {
                            ulItemIdx = that._conv_itemID_to_index('second', m, 'configurewlan');
                            if (ulItemIdx != -1) {
                                that._2ndItems.getItem(ulItemIdx).removeClass('dis');
                            }
                            ulItemIdx = that._conv_itemID_to_index('second', m, 'configurelan');
                            if (ulItemIdx != -1) {
                                if (netConnectedInfo.wirelessDevState !== 'Connected') {
                                    that._2ndItems.getItem(ulItemIdx).append($('<em>').html(hx.l('LOC_DISCONNECTED_ID')));
                                    that._2ndItems.getItem(ulItemIdx).find('em')[0].style.fontSize = '24px';
                                }
                                else {
                                    that._2ndItems.getItem(ulItemIdx).append($('<em>').html(''));
                                    that._2ndItems.getItem(ulItemIdx).find('em')[0].style.fontSize = '24px';
                                }
                            }
                        }
                    }
                }
                //wireless Update
                if (netConnectedInfo.wirelessDevState === 'Connected') {
                    var strength = netConnectedInfo.wirelessDevInfo.strength;
                    if (strength == 0) {
                        strength = 1;
                    }
                    wifiSignalClass = 'signal int' + strength;
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'configurewlan');
                    if (ulItemIdx != -1) {
                        if (that._2ndItems.getItem(ulItemIdx).find('em').length >= 1) {
                            that._2ndItems.getItem(ulItemIdx).find('em').addClass(wifiSignalClass);
                        }
                        else {
                            that._2ndItems.getItem(ulItemIdx).append($('<em>').addClass(wifiSignalClass));
                        }
                    }
                }
                else {
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'configurewlan');
                    if (ulItemIdx != -1) {
                        if (that._2ndItems.getItem(ulItemIdx).find('em').length >= 1) {
                            that._2ndItems.getItem(ulItemIdx).find('em').html(hx.l('LOC_NOT_CONFIGURED_ID'));
                            that._2ndItems.getItem(ulItemIdx).find('em')[0].style.fontSize = '24px';
                        }
                        else {
                            that._2ndItems.getItem(ulItemIdx).append($('<em>').html(hx.l('LOC_NOT_CONFIGURED_ID')));
                            that._2ndItems.getItem(ulItemIdx).find('em')[0].style.fontSize = '24px';
                        }
                    }
                }
            }
            else if (szItemID === 'pvrsetting') {
                if (that._isMedGoing || that._fileOpStatus) {
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'storage');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                }
                if (that._isRecording || that._isPbGoing) {
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'recordingoption');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'storage');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                }
                retArray = hx.svc.STORAGE_MASS.getWholeStorageInfo();
                for (i = 0; i < retArray.length; i++) {
                    switch (retArray[i].type) {
                        case 'internalPvrHDD':
                            bFindStroage = true;
                            break;
                        case 'externalPvrHDD':
                            bFindStroage = true;
                            break;
                        case 'externalHDD':
                            bFindStroage = true;
                            break;
                    }
                }
                if (bFindStroage === false) {
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'remoterecording');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'recordingoption');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'playbackoption');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                }
                if (retArray.length < 1) {
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'storage');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                }
            }
            else if (szItemID === 'systemsetting') {
                if (that._isMedGoing || that._isRecording || that._fileOpStatus || that._isPbGoing) {
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'facdefault');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                }
            }
            else if (szItemID === 'advancedsetting') {
                if (!that._isPrivacyPolicyAccepted) {
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'recorderregistration');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'remoterecording');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                }
            }
            else if (szItemID === 'channelsetting') {
                var chResult = hx.svc.CH_LIST.getChannelCount({
                    group: undefined,
                    cb: undefined
                });
                that._chCnt = chResult.chCnt;
                if (that._isMedGoing || that._fileOpStatus) {
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'automaticSearch');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'manualSearch');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    if (hx.config.useUpdateChannel === 1) {
                        ulItemIdx = that._conv_itemID_to_index('second', m, 'updatechlist');
                        if (ulItemIdx != -1) {
                            that._2ndItems.getItem(ulItemIdx).addClass('dis');
                        }
                    }
                }
                if (that._isRecording || that._isPbGoing) {
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'favouritechannels');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'editchlist');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'automaticSearch');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'manualSearch');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    if (hx.config.useImportExportChannels === 1) {
                        ulItemIdx = that._conv_itemID_to_index('second', m, 'importchannels');
                        if (ulItemIdx != -1) {
                            that._2ndItems.getItem(ulItemIdx).addClass('dis');
                        }
                        ulItemIdx = that._conv_itemID_to_index('second', m, 'exportchannels');
                        if (ulItemIdx != -1) {
                            that._2ndItems.getItem(ulItemIdx).addClass('dis');
                        }
                    }
                    if (hx.config.useUpdateChannel === 1) {
                        ulItemIdx = that._conv_itemID_to_index('second', m, 'updatechlist');
                        if (ulItemIdx != -1) {
                            that._2ndItems.getItem(ulItemIdx).addClass('dis');
                        }
                    }
                }
                if (that._chCnt === 0) {
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'favouritechannels');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    ulItemIdx = that._conv_itemID_to_index('second', m, 'editchlist');
                    if (ulItemIdx != -1) {
                        that._2ndItems.getItem(ulItemIdx).addClass('dis');
                    }
                    if (hx.config.useImportExportChannels === 1) {
                        // Perhaps, the Channel import function would be needed when there is no channel.
                        /*ulItemIdx = that._conv_itemID_to_index('second', m, 'importchannels');
                         if (ulItemIdx != -1) {
                         that._2ndItems.getItem(ulItemIdx).addClass('dis');
                         }*/
                        ulItemIdx = that._conv_itemID_to_index('second', m, 'exportchannels');
                        if (ulItemIdx != -1) {
                            that._2ndItems.getItem(ulItemIdx).addClass('dis');
                        }
                    }
                }
            }
        };
        CPgSettings.prototype._gotoSubPage = function (aTarget) {
            var _this = this;
            switch (aTarget) {
                case 'CPgAutomaticSearch':
                    var aMsgToSelectSmartRetune = hx.msg('confirm', {
                        text: "Do you want to search for new channels?<br>Select SMART to keep your scheduled recordings and personalised channel list intact or select DEFAULT for complete retune (you will lose scheduled recordings).",
                        auto_close: false,
                        //timeout : 5000,
                        btn_title_arr: ["SMART", "DEFAULT"],
                        dia_class: 'osd',
                        callback_fn: function (_status) {
                            var wholeTarget = aTarget;
                            var pageUrl = '';
                            if (_status === 'key_back' || _status === 'key_power' || _status === 'key_esc') {
                                _this.setFocusToSecondItem('channelsetting', 'automaticSearch');
                                return;
                            }
                            else if (_status === "SMART") {
                                pageUrl = '?set=smartRetune';
                                wholeTarget += pageUrl;
                            }
                            else if (_status === "DEFAULT") {
                                pageUrl = '?set=reset';
                                wholeTarget += pageUrl;
                            }
                            else {
                                return;
                            }
                            _this._gotoSubPage(wholeTarget);
                        }
                    });
                    return;
                default:
                    break;
            }
            this._keyOff();
            this.$hgroup.ani({
                style: this.aniConfig.fadeOut,
                trans: { dur: 100 }
            });
            this.$menuWrap.ani({
                style: this.aniConfig.menu.changeOut,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    _this.$menuWrap.css('visibility', 'hidden');
                    _this.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: aTarget,
                        me: _this
                    });
                }
            });
        };
        CPgSettings.prototype._keyOff = function () {
            if (this._1stItems) {
                this._1stItems.off();
            }
            if (this._2ndItems) {
                this._2ndItems.off();
            }
        };
        /* Just import db files from USB, MW shall automatically select USB storage and DB files.*/
        CPgSettings.prototype._actionSimpleImportChannels = function () {
            var that = this, stStroageArray = 0, ulItemIdx = 0;
            var ulItemIdx;
            if (that._imexport_prog) {
                hx.msg.close('progress', that._imexport_prog);
                that._imexport_prog = null;
            }
            that._imexport_prog = hx.msg('progress', {
                text: hx.l('LOC_IMPORT_CH_PROCESSING_MSG_ID'),
                auto_close: true,
                timeout: 30000,
                callback_fn: function (status) {
                    if (status === 'auto_close' || status === 'key_back') {
                        /* close importing progress */
                        if (that._imexport_prog) {
                            hx.msg.close('progress', that._imexport_prog);
                            that._imexport_prog = null;
                        }
                    }
                }
            });
            hx.svc.CHANNEL_IMPORT_EXPORT.startChannelImport();
            /* process focus. */
            ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'importchannels');
            that._2ndItems.setItemFocus(ulItemIdx);
            that._2ndItems.on();
        };
        /* Using a storage selector, select storage and db files and import DB files. (not yes completed!!!)*/
        CPgSettings.prototype._actionSimpleExportChannels = function () {
            var that = this, stStroageArray = 0, ulItemIdx = 0;
            var ulItemIdx;
            if (that._imexport_prog) {
                hx.msg.close('progress', that._imexport_prog);
                that._imexport_prog = null;
            }
            that._imexport_prog = hx.msg('progress', {
                text: hx.l('LOC_EXPORT_CH_PROCESSING_MSG_ID'),
                auto_close: true,
                timeout: 30000,
                callback_fn: function (status) {
                    if (status === 'auto_close' || status === 'key_back') {
                        /* close exporting progress */
                        if (that._imexport_prog) {
                            hx.msg.close('progress', that._imexport_prog);
                            that._imexport_prog = null;
                        }
                    }
                }
            });
            hx.svc.CHANNEL_IMPORT_EXPORT.startChannelExport();
            /* process focus */
            ulItemIdx = that._conv_itemID_to_index('second', that._index.m, 'exportchannels');
            that._2ndItems.setItemFocus(ulItemIdx);
            that._2ndItems.on();
        };
        // end of CPgSettings.prototype
        CPgSettings.prototype.title = function () {
            return hx.al.component('text', {
                tag: 'h1',
                attr: {
                    'data-langID': 'LOC_CSETTINGS_ID'
                }
            });
        };
        CPgSettings.prototype.firstItems = function () {
            return hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': 'dep1'
                },
                data: CPgSettings.items.first,
                count: CPgSettings.items.first.length,
                fx: function (i, o) {
                    o.append($('<b>', {
                        'class': 'item',
                        'html': hx.l(this.data[i])
                    }));
                    o.append($('<b>', {
                        'style': 'font-family: "PrismFontB";font-size: 36px;'
                    }).html('&nbsp'));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_RIGHT, hx.key.KEY_BACK, hx.key.KEY_ESCAPE]
            });
        };
        CPgSettings.prototype.secondItems = function (i) {
            return hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': 'dep2'
                },
                data: CPgSettings.items.second[i],
                count: CPgSettings.items.second[i].length,
                fx: function (i, o) {
                    o.append($('<b>', {
                        'class': 'item',
                        'html': hx.l(this.data[i])
                    }));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_LEFT, hx.key.KEY_BACK, hx.key.KEY_ESCAPE],
                pages: CPgSettings.items.secondLink[i]
            });
        };
        CPgSettings.prototype.okButton = function () {
            return hx.al.component('button', {
                attr: {
                    'class': 'button on',
                    'data-langID': 'LOC_OK_ID'
                },
                usePointerKey: true
            });
        };
        CPgSettings.items = {
            itemIdfirst: [],
            first: [],
            itemIdsecond: [
                [],
                [],
                [],
                [],
                [],
                []
            ],
            second: [
                [],
                [],
                [],
                [],
                [],
                []
            ],
            secondLink: [
                [],
                [],
                [],
                [],
                [],
                []
            ],
        };
        return CPgSettings;
    })(__KERNEL_SETTINGS__);
    return CPgSettings;
});
