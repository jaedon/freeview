///<reference path="../../../def/sugar.d.ts" />

import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx: ifSugar;

// file name : product/fvp4000t/pages/settings/channel/manualChannelSearchT.js
/*jslint sloppy:true, nomen:true, vars:true */

class cCPgManualSearch extends  __KERNEL_SETTINGS__{
    TMenuList = {
        CHANNEL : 0,
        FREQUENCY :1,
        BANDWIDTH : 2,
        NETWORKSEARCH : 3
    };
    aniConfig : any;
    //sendEventToEntity : any;
    //append : any;

    /* local variable */
    _userChannel : string = '21';
    _userFreq : number = 474000;
    _userBand : string = '8MHz';
    _userTransmission : string = 'DVB-T';
    _userNetworkSearch : string = 'Off';
    _isChangePageInSetting : boolean = false;
    _userTpStrProgress : any;
    _contextList : any;
    _mIndex : number;
    _mainMenu : any;
    _$contextMenu : any;
    _numbers : any;
    _searchBtn : any;
    _$menuWrap : any;
    _$wrapAticle : any;
    _$menu2 : any;
    _userTpQltProgress : any;
    _subMenuScrollup : any;
    _subMenuScrolldown : any;
    _$box1 : any;
    _txt4 : any;
    _$frline : any;
    _$wrappw : any;

    constructor() {
        super();
    }

