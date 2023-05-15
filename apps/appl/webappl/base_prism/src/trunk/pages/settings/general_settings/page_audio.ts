// file name : pages/settings/general_settings/audio.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../../def/sugar.d.ts"/>
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx: ifSugar;
class cPgAudio extends  __KERNEL_SETTINGS__{
    TMenuList = {
        SPDIF : 0,
        HDMI : 1,
        AUDIOTYPE : 2,
        LIPSYNC : 3,
        AUDIODESCRIPTION : 2
    };
    _contextListMap: any;
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
        }
    };
    _$menu: JQuery;
    _$contextMenu: JQuery;
    _audioMainMenu: any;
    _audioModeHdmi: string = '';
    _audioModePcm: string = '';
    _audioType: string = '';
    _lipsyncDelay: number = 0;
    _audioDescription : any;
    _isChangePageInSetting: boolean = false;
    _currentSubMenu: number;
    _subList: any;
    constructor() {
        super();
        this._contextListMap = [['LOC_SPDIF_ID', 'LOC_HDMI_ID', 'LOC_AUDIO_DESCRIPTION_ID', 'LOC_LIPSYNC_DELAY_ID'],
            ['LOC_STEREO_ID', 'LOC_DOLBY_DIGITAL_ID'], // //S/PDIF
            ['LOC_STEREO_ID', 'LOC_DOLBY_DIGITAL_ID'], //HDMI
            ['LOC_OFF_ID', 'LOC_ON_ID'] //Audio Description
        ];
    }
    create() {
        var that = this;
        super.create();
        this._$menu = $('<article>', {
            'class' : 'menu'
        });
        this._$contextMenu = $('<div>', {
            'class' : 'dep2'
        });
        this._audioMainMenu = hx.al.CPgAudio.audioMainMenu(this._contextListMap[0], 'dep1');
        var $title = $('#set .title');
        if ($title[0]) {
            (<HTMLElement>$title[0].children[0]).innerHTML = hx.l('LOC_AUDIO_01_ID');
        }
        var retObjAudio = hx.svc.SETTING_UTIL.getAudioInfo({
                cb : undefined
        });
        this._audioModeHdmi = retObjAudio.audioModeHdmi;
        this._audioModePcm = retObjAudio.audioModePcm;
        this._audioType = retObjAudio.audioType;
        if (this._audioType === 'Unknown' || this._audioType === undefined) {
            this._audioType = 'Stereo';
        }
        this._lipsyncDelay = retObjAudio.lipSyncDelay;
        this._audioDescription = retObjAudio.audioDescription;
        if ((this._audioDescription === 'Unknown' || this._audioDescription === undefined) && ENV.op === ENV.listOP.UKDTT)  {
            this._audioDescription = true;
        }
        this._setAllAudioMenu();

        $('#set').append(this._$menu.append(this._$contextMenu.after(this._audioMainMenu.$)));

        hx.al.compose(this);

        $title.ani({
            style : that.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });
        this._$menu.ani({
            style : that.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            },
            cb : () => {
                that.append(that._audioMainMenu);
                that._audioMainMenu.on();
                that._isChangePageInSetting = false;
            }
        });
    }
    destroy() {
        super.destroy();
        this._$menu.remove();
        if (!this._isChangePageInSetting) {
            $('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }
        this._$menu = undefined;
        this._$contextMenu = undefined;
        this._audioMainMenu = undefined;
    }
    _on_alClicked(aParam: any) : boolean {
        var that = this, data = aParam.alData, target = data.me, bConsumed:boolean = false;
        switch (target) {
            case this._audioMainMenu:
                bConsumed = true;
                if (data.index === this.TMenuList.SPDIF || data.index === this.TMenuList.HDMI || data.index === this.TMenuList.AUDIOTYPE) {  //make subList
                    this._audioMainMenu.off();
                    this._makesubList(data.index);
                } else if (data.index === this.TMenuList.LIPSYNC) {
                    $('#set').hide();
                    var tmp = hx.msg('lipsync', {
                        auto_close : false,
                        lipsyncDelay : that._lipsyncDelay,
                        //dia_class : 'osd',
                        callback_fn : fn
                    });

                    function fn(aStatus: string, aLipsyncDelay: number) {
                        if (aStatus === 'ok') {
                            that._lipsyncDelay = aLipsyncDelay;
                            //set lipsyncDelay with newlipsyncDelay - old and new are same
							hx.svc.SETTING_UTIL.setAudioInfo({
                                audioDescription : that._audioDescription, audioModePcm : that._audioModePcm, audioModeHdmi : that._audioModeHdmi, audioType : that._audioType, lipSyncDelay : that._lipsyncDelay
                            });
                            $('#set').show();
                            if(that._lipsyncDelay === 0) {
                                that._audioMainMenu.getItem(3).find('em').html(that._lipsyncDelay + ' ms');
                            } else {
                                that._audioMainMenu.getItem(3).find('em').html('+' + that._lipsyncDelay + ' ms');
                            }
                            that._audioMainMenu.on();
                            that._audioMainMenu.setItemFocus(3);
                        } else if (aStatus === 'left') {
                            //set lipsyncDelay with newlipsyncDelay
							hx.svc.SETTING_UTIL.setAudioInfo({audioDescription : that._audioDescription, audioModePcm : that._audioModePcm, audioModeHdmi : that._audioModeHdmi, audioType : that._audioType, lipSyncDelay : aLipsyncDelay});
                        } else if (aStatus === 'right') {
                            //set lipsyncDelay with newlipsyncDelay
                            hx.svc.SETTING_UTIL.setAudioInfo({audioDescription : that._audioDescription, audioModePcm : that._audioModePcm, audioModeHdmi : that._audioModeHdmi, audioType : that._audioType, lipSyncDelay : aLipsyncDelay});
                        } else if (aStatus === 'key_back') {
                            //set lipsyncDelay with oldlipsyncDelay
                            hx.svc.SETTING_UTIL.setAudioInfo({audioDescription : that._audioDescription, audioModePcm : that._audioModePcm, audioModeHdmi : that._audioModeHdmi, audioType : that._audioType, lipSyncDelay : that._lipsyncDelay});
                        $('#set').show();
                            that._audioMainMenu.on();
                            that._audioMainMenu.setItemFocus(3);
                        } else if (aStatus === 'key_esc') {
                            //set lipsyncDelay with oldlipsyncDelay
							hx.svc.SETTING_UTIL.setAudioInfo({audioDescription : that._audioDescription, audioModePcm : that._audioModePcm, audioModeHdmi : that._audioModeHdmi, audioType : that._audioType, lipSyncDelay : that._lipsyncDelay});
                            $('#set').show();
                            that._audioMainMenu.on();
                            that._audioMainMenu.setItemFocus(3);
                        }
                    }
                }
                this._currentSubMenu = data.index;
                break;
            case this._subList:
                bConsumed = true;
                this._subList.off();
                this._$contextMenu.ani({
                    style: that.aniConfig.dep2.fadeOut,
                    trans: {dur: 150, delay: 75},
                    cb: () => {
                        that._$contextMenu.removeClass('active');
                        if(that._currentSubMenu === that.TMenuList.SPDIF) {
                            that._setSubSpdif(data.index);
                        } else if(that._currentSubMenu === that.TMenuList.HDMI) {
                            that._setSubHdmi(data.index);
                        } else if(that._currentSubMenu === that.TMenuList.AUDIOTYPE && ENV.op !== ENV.listOP.UKDTT) {
                            that._setSubAudioType(data.index);
                        }  else if(that._currentSubMenu === that.TMenuList.AUDIODESCRIPTION && ENV.op === ENV.listOP.UKDTT) {
                            that._setSubAudioDescription(data.index);
                        }
                    }
                });
                break;
        }
        return bConsumed;
    }
    _on_alExit(aParam: any) : boolean {
        var that = this, bConsumed: boolean = false;
        if (aParam.alData) {
            if (aParam.alData.me === this._audioMainMenu) {
                bConsumed = true;
                this._isChangePageInSetting = true;
                hx.al.decompose(this);
                this._$menu.ani({
                    style : that.aniConfig.menu.changeOut3,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb : () => {
                        that.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings',
                            me : that
                        });
                    }
                });
            }
        }
        return bConsumed;
    }
    _on_alKey(aParam: any) : boolean {
        var that = this, bConsumed = false;
        switch (aParam.alKey) {
            case hx.key.KEY_ESCAPE:
                if (aParam.alData) {
                    bConsumed = true;
                    hx.svc.SETTING_UTIL.setFrontPanelString({
                            text : ''
                    });
                    this.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgLiveController',
                        me : this
                    });
                }
                break;
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
            case hx.key.KEY_BACK:
            case hx.key.KEY_LEFT:
                if (aParam.alData) {
                    if (aParam.alData.me === this._subList) {
                        bConsumed = true;
                        this._subList.off();
                        this._$contextMenu.ani({
                            style : that.aniConfig.dep2.fadeOut,
                            trans : {
                                dur : 150,
                                delay : 75
                            },
                            cb : () => {
                                var menuFocusIndex = that._currentSubMenu;
                                that._subList.$.remove();
                                that._$contextMenu.removeClass('active');
                                that._audioMainMenu.getItem(menuFocusIndex).removeClass('his');
                                that._audioMainMenu.setItemFocus(menuFocusIndex);
                                that._audioMainMenu.on();
                            }
                        });
                    }
                }
                break;
        }
        return bConsumed;
    }
    _makesubList(aParam: number) {
        var that = this, index: number, subFocusIndex: number;
        this._currentSubMenu = aParam;
        if (aParam === this.TMenuList.SPDIF) {
            index = 1;
            if (this._audioModePcm === 'PCM') {
                subFocusIndex = 0;
            } else if (this._audioModePcm === 'Multi Channel') {
                subFocusIndex = 1;
            }
        } else if (aParam === this.TMenuList.HDMI) {
            index = 2;
            if (this._audioModeHdmi === 'PCM') {
                subFocusIndex = 0;
            } else if (this._audioModeHdmi === 'Multi Channel') {
                subFocusIndex = 1;
            }
        } else if (aParam === this.TMenuList.AUDIOTYPE && ENV.op !== ENV.listOP.UKDTT ) {
            index = 3;
            switch(this._audioType) {
                case 'Stereo' :
                    subFocusIndex = 0;
                    break;
                case 'Left':
                    subFocusIndex = 1;
                    break;
                case 'Right':
                    subFocusIndex = 2;
                    break;
                case 'Unknown' :
                case undefined :
                    that._audioType = 'Stereo';
                    subFocusIndex = 0;
                    break;
                default :
                    that._audioType = 'Stereo';
                    subFocusIndex = 0;
                    break;
            }
        } else if (aParam === this.TMenuList.AUDIODESCRIPTION && ENV.op === ENV.listOP.UKDTT ) {
            index = 3;
            switch(this._audioDescription) {
                case true : //'On' :
                    subFocusIndex = 1;
                    break;
                case false : //'Off':
                    subFocusIndex = 0;
                    break;
                case undefined :
                    this._audioDescription = true;
                    subFocusIndex = 1;
                    break;
                default :
                    this._audioDescription = true;
                    subFocusIndex = 1;
                    break;
            }
        }
        var dataList = this._contextListMap[index];

        this._subList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : subFocusIndex,
            displayCount : dataList.length
        });
        this._$contextMenu.addClass('active');
        this._audioMainMenu.getItem(aParam).addClass('his');
        this._$contextMenu.append(this._subList.$);
        this._subList.bind(dataList);
        this._subList.draw();
        this._$contextMenu.ani({
            style: that.aniConfig.dep2.fadeIn,
            trans: {dur: 200, delay: 100},
            cb: () => {
                that.append(that._subList);
                that._subList.on();
            }
        });
    }
    _setAllAudioMenu() {
        if (this._audioModePcm === 'PCM') {
            this._audioMainMenu.getItem(0).find('em').html(hx.l(this._contextListMap[1][0]));
        } else if (this._audioModePcm === 'Multi Channel') {
            this._audioMainMenu.getItem(0).find('em').html(hx.l(this._contextListMap[1][1]));
        }

        if (this._audioModeHdmi === 'PCM') {
            this._audioMainMenu.getItem(1).find('em').html(hx.l(this._contextListMap[2][0]));
        } else if (this._audioModeHdmi === 'Multi Channel') {
            this._audioMainMenu.getItem(1).find('em').html(hx.l(this._contextListMap[2][1]));
        }
        if(ENV.op !== ENV.listOP.UKDTT) {
            if (this._audioType === 'Stereo') {
                this._audioMainMenu.getItem(2).find('em').html(hx.l(this._contextListMap[3][0]));
            } else if (this._audioType === 'Left') {
                this._audioMainMenu.getItem(2).find('em').html(hx.l(this._contextListMap[3][1]));
            } else if (this._audioType === 'Right') {
                this._audioMainMenu.getItem(2).find('em').html(hx.l(this._contextListMap[3][2]));
            }
        }else if(ENV.op === ENV.listOP.UKDTT){
            if (this._audioDescription === false ) {
                this._audioMainMenu.getItem(2).find('em').html(hx.l(this._contextListMap[3][0]));
            } else {
                this._audioMainMenu.getItem(2).find('em').html(hx.l(this._contextListMap[3][1]));
            }
        }

        if(this._lipsyncDelay === 0) {
            this._audioMainMenu.getItem(3).find('em').html(this._lipsyncDelay + ' ms');
        } else {
            this._audioMainMenu.getItem(3).find('em').html('+' + this._lipsyncDelay + ' ms');
        }
    }
    _setSubSpdif(aIndex: number) {
        var $spdifSub: JQuery = this._audioMainMenu.getItem(0).find('em');
        var spdifKey: string = hx.l(this._contextListMap[1][aIndex]);
        $spdifSub.html(spdifKey);

        if (aIndex === 0) {
            this._audioModePcm = 'PCM';
        } else if (aIndex === 1) {
            this._audioModePcm = 'Multi Channel';
        }
        hx.svc.SETTING_UTIL.setAudioInfo({audioDescription : this._audioDescription, audioModePcm : this._audioModePcm, audioModeHdmi : this._audioModeHdmi, audioType : this._audioType, lipSyncDelay : this._lipsyncDelay});
        this._audioMainMenu.getItem(0).removeClass('his');
        this._subList.$.remove();
        this._$contextMenu.removeClass('active');
        this._audioMainMenu.on();
    }
    _setSubHdmi(aIndex: number) {
        var $hdmiSub: JQuery = this._audioMainMenu.getItem(1).find('em');
        var hdmiKey: string = hx.l(this._contextListMap[2][aIndex]);
        $hdmiSub.html(hdmiKey);

        if (aIndex === 0) {
            this._audioModeHdmi = 'PCM';
        } else if (aIndex === 1) {
            this._audioModeHdmi = 'Multi Channel';
        }
        hx.svc.SETTING_UTIL.setAudioInfo({audioDescription : this._audioDescription, audioModePcm : this._audioModePcm, audioModeHdmi : this._audioModeHdmi, audioType : this._audioType, lipSyncDelay : this._lipsyncDelay});
        this._audioMainMenu.getItem(1).removeClass('his');
        this._subList.$.remove();
        this._$contextMenu.removeClass('active');
        this._audioMainMenu.on();
        this._audioMainMenu.setItemFocus(1);
    }
    _setSubAudioType(aIndex: number) {
        var $audioTypeSub: JQuery = this._audioMainMenu.getItem(2).find('em');
        var audioTypeKey: string = hx.l(this._contextListMap[3][aIndex]);
        $audioTypeSub.html(audioTypeKey);

        if (aIndex === 0) {
            this._audioType = 'Stereo';
        } else if (aIndex === 1) {
            this._audioType = 'Left';
        } else if (aIndex === 2) {
            this._audioType = 'Right';
        }
        hx.svc.SETTING_UTIL.setAudioInfo({audioDescription : this._audioDescription, audioModePcm : this._audioModePcm, audioModeHdmi : this._audioModeHdmi, audioType : this._audioType, lipSyncDelay : this._lipsyncDelay});
        this._audioMainMenu.getItem(2).removeClass('his');
        this._subList.$.remove();
        this._$contextMenu.removeClass('active');
        this._audioMainMenu.on();
        this._audioMainMenu.setItemFocus(2);
    }
    _setSubAudioDescription (aIndex: number) {
        var $audioDescriptionSub: JQuery = this._audioMainMenu.getItem(2).find('em');
        var audioDescriptionKey: string = hx.l(this._contextListMap[3][aIndex]);
        $audioDescriptionSub.html(audioDescriptionKey);

        if (aIndex === 0) {
            this._audioDescription = false;
        } else if (aIndex === 1) {
            this._audioDescription = true;
        }   else  {
            this._audioDescription = true;
        }

         hx.svc.SETTING_UTIL.setAudioInfo({audioDescription : this._audioDescription, audioModePcm : this._audioModePcm, audioModeHdmi : this._audioModeHdmi, audioType : this._audioType, lipSyncDelay : this._lipsyncDelay});

        this._audioMainMenu.getItem(2).removeClass('his');
        this._subList.$.remove();
        this._$contextMenu.removeClass('active');
        this._audioMainMenu.on();
        this._audioMainMenu.setItemFocus(2);
    }
}
hx.al.CPgAudio = cPgAudio;
hx.al.CPgAudio.audioMainMenu = function(aParam, aParam2) {
    var audiomainmenu = hx.al.component('settingsList', {
        itemAttr : {},
        attr : {
            'class' : aParam2
        },
        data : aParam,
        count : aParam.length,
        fx : function (i, o)  {
            o.html(hx.l(this.data[i])).append($('<em>'));
        },
        keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
    });
    return audiomainmenu;
};
hx.al.CPgAudio.audioSubMenu = function(param) {
    var audiosubmenu = hx.al.component('settingsList', {
        itemAttr : {},
        attr : {
            'class' : 'dep2'
        },
        data : param,
        count : param.length,
        fx : function (i, o)  {
            o.html(hx.l(this.data[i]));
        },
        keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_LEFT, hx.key.KEY_BACK]
    });
    return audiosubmenu;
};
export = cPgAudio;