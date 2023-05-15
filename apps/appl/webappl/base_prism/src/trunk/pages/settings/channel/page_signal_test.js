var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    //file name : pages/settings/channel/signal_test.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgSignalTest = (function (_super) {
        __extends(cCPgSignalTest, _super);
        function cCPgSignalTest() {
            _super.call(this);
            this.TMenuItemSignalTestList = {
                SIGNAL_TUNER1: 0,
                SIGNAL_TUNER2: 1,
                SIGNAL_TUNER3: 2,
                SIGNAL_TUNER4: 3
            };
            this._isChangePageInSetting = false;
        }
        cCPgSignalTest.prototype.create = function () {
            _super.prototype.create.call(this);
            var that = this;
            $.extend(this, {
                tuningParams: [],
                _$menu: $('<article>', {
                    'class': 'wrap sigTest icoRec'
                }),
                _$box1: $('<div>', {
                    'class': 'box'
                }),
                _$sigText1: $('<h2>'),
                _strength1: hx.al.CPgSignalTest.strength(0),
                _quality1: hx.al.CPgSignalTest.quality(0),
                _$box2: $('<div>', {
                    'class': 'box'
                }),
                _$sigText2: $('<h2>'),
                _strength2: hx.al.CPgSignalTest.strength(0),
                _quality2: hx.al.CPgSignalTest.quality(0),
                _$box3: $('<div>', {
                    'class': 'box'
                }),
                _$sigText3: $('<h2>'),
                _strength3: hx.al.CPgSignalTest.strength(0),
                _quality3: hx.al.CPgSignalTest.quality(0),
                _$box4: $('<div>', {
                    'class': 'box'
                }),
                _$sigText4: $('<h2>'),
                _strength4: hx.al.CPgSignalTest.strength(0),
                _quality4: hx.al.CPgSignalTest.quality(0),
                _doneBtn: hx.al.component('button', {
                    attr: {
                        'class': 'button',
                        'data-langID': ''
                    },
                    usePointerKey: true
                })
            });
            var set_title = $('#set .title');
            if (set_title[0]) {
                set_title[0].children[0].innerHTML = hx.l('LOC_SIGNAL_TEST_ID');
            }
            this._$sigText1.append($('<b>', {
                'html': hx.l('LOC_TUNER_1_ID')
            })).append($('<span>', {
                'class': ''
            })).append($('<em>', {
                'html': ''
            }));
            if (hx.config.maxTuner > this.TMenuItemSignalTestList.SIGNAL_TUNER2) {
                this._$sigText2.append($('<b>', {
                    'html': hx.l('LOC_TUNER_2_ID')
                })).append($('<span>', {
                    'class': ''
                })).append($('<em>', {
                    'html': ''
                }));
            }
            if (hx.config.maxTuner > this.TMenuItemSignalTestList.SIGNAL_TUNER3) {
                this._$sigText3.append($('<b>', {
                    'html': hx.l('LOC_TUNER_3_ID')
                })).append($('<span>', {
                    'class': ''
                })).append($('<em>', {
                    'html': ''
                }));
            }
            if (hx.config.maxTuner > this.TMenuItemSignalTestList.SIGNAL_TUNER4) {
                this._$sigText4.append($('<b>', {
                    'html': hx.l('LOC_TUNER_4_ID')
                })).append($('<span>', {
                    'class': ''
                })).append($('<em>', {
                    'html': ''
                }));
            }
            $('#set').append(this._$menu);
            if (hx.config.maxTuner === (this.TMenuItemSignalTestList.SIGNAL_TUNER4 + 1)) {
                this._$menu.append(this._$box1.append(this._$sigText1.after(this._strength1.$.after(this._quality1.$))).after(this._$box2.append(this._$sigText2.after(this._strength2.$.after(this._quality2.$))).after(this._$box3.append(this._$sigText3.after(this._strength3.$.after(this._quality3.$))).after(this._$box4.append(this._$sigText4.after(this._strength4.$.after(this._quality4.$))).after(this._doneBtn.$)))));
            }
            else if (hx.config.maxTuner === (this.TMenuItemSignalTestList.SIGNAL_TUNER3 + 1)) {
                this._$menu.append(this._$box1.append(this._$sigText1.after(this._strength1.$.after(this._quality1.$))).after(this._$box2.append(this._$sigText2.after(this._strength2.$.after(this._quality2.$))).after(this._$box3.append(this._$sigText3.after(this._strength3.$.after(this._quality3.$))).after(this._doneBtn.$))));
            }
            else if (hx.config.maxTuner === (this.TMenuItemSignalTestList.SIGNAL_TUNER2 + 1)) {
                this._$menu.append(this._$box1.append(this._$sigText1.after(this._strength1.$.after(this._quality1.$))).after(this._$box2.append(this._$sigText2.after(this._strength2.$.after(this._quality2.$))).after(this._doneBtn.$)));
            }
            else {
                this._$menu.append(this._$box1.append(this._$sigText1.after(this._strength1.$.after(this._quality1.$))).after(this._doneBtn.$));
            }
            hx.al.compose(this);
            set_title.ani({
                style: this.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            $('#set .wrap.sigTest').ani({
                style: that.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                },
                cb: function () {
                    that.append(that._doneBtn);
                    that._doneBtn.on();
                    that._doneBtn.$.addClass('on');
                    that._isChangePageInSetting = false;
                    if (ENV.listModel.HMS1000S === ENV.model) {
                        that._doneBtn.$.html(hx.l('LOC_CHECK_ANTENNA_CONNECTION_ID'));
                        that._doneBtn.$.addClass('antconn');
                    }
                    else {
                        that._doneBtn.$.html(hx.l('LOC_DONE_ID'));
                    }
                    hx.svc.SETTING_UTIL.addEventCb('NotiSignalInfo', that, function (aParam) {
                        that._on_NotiSignalInfo(aParam);
                    });
                    hx.svc.SETTING_UTIL.getSignalInfo();
                }
            });
        };
        cCPgSignalTest.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            hx.svc.SETTING_UTIL.clearEventCb(this);
            hx.svc.SETTING_UTIL.stopGetSignalInfo();
            clearInterval(this._timerSignalInform);
            this._$menu.remove();
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
        };
        cCPgSignalTest.prototype._on_NotiSignalInfo = function (aParam) {
            var convName = '';
            var chNum;
            var i, j;
            this.tuningParams.length = 0;
            for (i = 0, j = 1; i < aParam.signal.length; i += 1, j += 1) {
                if (aParam.signal[i].lock) {
                    if (aParam.signal[i].strength > 100)
                        aParam.signal[i].strength = 100;
                    if (aParam.signal[i].quality > 100)
                        aParam.signal[i].quality = 100;
                }
                this['_strength' + j].$.find('dd').eq(0).html(aParam.signal[i].strength + '%');
                this['_strength' + j].$.find('dd span').css('width', aParam.signal[i].strength + '%');
                this['_quality' + j].$.find('dd').eq(0).html(aParam.signal[i].quality + '%');
                this['_quality' + j].$.find('dd span').css('width', aParam.signal[i].quality + '%');
                if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                    if (aParam.signal[i].tuningParams) {
                        this.tuningParams = aParam.signal[i].tuningParams.split(',');
                        chNum = this.getChNumFromFreq(this.tuningParams[0], this.tuningParams[1]);
                        this['_$sigText' + j].find('em').html(chNum + ', ' + this.tuningParams[0] + 'kHz' + ', ' + this.tuningParams[1] + ', ' + this.tuningParams[2] + ', ' + this.tuningParams[3]);
                    }
                    else {
                        this['_$sigText' + j].find('em').html('');
                    }
                }
                else {
                    if (aParam.signal[i].tuningParams) {
                        this.tuningParams = aParam.signal[i].tuningParams.split(',');
                        convName = this.getSwitchedSatelliteName(this.tuningParams[0]);
                        this['_$sigText' + j].find('em').html(convName + ',' + this.tuningParams[1] + ',' + this.tuningParams[2] + ',' + this.tuningParams[3] + ',' + this.tuningParams[4]);
                    }
                    else {
                        this['_$sigText' + j].find('em').html('');
                    }
                }
            }
        };
        cCPgSignalTest.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            switch (aParam.alKey) {
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
            }
            return bConsumed;
        };
        cCPgSignalTest.prototype._on_alClicked = function (aParam) {
            var _this = this;
            var data = aParam.alData, target = data.me, bConsumed = false, antennaConnectionStr;
            switch (target) {
                case this._doneBtn:
                    bConsumed = true;
                    if (ENV.listModel.HMS1000S === ENV.model) {
                        var typeObj = hx.svc.WEB_STORAGE.getLastSearchType();
                        var connType = hx.svc.SETTING_UTIL.getSignalConnectionType();
                        if (typeObj.searchType === 'scd') {
                            antennaConnectionStr = 'LOC_SCD_ANTENNA_CABLE_CONNECT_ID';
                        }
                        else {
                            if (connType === 0)
                                antennaConnectionStr = 'LOC_ONE_ANTENNA_CABLE_CONNECT_ID';
                            else if (connType === 1 || connType === 2)
                                antennaConnectionStr = 'LOC_TWO_ANTENNA_CABLES_CONNECT_ID';
                        }
                        hx.msg('response', {
                            text: hx.l(antennaConnectionStr),
                            auto_close: true,
                            close_time: 3000
                        });
                    }
                    else {
                        this._isChangePageInSetting = true;
                        hx.al.decompose(this);
                        $('#set .wrap.sigTest').ani({
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
                    break;
            }
            return bConsumed;
        };
        cCPgSignalTest.prototype._on_alExit = function (aParam) {
            var _this = this;
            var data = aParam.alData, bConsumed = false;
            if (data) {
                var target = data.me;
                switch (target) {
                    case this._doneBtn:
                        bConsumed = true;
                        this._isChangePageInSetting = true;
                        hx.al.decompose(this);
                        $('#set .wrap.sigTest').ani({
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
                        break;
                }
            }
            return bConsumed;
        };
        cCPgSignalTest.prototype.getChNumFromFreq = function (aFreq, aBandwidth) {
            var chNum, length, i;
            var frequency = parseInt(aFreq, 10);
            if (aBandwidth === '7MHz') {
                length = hx.config.settings.channelgroupT.bandwidth7MHz.centreFrequency.length;
                for (i = 0; i < length; i += 1) {
                    if (frequency === hx.config.settings.channelgroupT.bandwidth7MHz.centreFrequency[i]) {
                        break;
                    }
                }
                chNum = hx.config.settings.channelgroupT.bandwidth7MHz.channel[i];
            }
            else if (aBandwidth === '8MHz') {
                length = hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency.length;
                for (i = 0; i < length; i += 1) {
                    if (frequency === hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency[i]) {
                        break;
                    }
                }
                chNum = hx.config.settings.channelgroupT.bandwidth8MHz.channel[i];
            }
            if (chNum === undefined) {
                chNum = ' -';
            }
            return 'CH' + chNum;
        };
        cCPgSignalTest.prototype.getSwitchedSatelliteName = function (aName) {
            var that = this;
            function _SatNameConverter(name) {
                var convName = null;
                var satelNameIndex = 0;
                switch (name) {
                    case 'NONE':
                        convName = 'NONE';
                        satelNameIndex = 0;
                        break;
                    case 'AMOS_2&3':
                        convName = 'AMOS 2&amp;3 (4.0W) ';
                        satelNameIndex = 1;
                        break;
                    case 'AMOS 2&amp;3 (4.0W) ':
                        convName = 'AMOS_2&3';
                        satelNameIndex = 1;
                        break;
                    case 'APSTAR_2R':
                        convName = 'APSTAR 2R (76.5E) ';
                        satelNameIndex = 2;
                        break;
                    case 'APSTAR 2R (76.5E) ':
                        convName = 'APSTAR_2R';
                        satelNameIndex = 2;
                        break;
                    case 'ARABSAT_5A':
                        convName = 'ARABSAT 5A (30.5E) ';
                        satelNameIndex = 3;
                        break;
                    case 'ARABSAT 5A (30.5E) ':
                        convName = 'ARABSAT_5A';
                        satelNameIndex = 3;
                        break;
                    case 'ASIASAT_3S':
                        convName = 'ASIASAT 3S (105.5E) ';
                        satelNameIndex = 4;
                        break;
                    case 'ASIASAT 3S (105.5E) ':
                        convName = 'ASIASAT_3S';
                        satelNameIndex = 4;
                        break;
                    case 'ASIASAT_5':
                        convName = 'ASIASAT 5 (100.5E) ';
                        satelNameIndex = 5;
                        break;
                    case 'ASIASAT 5 (100.5E) ':
                        convName = 'ASIASAT_5';
                        satelNameIndex = 5;
                        break;
                    case 'ASTRA_1':
                        convName = 'ASTRA 1 (19.2E) ';
                        satelNameIndex = 6;
                        break;
                    case 'ASTRA 1 (19.2E) ':
                        convName = 'ASTRA_1';
                        satelNameIndex = 6;
                        break;
                    case 'ASTRA_2':
                        convName = 'ASTRA 2 (28.2E) ';
                        satelNameIndex = 7;
                        break;
                    case 'ASTRA 2 (28.2E) ':
                        convName = 'ASTRA_2';
                        satelNameIndex = 7;
                        break;
                    case 'ASTRA_3A&3B':
                        convName = 'ASTRA 3A&amp;3B (23.5E) ';
                        satelNameIndex = 8;
                        break;
                    case 'ASTRA 3A&amp;3B (23.5E) ':
                        convName = 'ASTRA_3A&3B';
                        satelNameIndex = 8;
                        break;
                    case 'ASTRA_4A':
                        convName = 'ASTRA 4A (5.0E) ';
                        satelNameIndex = 9;
                        break;
                    case 'ASTRA 4A (5.0E) ':
                        convName = 'ASTRA_4A';
                        satelNameIndex = 9;
                        break;
                    case 'ATLANTIC_BIRD_1':
                        convName = 'ATLANTIC BIRD 1 (12.5W) ';
                        satelNameIndex = 10;
                        break;
                    case 'ATLANTIC BIRD 1 (12.5W) ':
                        convName = 'ATLANTIC_BIRD_1';
                        satelNameIndex = 10;
                        break;
                    case 'ATLANTIC_BIRD_2':
                        convName = 'ATLANTIC BIRD 2 (8.0W) ';
                        satelNameIndex = 11;
                        break;
                    case 'ATLANTIC BIRD 2 (8.0W) ':
                        convName = 'ATLANTIC_BIRD_2';
                        satelNameIndex = 11;
                        break;
                    case 'ATLANTIC_BIRD_3':
                        convName = 'ATLANTIC BIRD 3 (5.0W) ';
                        satelNameIndex = 12;
                        break;
                    case 'ATLANTIC BIRD 3 (5.0W) ':
                        convName = 'ATLANTIC_BIRD_3';
                        satelNameIndex = 12;
                        break;
                    case 'BADR_4&6':
                        convName = 'BADR 4&amp;6 (26.0E) ';
                        satelNameIndex = 13;
                        break;
                    case 'BADR 4&amp;6 (26.0E) ':
                        convName = 'BADR_4&6';
                        satelNameIndex = 13;
                        break;
                    case 'EUROBIRD_1':
                        convName = 'EUROBIRD 1 (28.2E) ';
                        satelNameIndex = 14;
                        break;
                    case 'EUROBIRD 1 (28.2E) ':
                        convName = 'EUROBIRD_1';
                        satelNameIndex = 14;
                        break;
                    case 'EUROBIRD_3':
                        convName = 'EUROBIRD 3 (33.0E) ';
                        satelNameIndex = 15;
                        break;
                    case 'EUROBIRD 3 (33.0E) ':
                        convName = 'EUROBIRD_3';
                        satelNameIndex = 15;
                        break;
                    case 'EUROBIRD_9A':
                        convName = 'EUROBIRD 9A (9.0E) ';
                        satelNameIndex = 16;
                        break;
                    case 'EUROBIRD 9A (9.0E) ':
                        convName = 'EUROBIRD_9A';
                        satelNameIndex = 16;
                        break;
                    case 'EUTELSAT_W2A':
                        convName = 'EUTELSAT W2A (10.0E) ';
                        satelNameIndex = 17;
                        break;
                    case 'EUTELSAT W2A (10.0E) ':
                        convName = 'EUTELSAT_W2A';
                        satelNameIndex = 17;
                        break;
                    case 'EUTELSAT_W3A':
                        convName = 'EUTELSAT W3A (7.0E) ';
                        satelNameIndex = 18;
                        break;
                    case 'EUTELSAT W3A (7.0E) ':
                        convName = 'EUTELSAT_W3A';
                        satelNameIndex = 18;
                        break;
                    case 'EUTELSAT_W4&W7':
                        convName = 'EUTELSAT W4&amp;W7 (36.0E) ';
                        satelNameIndex = 19;
                        break;
                    case 'EUTELSAT W4&amp;W7 (36.0E) ':
                        convName = 'EUTELSAT_W4&W7';
                        satelNameIndex = 19;
                        break;
                    case 'EUTELSAT_W6':
                        convName = 'EUTELSAT W6 (21.6E) ';
                        satelNameIndex = 20;
                        break;
                    case 'EUTELSAT W6 (21.6E) ':
                        convName = 'EUTELSAT_W6';
                        satelNameIndex = 20;
                        break;
                    case 'EXPRESS_AM1':
                        convName = 'EXPRESS AM1 (40.0E) ';
                        satelNameIndex = 21;
                        break;
                    case 'EXPRESS AM1 (40.0E) ':
                        convName = 'EXPRESS_AM1';
                        satelNameIndex = 21;
                        break;
                    case 'EXPRESS_AM22':
                        convName = 'EXPRESS AM22 (53.0E) ';
                        satelNameIndex = 22;
                        break;
                    case 'EXPRESS AM22 (53.0E) ':
                        convName = 'EXPRESS_AM22';
                        satelNameIndex = 22;
                        break;
                    case 'HELLAS_SAT2':
                        convName = 'HELLAS SAT2 (39.0E) ';
                        satelNameIndex = 23;
                        break;
                    case 'HELLAS SAT2 (39.0E) ':
                        convName = 'HELLAS_SAT2';
                        satelNameIndex = 23;
                        break;
                    case 'HISPASAT_1C&1D':
                        convName = 'HISPASAT 1C&amp;1D (30.0W) ';
                        satelNameIndex = 24;
                        break;
                    case 'HISPASAT 1C&amp;1D (30.0W) ':
                        convName = 'HISPASAT_1C&1D';
                        satelNameIndex = 24;
                        break;
                    case 'HOTBIRD':
                        convName = 'HOTBIRD (13.0E) ';
                        satelNameIndex = 25;
                        break;
                    case 'HOTBIRD (13.0E) ':
                        convName = 'HOTBIRD';
                        satelNameIndex = 25;
                        break;
                    case 'INSAT_3A&4B':
                        convName = 'INSAT 3A&amp;4B (93.5E) ';
                        satelNameIndex = 26;
                        break;
                    case 'INSAT 3A&amp;4B (93.5E) ':
                        convName = 'INSAT_3A&4B';
                        satelNameIndex = 26;
                        break;
                    case 'INTELSAT_10-02':
                        convName = 'INTELSAT 10-02 (1.0W) ';
                        satelNameIndex = 27;
                        break;
                    case 'INTELSAT 10-02 (1.0W) ':
                        convName = 'INTELSAT_10-02';
                        satelNameIndex = 27;
                        break;
                    case 'INTELSAT_12':
                        convName = 'INTELSAT 12 (45.0E) ';
                        satelNameIndex = 28;
                        break;
                    case 'INTELSAT 12 (45.0E) ':
                        convName = 'INTELSAT_12';
                        satelNameIndex = 28;
                        break;
                    case 'INTELSAT_7&10':
                        convName = 'INTELSAT 7&amp;10 (68.5E) ';
                        satelNameIndex = 29;
                        break;
                    case 'INTELSAT 7&amp;10 (68.5E) ':
                        convName = 'INTELSAT_7&10';
                        satelNameIndex = 29;
                        break;
                    case 'INTELSAT_901':
                        convName = 'INTELSAT 901 (18.0W) ';
                        satelNameIndex = 30;
                        break;
                    case 'INTELSAT 901 (18.0W) ':
                        convName = 'INTELSAT_901';
                        satelNameIndex = 30;
                        break;
                    case 'INTELSAT_902':
                        convName = 'INTELSAT 902 (62.0E) ';
                        satelNameIndex = 31;
                        break;
                    case 'INTELSAT 902 (62.0E) ':
                        convName = 'INTELSAT_902';
                        satelNameIndex = 31;
                        break;
                    case 'INTELSAT_904':
                        convName = 'INTELSAT 904 (60.0E) ';
                        satelNameIndex = 32;
                        break;
                    case 'INTELSAT 904 (60.0E) ':
                        convName = 'INTELSAT_904';
                        satelNameIndex = 32;
                        break;
                    case 'INTELSAT_906':
                        convName = 'INTELSAT 906 (64.2E) ';
                        satelNameIndex = 33;
                        break;
                    case 'INTELSAT 906 (64.2E) ':
                        convName = 'INTELSAT_906';
                        satelNameIndex = 33;
                        break;
                    case 'INTELSAT_907':
                        convName = 'INTELSAT 907 (27.5W) ';
                        satelNameIndex = 34;
                        break;
                    case 'INTELSAT 907 (27.5W) ':
                        convName = 'INTELSAT_907';
                        satelNameIndex = 34;
                        break;
                    case 'NILESAT_101&102':
                        convName = 'NILESAT 101&amp;102 (7.0W) ';
                        satelNameIndex = 35;
                        break;
                    case 'NILESAT 101&amp;102 (7.0W) ':
                        convName = 'NILESAT_101&102';
                        satelNameIndex = 35;
                        break;
                    case 'NSS_12':
                        convName = 'NSS 12 (57.0E) ';
                        satelNameIndex = 36;
                        break;
                    case 'NSS 12 (57.0E) ':
                        convName = 'NSS_12';
                        satelNameIndex = 36;
                        break;
                    case 'NSS_7':
                        convName = 'NSS 7 (22.0W) ';
                        satelNameIndex = 37;
                        break;
                    case 'NSS 7 (22.0W) ':
                        convName = 'NSS_7';
                        satelNameIndex = 37;
                        break;
                    case 'TELSTAR_12':
                        convName = 'TELSTAR 12 (15.0W) ';
                        satelNameIndex = 38;
                        break;
                    case 'TELSTAR 12 (15.0W) ':
                        convName = 'TELSTAR_12';
                        satelNameIndex = 38;
                        break;
                    case 'THOR_5&6':
                        convName = 'THOR 5&amp;6 (1.0W) ';
                        satelNameIndex = 39;
                        break;
                    case 'THOR 5&amp;6 (1.0W) ':
                        convName = 'THOR_5&6';
                        satelNameIndex = 39;
                        break;
                    case 'TURKSAT_2A&3A':
                        convName = 'TURKSAT 2A&amp;3A (42.0E) ';
                        satelNameIndex = 40;
                        break;
                    case 'TURKSAT 2A&amp;3A (42.0E) ':
                        convName = 'TURKSAT_2A&3A';
                        satelNameIndex = 40;
                        break;
                    case 'YAMAL_201':
                        convName = 'YAMAL 201 (90.0E) ';
                        satelNameIndex = 41;
                        break;
                    case 'YAMAL 201 (90.0E) ':
                        convName = 'YAMAL_201';
                        satelNameIndex = 41;
                        break;
                    case 'ARABSAT_2B':
                        convName = 'ARABSAT 2B (30.5E) ';
                        satelNameIndex = 42;
                        break;
                    case 'ARABSAT 2B (30.5E) ':
                        convName = 'ARABSAT_2B';
                        satelNameIndex = 42;
                        break;
                    case 'EUTELSAT_W1':
                        convName = 'EUTELSAT W1 (10.0E) ';
                        satelNameIndex = 43;
                        break;
                    case 'EUTELSAT W1 (10.0E) ':
                        convName = 'EUTELSAT_W1';
                        satelNameIndex = 43;
                        break;
                    case 'EUTELSAT_W2':
                        convName = 'EUTELSAT W2 (16.0E) ';
                        satelNameIndex = 44;
                        break;
                    case 'EUTELSAT W2 (16.0E) ':
                        convName = 'EUTELSAT_W2';
                        satelNameIndex = 44;
                        break;
                    case 'NSS_703':
                        convName = 'NSS 703 (57.0E) ';
                        satelNameIndex = 45;
                        break;
                    case 'NSS 703 (57.0E) ':
                        convName = 'NSS_703';
                        satelNameIndex = 45;
                        break;
                    case 'SIRIUS':
                        convName = 'SIRIUS (5.0E) ';
                        satelNameIndex = 46;
                        break;
                    case 'SIRIUS (5.0E) ':
                        convName = 'SIRIUS';
                        satelNameIndex = 46;
                        break;
                    case 'TELECOM_2D':
                        convName = 'TELECOM 2D (8.0W) ';
                        satelNameIndex = 47;
                        break;
                    case 'TELECOM 2D (8.0W) ':
                        convName = 'TELECOM_2D';
                        satelNameIndex = 47;
                        break;
                    case 'EUROBIRD_9':
                        convName = 'EUROBIRD 9 (9.0E) ';
                        satelNameIndex = 48;
                        break;
                    case 'EUROBIRD 9 (9.0E) ':
                        convName = 'EUROBIRD_9';
                        satelNameIndex = 48;
                        break;
                    case 'INTELSAT_802':
                        convName = 'INTELSAT 802 (33.0E) ';
                        satelNameIndex = 49;
                        break;
                    case 'INTELSAT 802 (33.0E) ':
                        convName = 'INTELSAT_802';
                        satelNameIndex = 49;
                        break;
                    case 'THOR_3_5':
                        convName = 'THOR 3 5 (1.0W) ';
                        satelNameIndex = 50;
                        break;
                    case 'THOR 3 5 (1.0W) ':
                        convName = 'THOR_3_5';
                        satelNameIndex = 50;
                        break;
                    case 'INSAT_2E4A':
                        convName = 'INSAT 2E4A (83.0E) ';
                        satelNameIndex = 51;
                        break;
                    case 'INSAT 2E4A (83.0E) ':
                        convName = 'INSAT_2E4A';
                        satelNameIndex = 51;
                        break;
                    case 'INTELSAT_17':
                        convName = 'INTELSAT 17 (66.0E) ';
                        satelNameIndex = 52;
                        break;
                    case 'INTELSAT 17 (66.0E) ':
                        convName = 'INTELSAT_17';
                        satelNameIndex = 52;
                        break;
                    case 'YAHLIVE':
                        convName = 'YAHLIVE (52.5E) ';
                        satelNameIndex = 53;
                        break;
                    case 'YAHLIVE (52.5E) ':
                        convName = 'YAHLIVE';
                        satelNameIndex = 53;
                        break;
                    default:
                        convName = hx.l('LOC_UNKNOWN_ID');
                        satelNameIndex = 42;
                        break;
                }
                return {
                    convName: convName,
                    index: satelNameIndex
                };
            }
            return _SatNameConverter(aName).convName;
        };
        return cCPgSignalTest;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgSignalTest = cCPgSignalTest;
    hx.al.CPgSignalTest.prototype.aniConfig = {
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
    hx.al.CPgSignalTest.txt = function (aPparam, aParam2) {
        return hx.al.component('text', {
            tag: aParam2,
            text: aPparam,
            attr: {
                'data-langID': false,
                'class': 'txt'
            }
        });
    };
    hx.al.CPgSignalTest.strength = function (aParam) {
        return hx.al.component('searchProgress', {
            tag: 'dl',
            data: [hx.l('LOC_STRENGTH_ID'), aParam + '%', 'width:' + aParam + '%'],
            attr: {
                'class': 'progress type1'
            }
        });
    };
    hx.al.CPgSignalTest.quality = function (aParam) {
        return hx.al.component('searchProgress', {
            tag: 'dl',
            data: [hx.l('LOC_QUALITY_ID'), aParam + '%', 'width:' + aParam + '%'],
            attr: {
                'class': 'progress type2'
            }
        });
    };
    hx.al.CPgSignalTest.button = function (aParam) {
        return (new hx.al.component('button', {
            attr: {
                'class': 'button',
                'data-langID': aParam
            },
            usePointerKey: true,
            notSelfDestroy: true
        }));
    };
    return cCPgSignalTest;
});