    create (aEntity, aPageUrl) {
        super.create();
        this._userChannel = '21';
        this._userFreq = 474000;
        this._userTransmission = 'DVB-T';
        this._userBand = '8MHz';
        this._userNetworkSearch = 'Off';

        $.extend(this, {
            _bPageActive : true,
            _$menuWrap : $('<article>', {
                'class' : 'menu'
            }),
            _mainMenu : hx.al.CPgManualSearch.satmainMenu(this._contextListMap[0], this._contextListMap[5], 'dep1', 'notroll'),

            _$contextMenu : $('<div>', {
                'class' : 'dep2'
            }),

            _$menu2 : $('<article>', {
                'class' : 'wrap sigTest icoRec'
            }),
            _$box1 : $('<div>', {
                'class' : 'box',
                'style' : 'margin-top:370px'
            }),
            _userTpStrProgress : hx.al.CPgManualSearch.progress(this._contextListMap[6], 'progress type1'),
            _userTpQltProgress : hx.al.CPgManualSearch.progress(this._contextListMap[7], 'progress type2'),

            _searchBtn : hx.al.CPgManualSearch.button('LOC_SEARCH_ID'),

            /* INPUT FREQUENCY RATE */
            _$wrapAticle : $('<article>', {
                'id' : 'DIA',
                'class' : 'osd',
                'style' : "display:none;"
            }),
            _$frline : $('<div>', {
                'class' : 'pas multi',
                'style' : 'top:253px;'
            }),
            _txt4 : hx.al.CPgManualSearch.txt(hx.l('LOC_ENTER_FREQUENCY_NUMBER_ID'), 'p'),
            _$wrappw : $('<p>', {
                'class' : 'wrap_pw'
            }),
            _numbers : hx.al.CPgManualSearch.numbers(6)

        });

        var $title : any = $('#set .title');
        if ($title[0]) {
            $title[0].children[0].innerHTML = hx.l('LOC_MANUAL_CHANNEL_SEARCH_ID').toUpperCase();
        }

        $('#set').append(this._$menuWrap.append(this._mainMenu.$.after(this._$contextMenu)).after(this._$menu2.append(this._$box1.append(this._userTpStrProgress.$.after(this._userTpQltProgress.$)).after(this._searchBtn.$))));

        $('#set').after(this._$wrapAticle.append(this._$frline.append(this._txt4.$.after(this._$wrappw.append(this._numbers.$)))));

        hx.al.compose(this).append(this._mainMenu).append(this._searchBtn).append(this._numbers);

        hx.il.criticalTasking.crChannelSearchTasking = true;

        // DMR uri set null ('bring in client' do not have to work)
        hx.svc.DMR.setMedia(null);

        hx.svc.AV_CTRL.stopCurChannel({
                stopAV : 1,
                stopTsr : 1
        });

        this._isChangePageInSetting = false;

        var tpParam : any = hx.svc.CHANNEL_SEARCH_T.getUserTPParam();

        this._getUserTpParam(tpParam);

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
            cb : ()=> {
                this.append(this._mainMenu);
                this._mainMenu.on();
                this._isChangePageInSetting = false;
            }
        });

        $('#set .wrap.sigTest').ani({
            style : this.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            }
        });
    }
    destroy () {
        super.destroy();
        hx.svc.SETTING_UTIL.clearEventCb(this);
        hx.il.criticalTasking.crChannelSearchTasking = false;
        //TODO : lastChannel
        this._$menuWrap.remove();
        this._$contextMenu.remove();
        this._$wrapAticle.remove();
        this._$menu2.remove();
        this._userChannel = '';
        this._userFreq = 0;
        this._userBand = '';
        this._userNetworkSearch = '';

        if (!this._isChangePageInSetting) {
            $('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }

        this._$menuWrap = undefined;
        this._$contextMenu = undefined;
        this._mainMenu = undefined;
    }
    /* dl event */
    _on_NotiSignalInfo (aParam) {
        var i : number;
        //not consider 2 conn
        var strength : number = 0;
        var quality : number = 0;

        if (aParam.signal) {
            for ( i = 0; i < aParam.signal.length; i += 1) {
                if (aParam.signal[i]) {
                    var freq = aParam.signal[i].tuningParams.split(',')[0];

                    if (aParam.signal[i].lock && freq === this._userFreq.toString()) {
                        strength = aParam.signal[i].strength;
                        quality = aParam.signal[i].quality;
                        break;
                    }
                }
            }
        }

        if (strength > 100) {
            strength = 100;
        }
        if (quality > 100) {
            quality = 100;
        }

        this._userTpStrProgress.$.find('dd').eq(0).html(strength + '%');
        this._userTpStrProgress.$.find('dd span').css('width', strength + '%');
        this._userTpQltProgress.$.find('dd').eq(0).html(quality + '%');
        this._userTpQltProgress.$.find('dd span').css('width', quality + '%');
    }
    /* al event */
    _on_alScrollup (aParam) : boolean {
        var target : any = aParam.alData.me, bConsumed : boolean = false;

        if (target === this._searchBtn) {
            bConsumed = true;
            this._searchBtn.$.removeClass('on');
            this._mainMenu.on();
            this._mainMenu.setItemFocus(4);
        } else if (target === this._mainMenu) {
            bConsumed = true;
            this._searchBtn.on();
            this._searchBtn.$.addClass('on');
        } else if (target === this._contextList) {
            bConsumed = true;
            this._subMenuScrollup(this._contextList);
        }
        return bConsumed;
    }
    _on_alScrolldown (aParam) : boolean {
        var target : any = aParam.alData.me, bConsumed : boolean = false;

        if (target === this._searchBtn) {
            bConsumed = true;
            this._searchBtn.$.removeClass('on');
            this._mainMenu.on();
        } else if (target === this._mainMenu) {
            bConsumed = true;
            this._searchBtn.$.addClass('on');
            this._searchBtn.on();
        } else if (target === this._contextList) {
            bConsumed = true;
            this._subMenuScrolldown(this._contextList);
        }
        return bConsumed;
    }
    _on_alScrollRight (aParam) : boolean {
        var data : any = aParam.alData, target : any = data.me, bConsumed : boolean = false;
        var $target : any;

        if (target === this._mainMenu) {
            bConsumed = true;
            this._mIndex = data.index;
            if (data.index === 1) {
                this._$wrapAticle.show();
                $target = $('#DIA .pas').eq(0);
                hx.il.ani.action({
                    type : 'in',
                    targets : $target
                });
                this._numbers.on();
            } else {
                this._mainMenu.getItem(data.index).addClass('his');
                var focusIndex = this._getFocusIndex(data.index);
                this._mainMenu.off();
                this._make_editSubMenu(this._mIndex, focusIndex);
            }
        }
        return bConsumed;
    }
    _on_alScrollLeft (aParam) : boolean {
        var target : any = aParam.alData.me, bConsumed : boolean = false;

        if (target === this._contextList) {
            bConsumed = true;
            this._contextList.off();
            this._$contextMenu.ani({
                style : this.aniConfig.dep2.fadeOut,
                trans : {
                    dur : 150,
                    delay : 75
                },
                cb : ()=> {
                    var subIndex : number = this._mIndex;
                    if (this._mIndex === 8) {
                        subIndex = 4;
                    }
                    this._mainMenu.getItem(subIndex).removeClass('his');
                    this._$contextMenu.removeClass('active');
                    this._contextList.$.remove();
                    this._mainMenu.on();
                    this._mainMenu.setItemFocus(subIndex);
                }
            });
        }
        return bConsumed;
    }
    _on_alClicked (aParam) : boolean {
        var that = this, data : any = aParam.alData, target : any = data.me, bConsumed : boolean = false;
        var $target : any;

        if (target === that._mainMenu) {
            bConsumed = true;
            that._mIndex = data.index;
            if (data.index === 1) {
                that._$wrapAticle.show();
                $target = $('#DIA .pas').eq(0);
                hx.il.ani.action({
                    type : 'in',
                    targets : $target
                });
                that._numbers.on();
                that._numbers.position(0);
            } else {
                that._mainMenu.getItem(data.index).addClass('his');
                var focusIndex = that._getFocusIndex(data.index);
                that._mainMenu.off();
                that._make_editSubMenu(that._mIndex, focusIndex);
            }
        } else if (target === that._contextList) {
            bConsumed = true;
            that._contextList.off();
            that._$contextMenu.ani({
                style : that.aniConfig.dep2.fadeOut,
                trans : {
                    dur : 150,
                    delay : 75
                },
                cb : function() {
                    if (that._mIndex === 0) {
                        that._setChannel(data.index, data.getFirstItem);
                    } else if (that._mIndex === 2) {
                        // TODO: transmission set
                        that._setTransmission(data.index)
                    } else if (that._mIndex === 3) {
                        that._setBandwidth(data.index);
                    } else if (that._mIndex === 4) {
                        that._setNetworkSearch(data.index);
                    }
                }
            });
        } else if (target === that._numbers) {
            bConsumed = true;
            var $tpsubFreq : any = that._mainMenu.getItem(1).find('em');
            var freqkey : any = that._numbers.getNum();
            freqkey = parseInt(freqkey, 10);

            if (that._checkFreq(freqkey)) {
                that._userFreq = freqkey;
                that._setFreq(freqkey);
                $tpsubFreq.html(freqkey);
                hx.svc.SETTING_UTIL.clearEventCb(this);
                hx.svc.SETTING_UTIL.stopGetSignalInfo();
                hx.svc.CHANNEL_SEARCH_T.setUserTempTPParam({
                        userChannel : that._userChannel,
                        userFreq : that._userFreq,
                        userTransmission : that._userTransmission,
                        userBand : that._userBand,
                        userNetworkSearch : that._userNetworkSearch
                });

                that._doStartTune();
            } else {
                hx.msg('response', {
                    text : hx.l('LOC_FREQUENCY_NOT_VALID_ID'),
                    auto_close : true,
                    close_time : 3000,
                    dia_class : 'res vaTop',
                    callback_fn : function() {
                        that._$wrapAticle.show();
                        $target = $('#DIA .pas').eq(0);
                        hx.il.ani.action({
                            type : 'in',
                            targets : $target
                        });
                        that._numbers.resetNum();
                        that._numbers.on();
                        that._numbers.position(0);
                    }
                });
            }

            that._hideNumber(1);
        } else if (data.me === that._searchBtn) {
            bConsumed = true;
            /* Transponder Setting OK Button */
            hx.svc.CHANNEL_SEARCH_T.setUserTestTPParam({
                    userChannel : that._userChannel,
                    userFreq : that._userFreq,
                    userTransmission : that._userTransmission,
                    userBand : that._userBand,
                    userNetworkSearch : that._userNetworkSearch
            });
            hx.svc.CHANNEL_SEARCH_T.setUserTPParam({
                    userChannel : that._userChannel,
                    userFreq : that._userFreq,
                    userTransmission : that._userTransmission,
                    userBand : that._userBand,
                    userNetworkSearch : that._userNetworkSearch
            });

            that._isChangePageInSetting = true;
            that._stopSignal();

            that.sendEventToEntity({
                alEvent : 'alChangePage',
                target : 'CPgSearchStart',
                me : that
            });
        }
        return bConsumed;
    }
    _on_alExit (aParam) : boolean {
        var that = this, bConsumed : boolean = false;

        if (aParam.alData) {
            if ((aParam.alData.me === that._mainMenu) || (aParam.alData.me === that._searchBtn)) {
                bConsumed = true;
                that._stopSignal();
                hx.al.decompose(that);

                that._changeToLastChannel();
                that._$menu2.ani({
                    style : that.aniConfig.menu.changeOut3,
                    trans : {
                        dur : 200,
                        delay : 100
                    }
                });

                that._$menuWrap.ani({
                    style : that.aniConfig.menu.changeOut3,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb : function() {
                        that.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings',
                            me : that
                        });
                    }
                });

                that._isChangePageInSetting = true;
            } else if (aParam.alData.me === that._contextList) {
                bConsumed = true;
                that._contextList.off();
                that._$contextMenu.ani({
                    style : that.aniConfig.dep2.fadeOut,
                    trans : {
                        dur : 150,
                        delay : 75
                    },
                    cb : function() {
                        var subIndex : number = that._mIndex;
                        if (that._mIndex === 8) {
                            subIndex = 4;
                        }
                        that._mainMenu.getItem(subIndex).removeClass('his');
                        that._$contextMenu.removeClass('active');
                        that._contextList.$.remove();
                        that._mainMenu.on();
                        that._mainMenu.setItemFocus(subIndex);
                    }
                });
            } else if (aParam.alData.me === that._numbers) {
                bConsumed = true;
                that._hideNumber(1);
            }
        }
        return bConsumed;
    }
    _on_alKey (param) : boolean {
        var bConsumed : boolean = false;

        switch (param.alKey) {
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
            case hx.key.KEY_ESCAPE:
                bConsumed = true;
                this._stopSignal();

                hx.svc.SETTING_UTIL.setFrontPanelString({
                        text : ''
                });

                this._changeToLastChannel();
                this.sendEventToEntity({
                    alEvent : 'alChangePage',
                    target : 'CPgLiveController',
                    me : this
                });

                break;
            case hx.key.KEY_POWER:
                this._stopSignal();
                hx.svc.SETTING_UTIL.setFrontPanelString({
                    text : ''
                });
                break;
            case hx.key.KEY_BACK:
                if (param.alData) {
                    bConsumed = true;
                    if (param.alData.me === this._contextList) {
                        this._contextList.off();
                        this._$contextMenu.ani({
                            style : this.aniConfig.dep2.fadeOut,
                            trans : {
                                dur : 150,
                                delay : 75
                            },
                            cb : ()=> {
                                var subIndex : number = this._mIndex;
                                if (this._mIndex === 8) {
                                    subIndex = 4;
                                }
                                this._mainMenu.getItem(subIndex).removeClass('his');
                                this._$contextMenu.removeClass('active');
                                this._contextList.$.remove();
                                this._mainMenu.on();
                                this._mainMenu.setItemFocus(subIndex);
                            }
                        });
                    }
                }
                break;
        }
        return bConsumed;
    }
    _hideNumber (param : number, param2? : string) {
        var that = this, $hideTarget, $target;
        that._numbers.off();

        if (param === 1) {
            $hideTarget = that._$wrapAticle;
            $target = $('#DIA .pas').eq(0);
        }

        hx.il.ani.action({
            type : 'out',
            targets : $target,
            cb : function() {
                $hideTarget.hide();
                if (param === 1) {
                    that._mainMenu.on();
                    that._mainMenu.setItemFocus(1);
                } else if (param === 2) {
                    if (param2 === 'select') {
                        that._mainMenu.getItem(3).removeClass('his');
                        that._$contextMenu.removeClass('active');
                        that._contextList.$.remove();
                        that._mainMenu.on();
                        that._mainMenu.setItemFocus(3);
                    } else if (param2 === 'back') {
                        that._$contextMenu.ani({
                            style : that.aniConfig.dep2.fadeIn,
                            trans : {
                                dur : 200,
                                delay : 100
                            },
                            cb : function() {
                                that._contextList.on();
                                that._contextList.getItem(4).addClass('on');
                            }
                        });
                    }
                }
            }
        });
    }
    _getSignalInfo () {
        hx.svc.SETTING_UTIL.addEventCb('NotiSignalInfo', this, (aParam) => {
            this._on_NotiSignalInfo(aParam);
        });
        hx.svc.SETTING_UTIL.getSignalInfo();
    }
    _changeToLastChannel () {
        // tune to available channel
        var result = hx.svc.AV_CTRL.lastChannel();
        if (result === false) {
            hx.svc.AV_CTRL.availableChannel();
        }
    }
    _stopSignal () {
        hx.svc.SETTING_UTIL.stopGetSignalInfo();
        hx.svc.CHANNEL_SEARCH_T.stopScan();
    }
    _doStartTune () {
        hx.svc.CHANNEL_SEARCH_T.StartTerTune();

        this._getSignalInfo();
    }
    _tpSetAfterDoStartTune () {
        hx.svc.SETTING_UTIL.clearEventCb(this);
        hx.svc.SETTING_UTIL.stopGetSignalInfo();
        hx.svc.CHANNEL_SEARCH_T.setUserTempTPParam({
                userChannel : this._userChannel,
                userFreq : this._userFreq,
                userTransmission : this._userTransmission,
                userBand : this._userBand,
                userNetworkSearch : this._userNetworkSearch
        });
        this._doStartTune();
    }
    _contextListMap = [
        ['LOC_CHANNEL_ID', 'LOC_FREQ_ID', 'LOC_TRANSMISSION_ID', 'LOC_BANDWIDTH_ID', 'LOC_NETWORKSCH_ID'],
        [], // //Channel
        ['DVB-T', 'DVB-T2'], //Transmission
        ['LOC_7MHZ_ID', 'LOC_8MHZ_ID'], //Bandwidth
        ['LOC_ON_ID', 'LOC_OFF_ID'], //Network Search
        ['', '', '', '', ''], //tpSetting
        [hx.l('LOC_STRENGTH_ID'), '0%', 'width:0%'], //progress1
        [hx.l('LOC_QUALITY_ID'), '0%', 'width:0%'] //progress2
    ];
    _getUserTpParam (aParam) {
        this._userChannel = aParam.tpParam.userChannel;
        this._userFreq = parseInt(aParam.tpParam.userFreq, 10);
        this._userTransmission = aParam.tpParam.userTransmission;
        this._userBand = aParam.tpParam.userBand;
        this._userNetworkSearch = aParam.tpParam.userNetworkSearch;

        hx.svc.CHANNEL_SEARCH_T.setUserTempTPParam({
                userChannel : this._userChannel,
                userFreq : this._userFreq,
                userTransmission : this._userTransmission,
                userBand : this._userBand,
                userNetworkSearch : this._userNetworkSearch
        });
        this._doStartTune();

        if (this._userChannel === undefined || this._userChannel === 'undefined') {
            this._mainMenu.getItem(0).find('em').html('-');
        } else {
            this._mainMenu.getItem(0).find('em').html(this._userChannel);
        }
        this._mainMenu.getItem(1).find('em').html(this._userFreq);
        this._mainMenu.getItem(2).find('em').html(this._userTransmission);
        this._mainMenu.getItem(3).find('em').html(this._userBand);
        this._mainMenu.getItem(4).find('em').html(this._userNetworkSearch);
    }
    _getFocusIndex (aIndex : number) {
        var focusIndex : number = 0, i : number;

        switch (aIndex) {
            case 0:
                if (this._userBand === '7MHz') {
                    length = hx.config.settings.channelgroupT.bandwidth7MHz.channel.length;
                    for ( i = 0; i < length; i += 1) {
                        if (this._userChannel === hx.config.settings.channelgroupT.bandwidth7MHz.channel[i]) {
                            break;
                        }
                    }

                } else if (this._userBand === '8MHz') {
                    length = hx.config.settings.channelgroupT.bandwidth8MHz.channel.length;
                    for ( i = 0; i < length; i += 1) {
                        if (this._userChannel === hx.config.settings.channelgroupT.bandwidth8MHz.channel[i]) {
                            break;
                        }
                    }
                }

                focusIndex = i;
                break;
            case 2:
                //Transmission
                if (this._userTransmission === 'DVB-T') {
                    focusIndex = 0;
                } else if (this._userTransmission === 'DVB-T2') {
                    focusIndex = 1;
                }
                break;
            case 3:
                //Bandwidth
                if (this._userBand === '7MHz') {
                    focusIndex = 0;
                } else if (this._userBand === '8MHz') {
                    focusIndex = 1;
                }
                break;
            case 4:
                //Network Search
                if (this._userNetworkSearch === 'On') {
                    focusIndex = 0;
                } else if (this._userNetworkSearch === 'Off') {
                    focusIndex = 1;
                }
                break;
            default:
                break;
        }
        return focusIndex;
    }
    _setTransmission (aIndex : number) {
        var $value : any = this._mainMenu.getItem(2).find('em');
        var value : string = this._contextListMap[this._mIndex][aIndex];

        this._userTransmission = value;
        $value.html(value);

        //TODO: userTransmission will be add
        this._tpSetAfterDoStartTune();

        this._mainMenu.getItem(2).removeClass('his');
        this._$contextMenu.removeClass('active');
        this._contextList.$.remove();
        this._mainMenu.on();
        this._mainMenu.setItemFocus(2);
    }
    _setNetworkSearch (aIndex : number) {
        var $tpsubNetwork : any = this._mainMenu.getItem(4).find('em');
        var networkSearchKey : string = hx.l(this._contextListMap[this._mIndex][aIndex]);

        if (networkSearchKey === hx.l('LOC_ON_ID')) {
            this._userNetworkSearch = 'On';
        } else if (networkSearchKey === hx.l('LOC_OFF_ID')) {
            this._userNetworkSearch = 'Off';
        }
        $tpsubNetwork.html(networkSearchKey);

        this._tpSetAfterDoStartTune();

        this._mainMenu.getItem(4).removeClass('his');
        this._$contextMenu.removeClass('active');
        this._contextList.$.remove();
        this._mainMenu.on();
        this._mainMenu.setItemFocus(4);
    }
    _setFreq (aFreq : number) {
        var $tpsubChannel : any = this._mainMenu.getItem(0).find('em');
        var length : number, tempFreq : number, i : number;

        if (this._userBand === '7MHz') {
            length = hx.config.settings.channelgroupT.bandwidth7MHz.limitFrequency.length;
            for ( i = 0; i < length; i += 1) {
                if (aFreq > hx.config.settings.channelgroupT.bandwidth7MHz.limitFrequency[i][0] && aFreq <= hx.config.settings.channelgroupT.bandwidth7MHz.limitFrequency[i][1]) {
                    break;
                }
            }
            this._userChannel = hx.config.settings.channelgroupT.bandwidth7MHz.channel[i];
        } else if (this._userBand === '8MHz') {
            length = hx.config.settings.channelgroupT.bandwidth8MHz.limitFrequency.length;
            for ( i = 0; i < length; i += 1) {
                if (aFreq > hx.config.settings.channelgroupT.bandwidth8MHz.limitFrequency[i][0] && aFreq <= hx.config.settings.channelgroupT.bandwidth8MHz.limitFrequency[i][1]) {
                    break;
                }
            }
            this._userChannel = hx.config.settings.channelgroupT.bandwidth8MHz.channel[i];
        }

        if (this._userChannel === undefined) {
            $tpsubChannel.html('-');
        } else {
            $tpsubChannel.html(this._userChannel);
        }
    }
    _checkFreq (aFreq : number) : boolean {
        var result : boolean;

        /*
         if (that._userBand === '7MHz') {
         if ( (freq < 174000 ) || (freq > 230000 && freq < 519000) || (freq > 820000) ) {
         result = false;
         } else {
         result = true;
         }
         } else if (that._userBand === '8MHz') {
         if ( (freq < 174000 ) || (freq > 230000 && freq < 518000) || (freq > 822000) ) {
         result = false;
         } else {
         result = true;
         }
         }
         */
        result = true;

        return result;
    }
    _setChannel (aIndex : number, aFirstItem) {
        var $tpsubChannel = this._mainMenu.getItem(0).find('em');
        var $tpsubFreq = this._mainMenu.getItem(1).find('em');
        var channelKey : string, freqKey : number, i : number, length : number;

        if (this._userBand === '7MHz') {
            length = hx.config.settings.channelgroupT.bandwidth7MHz.channel.length;
            for ( i = 0; i < length; i += 1) {
                if (aFirstItem === hx.config.settings.channelgroupT.bandwidth7MHz.channel[i]) {
                    break;
                }
            }

            if ((aIndex += i) >= length) {
                aIndex -= length;
            }

            this._userChannel = hx.config.settings.channelgroupT.bandwidth7MHz.channel[aIndex];
            channelKey = this._userChannel;
            this._userFreq = hx.config.settings.channelgroupT.bandwidth7MHz.centreFrequency[aIndex];
            freqKey = this._userFreq;
        } else if (this._userBand === '8MHz') {
            length = hx.config.settings.channelgroupT.bandwidth8MHz.channel.length;
            for ( i = 0; i < length; i += 1) {
                if (aFirstItem === hx.config.settings.channelgroupT.bandwidth8MHz.channel[i]) {
                    break;
                }
            }

            if ((aIndex += i) >= length) {
                aIndex -= length;
            }
            this._userChannel = hx.config.settings.channelgroupT.bandwidth8MHz.channel[aIndex];
            channelKey = this._userChannel;
            this._userFreq = hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency[aIndex];
            freqKey = this._userFreq;
        }

        if (channelKey === undefined) {
            $tpsubChannel.html('-');
        } else {
            $tpsubChannel.html(channelKey);
        }
        $tpsubFreq.html(freqKey);

        this._tpSetAfterDoStartTune();

        this._mainMenu.getItem(0).removeClass('his');
        this._$contextMenu.removeClass('active');
        this._contextList.$.remove();
        this._mainMenu.on();
        this._mainMenu.setItemFocus(0);
    }
    _setBandwidth (aIndex : number) {
        var $tpsubChannel = this._mainMenu.getItem(0).find('em');
        var $tpsubFreq = this._mainMenu.getItem(1).find('em');
        var $tpsubBand = this._mainMenu.getItem(3).find('em');
        var channelKey : string, freqKey : number;
        var bandwidthKey : string = hx.l(this._contextListMap[this._mIndex][aIndex]);

        switch (bandwidthKey) {
            case hx.l('LOC_7MHZ_ID'):
                if (this._userBand !== '7MHz') {
                    this._userBand = '7MHz';
                    this._userChannel = hx.config.settings.channelgroupT.bandwidth7MHz.channel[0];
                    this._userFreq = hx.config.settings.channelgroupT.bandwidth7MHz.centreFrequency[0];
                    channelKey = this._userChannel;
                    freqKey = this._userFreq;
                }
                break;
            case hx.l('LOC_8MHZ_ID'):
                if (this._userBand !== '8MHz') {
                    this._userBand = '8MHz';
                    this._userChannel = hx.config.settings.channelgroupT.bandwidth8MHz.channel[0];
                    this._userFreq = hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency[0];
                    channelKey = this._userChannel;
                    freqKey = this._userFreq;
                }
                break;
            default:
                this._userBand = '7MHz';
                break;
        }
        if (channelKey === undefined) {
            $tpsubChannel.html('-');
        } else {
            $tpsubChannel.html(channelKey);
        }
        $tpsubFreq.html(freqKey);
        $tpsubBand.html(bandwidthKey);

        this._tpSetAfterDoStartTune();

        this._mainMenu.getItem(3).removeClass('his');
        this._$contextMenu.removeClass('active');
        this._contextList.$.remove();
        this._mainMenu.on();
        this._mainMenu.setItemFocus(3);
    }
    _make_editSubMenu (aParam : number, aSubFocusIndex : number) {
        var dataList : any;
        var mainFocus : number;
        var isNotId : boolean = false;
        var topPx : string, i : number;
        var length : number;

        if (aParam === 0) {
            mainFocus = 0;
            isNotId = true;

            if (this._userBand === '7MHz') {
                length = hx.config.settings.channelgroupT.bandwidth7MHz.channel.length;
                this._contextListMap[1] = [];
                for ( i = 0; i < length; i += 1) {
                    this._contextListMap[1][i] = hx.config.settings.channelgroupT.bandwidth7MHz.channel[i];
                }
            } else if (this._userBand === '8MHz') {
                length = hx.config.settings.channelgroupT.bandwidth8MHz.channel.length;
                this._contextListMap[1] = [];
                for ( i = 0; i < length; i += 1) {
                    this._contextListMap[1][i] = hx.config.settings.channelgroupT.bandwidth8MHz.channel[i];
                }
            }

            dataList = this._contextListMap[1];
        } else if (aParam === 2) {
            isNotId = true;
            dataList = this._contextListMap[aParam];
            topPx = '182px';
        } else {
            dataList = this._contextListMap[aParam];

            if (aParam === this.TMenuList.BANDWIDTH)
                topPx = '182px';
            else if (aParam === this.TMenuList.NETWORKSEARCH)
                topPx = '255px';
        }

        this._contextList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : aSubFocusIndex,
            displayCount : dataList.length,
            _isNotId : isNotId,
            top : topPx
        });

        this._$contextMenu.addClass('active');
        mainFocus = aParam;
        if (aParam === 8) {
            mainFocus = 4;
        }

        this._mainMenu.getItem(mainFocus).addClass('his');
        this._$contextMenu.append(this._contextList.$);
        this._contextList.bind(dataList);
        this._contextList.draw();
        this._$contextMenu.ani({
            style : this.aniConfig.dep2.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            },
            cb : ()=> {
                this.append(this._contextList);
                this._contextList.on();
            }
        });
    }
}
hx.al.CPgManualSearch =  cCPgManualSearch;

