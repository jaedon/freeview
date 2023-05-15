/* file name : pages/settings/general_settings/video.js */
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../../def/sugar.d.ts"/>
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx: ifSugar;

class CPgSettingsVideo extends __KERNEL_SETTINGS__ {
    static menu() {
        return hx.al.component('settingsList', {
            itemAttr : {},
            attr : {
                'class' : 'dep1'
            },
            fx : function (i, o){
                o.html(this.data[i])//
                    .append($('<em>'));
            },
            data : [hx.l('LOC_SCREENRATIO_ID'), hx.l('LOC_DISPLAYFORMAT_ID'), hx.l('LOC_RESOLUTION_ID'), hx.l('LOC_SD_OUTPUT_ID')],
            count : 4,
            keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
    }
    static actMenu(list, focusIndex) {
        var dataList = list;
        var subList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : focusIndex,
            displayCount : dataList.length,
            _isNotId : true
        });
        return subList;
    }
    TMenuList = {
        ESCREEN : 0,
        EDISPLAY : 1,
        ERESOLUTION : 2,
        ESDOUTPUT : 3
    };
    TSDOutputOption = {
        OFF : 0,
        ALWAYSON : 1
    };
    TSDOutputOptionStr = [hx.l('LOC_OFF_ID'), hx.l('LOC_ALWAYS_ON_ID')];
    TSDOutputOptionDesc = [hx.l('LOC_SD_OUTPUT_OFF_DESC_ID'), hx.l('LOC_SD_OUTPUT_ALWAYS_ON_DESC_ID')];
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
    _actMenu: any;
    _tvAR: any;
    _video43: any;
    _video169: any;
    _resolution: any;
    supportedResolutions: any;
    _isChangePageInSetting: boolean;
    $menuWrap: JQuery;
    _$contextMenu: JQuery;
    _$noti: JQuery;
    _menu: any;
    _mainIndex: number;
    _tvAspectRatio: string;
    _videoDisplayFormat: string;
    _curResolution: any;
    dataList: string[];
    _sdOutputIdx: any;
    create() {
        super.create();
        this._init();
    }
    destroy() {
        super.destroy();
        if (this._actMenu) {
            this._actMenu.$.remove();
            this._actMenu = undefined;
        }
        if (this._tvAR) {
            this._tvAR.$.remove();
            this._tvAR = undefined;
        }
        if (this._video43) {
            this._video43.$.remove();
            this._video43 = undefined;
        }
        if (this._video169) {
            this._video169.$.remove();
            this._video169 = undefined;
        }
        if (this._resolution) {
            this._resolution.$.remove();
            this._resolution = undefined;
        }
        this.$menuWrap.remove();
        this._$noti.remove();
        this._destroyService();
        if (!this._isChangePageInSetting) {
            $('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }
        this.$menuWrap = undefined;
        this._$noti = undefined;
        this._$contextMenu = undefined;
        this._menu = undefined;
    }
    _init() {
        this.$menuWrap = $('<article>', {
            'class' : 'menu'
        });
        this._$contextMenu = $('<div>', {
            'class' : 'dep2'
        });
        this._$noti = $('<p>', {
            'class' : 'noti'
        });
        this._menu = CPgSettingsVideo.menu();
        var $title = $('#set .title');
        if ($title[0]) {
            (<HTMLElement>$title[0].children[0]).innerHTML = hx.l('LOC_STR_VIDEO_01_ID');
        }
        $('#set')//
            .append(this.$menuWrap.append(this._$contextMenu.after(this._menu.$)).after(this._$noti));

        hx.al.compose(this);

        $title.ani({
            style : this.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });
        this._$noti.ani({
            style : this.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            }
        });
        this.$menuWrap.ani({
            style : this.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            },
            cb : () => {
                this.append(this._menu);
                this._menu.on();
                this._isChangePageInSetting = false;
                this._mainIndex = 0;
                this._callService(['getPictureInfo', 'getOutputOption']);
            }
        });
    }
    _callService(aParam) {
        var len = aParam.length, target, handle, setData, n: number;

        for ( n = 0; n < len; n += 1) {
            target = aParam[n];
            setData = null;

            switch (target) {
                case 'setPictureInfo':
                    //                    target = 'PictureInfo';
                    setData = this._convert(null, 'webToStb');
                    hx.svc.SETTING_UTIL[target](setData);
                    break;
                case 'getPictureInfo':
                    var k;
                    //                    target = 'PictureInfo';
                    k = hx.svc.SETTING_UTIL[target]();
                    this._convert(k, 'stbToWeb');
                    break;
                case 'getOutputOption':
                    var output = hx.svc.SETTING_UTIL.getOutputOption();
                    this._updateOutput((output === 1) ? this.TSDOutputOption.OFF : this.TSDOutputOption.ALWAYSON);
                    break;
                case 'setOutputOption':
                    setData = (this._sdOutputIdx === this.TSDOutputOption.OFF) ? 1 : 0;
                    hx.svc.SETTING_UTIL.setOutputOption(setData);
                    break;
            }
        }
    }
    _destroyService() {
    }
    _updateOutput(output) {
        var that = this;
        that._sdOutputIdx = output;
        that._menu.getItem(3).children('em').html(that.TSDOutputOptionStr[output]);
        if (that._mainIndex === that.TMenuList.ESDOUTPUT) {
            that._$noti.html(that.TSDOutputOptionDesc[output]);
        }
    }
    _convert(aParam1: any, aParam2: string) {
        if (aParam2 === 'stbToWeb') {
            switch (aParam1.videoDisplayFormat) {
                case 'Sqeeze':
                    aParam1.videoDisplayFormat = (aParam1.tvAspectRatio === hx.l('LOC_SCREEN_RATIO_169_ID')) ? hx.l('LOC_PILLARBOX_ID') : hx.l('LOC_LETTERBOX_ID');
                    break;
                case 'CenterCutOut':
                    aParam1.videoDisplayFormat = (aParam1.tvAspectRatio === hx.l('LOC_SCREEN_RATIO_169_ID')) ? hx.l('LOC_ZOOM_ID') : hx.l('LOC_CENTER_ID');
                    break;
                case 'Auto':
                    aParam1.videoDisplayFormat = hx.l('LOC_AUTO_ID');
                    break;
            }

            this._tvAspectRatio = aParam1.tvAspectRatio;
            this._videoDisplayFormat = aParam1.videoDisplayFormat;
            this._menu.getItem(0).children('em').html(aParam1.tvAspectRatio);
            this._menu.getItem(1).children('em').html(aParam1.videoDisplayFormat);

            if (aParam1.supportedResolutions.length === 0 || (aParam1.supportedResolutions.length === 1 && aParam1.supportedResolutions[0] === '576i')) {
                this._resolution = null;
                aParam1.supportedResolutions = ['576i'];
                this._menu.getItem(2).children('em').html('576i');
                this._callService(['setPictureInfo']);
            } else {
                this._curResolution = aParam1.curResolution;
                this._menu.getItem(2).children('em').html(aParam1.curResolution);
            }

            this._resolution = hx.al.component('simpleMenu', {
                attr : {
                    'style' : 'display:block;',
                    'class' : 'subList'
                },
                currentindex : 0,
                displayCount : aParam1.supportedResolutions.length,
                _isNotId : true
            });
            this.supportedResolutions = aParam1.supportedResolutions;
            this._resolution.bind(aParam1.supportedResolutions);
            this._resolution.draw();
            this._menu.setItemFocus(0);
        } else if (aParam2 === 'webToStb') {
            var $em = document.getElementsByTagName('em'), returnObj: any = {}, idx = 3;

            if (this._tvAspectRatio !== $em[idx - 3].innerHTML) {
                returnObj = $.extend(returnObj, {
                    'tvAspectRatio' : $em[idx - 3].innerHTML
                });

                this._tvAspectRatio = returnObj.tvAspectRatio;
            }

            if (this._videoDisplayFormat !== $em[idx - 2].innerHTML) {
                returnObj = $.extend(returnObj, {
                    'videoDisplayFormat' : $em[idx - 2].innerHTML
                });

                this._videoDisplayFormat = returnObj.videoDisplayFormat;
            }

            if (this._curResolution !== $em[idx - 1].innerHTML) {
                returnObj = $.extend(returnObj, {
                    'curResolution' : $em[idx - 1].innerHTML
                });

                this._curResolution = returnObj.curResolution;
            }

            switch (returnObj.videoDisplayFormat) {
                case hx.l('LOC_PILLARBOX_ID'):
                case hx.l('LOC_LETTERBOX_ID'):
                    returnObj.videoDisplayFormat = 'Sqeeze';
                    break;
                case hx.l('LOC_ZOOM_ID'):
                case hx.l('LOC_CENTER_ID'):
                    returnObj.videoDisplayFormat = 'CenterCutOut';
                    break;
                case hx.l('LOC_AUTO_ID'):
                    returnObj.videoDisplayFormat = 'Auto';
                    break;
            }

            return returnObj;
        }
    }
    _on_alClicked(aParam): boolean {
        var bConsumed: boolean = false;
        if (aParam.alData.me === this._menu) {
            bConsumed = true;
            var i = aParam.alData.index;
            var o = CPgSettingsVideo.actMenu, focusIndex: number;
            this._menu.off();
            this._mainIndex = i;
            switch (i) {
                case this.TMenuList.ESCREEN:
                    switch(this._tvAspectRatio) {
                        case hx.l('LOC_SCREEN_RATIO_43_ID'):
                            focusIndex = 0;
                            break;
                        case hx.l('LOC_SCREEN_RATIO_169_ID'):
                            focusIndex = 1;
                            break;
                        default:
                            focusIndex = 0;
                    }
                    this._tvAR = o([hx.l('LOC_SCREEN_RATIO_43_ID'), hx.l('LOC_SCREEN_RATIO_169_ID')], focusIndex);
                    this.dataList = [hx.l('LOC_SCREEN_RATIO_43_ID'), hx.l('LOC_SCREEN_RATIO_169_ID')];
                    this._actMenu = this._tvAR;
                    break;
                case this.TMenuList.EDISPLAY:
                    var tvAR: string = this._tvAspectRatio;
                    if (tvAR === hx.l('LOC_SCREEN_RATIO_43_ID')) {
                        switch(this._videoDisplayFormat) {
                            case hx.l('LOC_AUTO_ID'):
                                focusIndex = 0;
                                break;
                            case hx.l('LOC_LETTERBOX_ID'):
                                focusIndex = 1;
                                break;
                            case hx.l('LOC_CENTER_ID'):
                                focusIndex = 2;
                                break;
                            default:
                                focusIndex = 0;
                        }
                        this._video43 = o([hx.l('LOC_AUTO_ID'), hx.l('LOC_LETTERBOX_ID'), hx.l('LOC_CENTER_ID')], focusIndex);
                        this.dataList = [hx.l('LOC_AUTO_ID'), hx.l('LOC_LETTERBOX_ID'), hx.l('LOC_CENTER_ID')];
                        this._actMenu = this._video43;
                    } else if (tvAR === hx.l('LOC_SCREEN_RATIO_169_ID')) {
                        switch(this._videoDisplayFormat) {
                            case hx.l('LOC_AUTO_ID'):
                                focusIndex = 0;
                                break;
                            case hx.l('LOC_PILLARBOX_ID'):
                                focusIndex = 1;
                                break;
                            case hx.l('LOC_ZOOM_ID'):
                                focusIndex = 2;
                                break;
                            default:
                                focusIndex = 0;
                        }
                        this._video169 = o([hx.l('LOC_AUTO_ID'), hx.l('LOC_PILLARBOX_ID'), hx.l('LOC_ZOOM_ID')], focusIndex);
                        this.dataList = [hx.l('LOC_AUTO_ID'), hx.l('LOC_PILLARBOX_ID'), hx.l('LOC_ZOOM_ID')];
                        this._actMenu = this._video169;
                    }
                    break;
                case this.TMenuList.ERESOLUTION:
                    if (this.supportedResolutions) {
                        var j;
                        for ( j = 0; j < this.supportedResolutions.length; j += 1) {
                            if (this._menu.getItem(this.TMenuList.ERESOLUTION).find('em')[0].innerHTML === this.supportedResolutions[j]) {
                                focusIndex = j;
                            }
                        }
                    }
                    this._actMenu = this._resolution;
                    this.dataList = this.supportedResolutions;
                    break;
                case this.TMenuList.ESDOUTPUT:
                    this._tvAR = o(this.TSDOutputOptionStr, this._sdOutputIdx);
                    this.dataList = this.TSDOutputOptionStr;
                    this._actMenu = this._tvAR;
                    break;
            }

            if (this._actMenu) {
                this._$contextMenu.addClass('active');
                this._menu.getItem(i).addClass('his');
                this._$contextMenu.append(this._actMenu.$);
                this._actMenu.bind(this.dataList);
                this._actMenu.draw();
                this._actMenu.setItemFocus(focusIndex);
                this._$contextMenu.ani({
                    style : this.aniConfig.dep2.fadeIn,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb : () => {
                        this.append(this._actMenu);
                        this._actMenu.on();
                    }
                });

                //i = document.getElementsByTagName('em').length - 3 + i;
                var curItem = this._menu.getItem(i).find('em').html(), n;

                //document.getElementsByTagName('em')[i].innerHTML, n;
                for ( n = 0; n < this._actMenu.count; n += 1) {
                    if (curItem === this._actMenu.data[n]) {
                        this._actMenu.setItemFocus(n);
                        break;
                    }
                }

            }
        } else if (aParam.alData.me === this._actMenu) {
            bConsumed = true;
            this._actMenu.off();
            this._$contextMenu.ani({
                style : this.aniConfig.dep2.fadeOut,
                trans : {
                    dur : 150,
                    delay : 75
                },
                cb : () => {
                    var menu: any = document.getElementsByClassName('his')[0];
                    var oldItem = menu.getElementsByTagName('em')[0].innerHTML;
                    i = aParam.alData.index;
                    curItem = this.dataList[i];
                    this._$contextMenu.removeClass('active');
                    this._actMenu.$.remove();
                    $(menu).removeClass('his');
                    if (oldItem !== curItem) {
                        var j = this._mainIndex//  $em.length - 3 + ;
                        this._menu.getItem(j).find('em').html(curItem);
                        if (j === this.TMenuList.ESDOUTPUT) {
                            this._updateOutput(this.TSDOutputOptionStr.indexOf(curItem));
                            this._callService(['setOutputOption']);
                        } else {
                            if (menu.index === this.TMenuList.ESCREEN) {
                                this._menu.getItem(1).find('em').html(hx.l('LOC_AUTO_ID'));
                            }
                            this._callService(['setPictureInfo', 'getPictureInfo']);
                        }
                    }
                    this._menu.setItemFocus(this._mainIndex);
                    this._menu.on();
                }
            });
        }
        return bConsumed;
    }
    _on_alExit(aParam): boolean {
        var bConsumed: boolean = false;
        if (aParam.alData) {
            if (aParam.alData.me === this._menu) {
                bConsumed = true;
                this._isChangePageInSetting = true;
                hx.al.decompose(this);
                this._$noti.ani({
                    style: this.aniConfig.menu.changeOut3,
                    trans: {dur: 200, delay: 100}
                });
                this.$menuWrap.ani({
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
                    bConsumed = true;
                    if (aParam.alData.me === this._actMenu) {
                        this._actMenu.off();
                        this._$contextMenu.ani({
                            style : this.aniConfig.dep2.fadeOut,
                            trans : {
                                dur : 150,
                                delay : 75
                            },
                            cb : () => {
                                this._$contextMenu.removeClass('active');
                                this._actMenu.$.remove();
                                $(document.getElementsByClassName('his')[0]).removeClass('his');
                                this._menu.setItemFocus(this._mainIndex);
                                this._menu.on();
                            }
                        });
                    }
                }
                break;
        }
        return bConsumed;
    }
    _on_alChangeFocus(param) {
        var that = this;
        var focusIdx = param.alData.index, target = param.alData.me;
        this._mainIndex = focusIdx;
        if (target === that._menu && focusIdx === that.TMenuList.ESDOUTPUT) {
            that._$noti.html(that.TSDOutputOptionDesc[that._sdOutputIdx]);
        } else {
            that._$noti.html('');
        }
    }
}

export = CPgSettingsVideo;