hx.al.CPgManualSearch.prototype.aniConfig = {
    fadeIn : {
        start : {
            'opacity' : 0
        },
        end : {
            'opacity' : 1
        }
    },
    dep2 : {
        fadeIn : {
            start : {
                'opacity' : 0,
                'left' : '795px'
            },
            end : {
                'opacity' : 1,
                'left' : '775px'
            }
        },
        fadeOut : {
            start : {
                'opacity' : 1,
                'left' : '775px'
            },
            end : {
                'opacity' : 0,
                'left' : '795px'
            }
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

hx.al.CPgManualSearch.title = function() {
    return hx.al.component('text', {
        tag : 'h1',
        attr : {
            'data-langID' : 'LOC_MANUAL_CHANNEL_SEARCH_ID'
        }
    });
};

hx.al.CPgManualSearch.satmainMenu = function(aParam, aParam2, aParam3, aParam4) {
    var satmainmenu : any = hx.al.component('Search_list', {
        itemAttr : {},
        attr : {
            'class' : aParam3
        },
        roll : aParam4,
        data : aParam,
        data2 : aParam2,
        count : aParam.length,
        fx : function(i, o) {
            o.html(hx.l(this.data[i])).append($('<em>').html(this.data2[i]));
        }
    });
    return satmainmenu;
};

hx.al.CPgManualSearch.progress = function(aParam, aParam2) {
    return hx.al.component('searchProgress', {
        tag : 'dl',
        data : aParam,
        attr : {
            'class' : aParam2
        }
    });
};

hx.al.CPgManualSearch.button = function(aParam) {
    return (new hx.al.component('button', {
        attr : {
            'class' : 'button search',
            'data-langID' : aParam
        },
        usePointerKey : true
    }));
};

hx.al.CPgManualSearch.txt = function(aParam, aParam2) {
    return hx.al.component('text', {
        tag : aParam2,
        text : aParam,
        attr : {
            'data-langID' : false,
            'class' : 'txt'
        }
    });
};

hx.al.CPgManualSearch.numbers = function(aParam) {
    var numbers = hx.al.component('scnumbers', {
        baseText : '',
        count : aParam,
        attr : {
            'class' : 'wrap_num'
        }
    });
    return numbers;
};
export = cCPgManualSearch;
