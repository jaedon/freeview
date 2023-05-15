// file name : pages/settings/general_settings/screen_display.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../../def/sugar.d.ts"/>
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx: ifSugar;

class cPgScreenDisplay  extends  __KERNEL_SETTINGS__ {
    TMenuList = {
        DISPLAY_TIME : 0,
        TRANSPARENCY : 1,
        SUBTITLE_DISPLAY : 2,
        SUBTITLE_FONT : 3,
        HARD_OF_HEARING : 4
    };
    ATransparencyValue = ['1', '0.8'];
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
    _subfont: number;
    _displayTimeout: number = 0;
    _transparencyIdx: number = 1;
    _isChangePageInSetting: boolean = false;
    _$menu: JQuery;
    _$contextMenu: JQuery;
    _screenMainMenu: any;
    _subList: any;
    _currentSubMenu: number;
    _isSubtitleDisplay: number;
    _isHardofHearing: number;
    _handlers = [
        (aIndex: number) => {
            //display time
            this._displayTimeout = aIndex;
            hx.svc.SETTING_UTIL.setInfoDisplayTimeout({
                'timeout' : this._displayTimeout
            });

        },
        (aIndex: number) => {
            //display time
            this._transparencyIdx = aIndex;
            hx.svc.WEB_STORAGE.setBackgroundTransparency(this.ATransparencyValue[aIndex]);
            hx.il.updatePageBackground();
        },
        (aIndex: number) => {
//            subtitle display
            if(ENV.op === ENV.listOP.UKDTT){
                this._isSubtitleDisplay = aIndex;
                hx.svc.SETTING_UTIL.setSubtitleEnabled(!!aIndex);
                if (!aIndex) {
                    hx.svc.AV_CTRL.offSubtitleComponent();
                } else {
                    var subtitle = null;
                    var hardofHearingEnabled : boolean = hx.svc.SETTING_UTIL.getHardofHearing();
                    var p = hx.svc.AV_CTRL.getSubtitleComponent();
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

                        targetlang = hx.svc.SETTING_UTIL.getLanguage().LangSubTtl;//get lang
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
                        hx.svc.AV_CTRL.setSubtitleComponent({
                            'off': null,
                            'on': target
                        });
                    }
                }
            } else {
                this._isSubtitleDisplay = aIndex;
                hx.svc.SETTING_UTIL.setSubtitleEnabled(!!aIndex);
                if (!aIndex) {
                    hx.svc.AV_CTRL.offSubtitleComponent();
                }
            }
        },
        (aIndex: number) => {
            //subtitle font
            this._subfont = aIndex;
            hx.svc.SETTING_UTIL.setSubtitleFont(this._subfont);
        },
        (aIndex: number) => {
//            hard of hearing display
            this._isHardofHearing = aIndex;
            hx.svc.SETTING_UTIL.setHardofHearing(!!aIndex);
        }
    ];
    _handlersGetOIPFValue = [
        () => {
            return hx.svc.SETTING_UTIL.getInfoDisplayTimeout();
        },
        () => {
            return this.ATransparencyValue.indexOf(String(hx.svc.WEB_STORAGE.getBackgroundTransparency()));
        },
        () => {
            return hx.svc.SETTING_UTIL.getSubtitleEnabled() ? 1 : 0;
        },
        () => {
            return hx.svc.SETTING_UTIL.getSubtitleFont();
        },
        () => {
            return hx.svc.SETTING_UTIL.getHardofHearing() ? 1 : 0;
        }
    ];
    _handlersSetRefValue = [
        (aValue) => {
            this._displayTimeout = aValue;
        },
        (aValue) => {
            this._transparencyIdx = aValue;
        },
        (aValue) => {
            this._isSubtitleDisplay = aValue;
        },
        (aValue) => {
            this._subfont = aValue;
        },
        (aValue) => {
            this._isHardofHearing = aValue;
        }
    ];
    _handlersGetRefValue = [
        (): number => {
            return this._displayTimeout;
        },
        (): number => {
            return this._transparencyIdx;
        },
        (): number => {
            return this._isSubtitleDisplay;
        },
        (): number => {
            return this._subfont;
        },
        (): number => {
            return this._isHardofHearing;
        }
    ];
    private _dataHandler: ifDataHandlerSelectionMenu;
    constructor() {
        super();
        this.setDataCutted(this.productData.general_settings.on_screen_display);
        this._dataHandler = this.getHandlerSelectionMenu();
    }
    create() {
        super.create();
        var dataHandler = this._dataHandler;
        var titles = dataHandler.getItemTitles();
        this._$menu = $('<article>', {
            'class' : 'menu'
        });
        this._$contextMenu = $('<div>', {
            'class' : 'dep2'
        });
        this._screenMainMenu = this.screenMainMenu(titles, "dep1");
        var $title = $('#set .title');
        if ($title[0]) {
            (<HTMLElement>$title[0].children[0]).innerHTML = hx.l(dataHandler.getTitle());
        }

        var title: string;
        var option: string[];
        for (var i = 0; i < titles.length; i++) {
            title = titles[i];
            option = dataHandler.getItemOptions(title);
            this._handlersSetRefValue[i](this._handlersGetOIPFValue[i]());
            this._screenMainMenu.getItem(i).find('em').html(hx.l(option[this._handlersGetRefValue[i]()]));
        }
        if(ENV.op === ENV.listOP.UKDTT) {
            if( this._isSubtitleDisplay === 0) {
                this._screenMainMenu.getItem(this.TMenuList.HARD_OF_HEARING).addClass('dis');
            }
        }

        $('#set')
            .append(this._$menu.
                append(this._$contextMenu.after(this._screenMainMenu.$))
            );

        hx.al.compose(this);

        $title.ani({
            style : this.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });
        this._$menu.ani({
            style : this.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            },
            cb : () => {
                this.append(this._screenMainMenu);
                this._screenMainMenu.on();
                this._isChangePageInSetting = false;
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
        this._screenMainMenu = undefined;
    }
    _on_alClicked(aParam): boolean {
        var data = aParam.alData, target = data.me, bConsumed: boolean = false;

        switch (target) {
            case this._screenMainMenu:
                bConsumed = true;
                this._currentSubMenu = data.index;
                this._screenMainMenu.off();
                this._make_editSubMenu(data.index);
                break;
            case this._subList:
                bConsumed = true;
                this._subList.off();
                this._$contextMenu.ani({
                    style : this.aniConfig.dep2.fadeOut,
                    trans : {
                        dur : 150,
                        delay : 75
                    },
                    cb : () => {
                        var selectedMenuIndex = this._currentSubMenu;
//                        this._setSubListFocusedIndex(selectedMenuIndex, data.index + data.me.currentIndex);
                        this._setSubListFocusedIndex(selectedMenuIndex, data.absoluteIndex);
                    }
                });
                break;
        }
        return bConsumed;
    }
    _on_alExit(aParam): boolean {
        var bConsumed: boolean = false;
        if (aParam.alData) {
            if (aParam.alData.me === this._screenMainMenu) {
                bConsumed = true;
                this._isChangePageInSetting = true;
                hx.al.decompose(this);
                this._$menu.ani({
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
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
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
            case hx.key.KEY_BACK:
                if (aParam.alData) {
                    bConsumed = true;
                    if (aParam.alData.me === this._subList) {
                        this._subList.off();
                        this._$contextMenu.ani({
                            style : this.aniConfig.dep2.fadeOut,
                            trans : {
                                dur : 150,
                                delay : 75
                            },
                            cb : () => {
                                var menuFocusIndex = this._currentSubMenu;
                                this._subList.$.remove();
                                this._screenMainMenu.getItem(menuFocusIndex).removeClass('his');
                                this._$contextMenu.removeClass('active');
                                this._screenMainMenu.setItemFocus(menuFocusIndex);
                                this._screenMainMenu.on();
                            }
                        });
                    }
                }
                break;
        }
        return bConsumed;
    }
    _on_alScrollLeft(aParam): boolean {
        var bConsumed: boolean = false;

        if (aParam.alData.me === this._subList) {
            bConsumed = true;
            this._subList.off();
            this._$contextMenu.ani({
                style : this.aniConfig.dep2.fadeOut,
                trans : {
                    dur : 150,
                    delay : 75
                },
                cb : () => {
                    var menuFocusIndex = this._currentSubMenu;
                    this._subList.$.remove();
                    this._screenMainMenu.getItem(menuFocusIndex).removeClass('his');
                    this._$contextMenu.removeClass('active');
                    this._screenMainMenu.setItemFocus(menuFocusIndex);
                    this._screenMainMenu.on();
                }
            });
        }
        return bConsumed;
    }
    _make_editSubMenu(aIndex: number) {
        var subFocusIndex: number = this._handlersGetRefValue[aIndex]();
        var dataHandler = this._dataHandler;
        var dataList = dataHandler.getItemOptions(dataHandler.getItemTitles()[aIndex]);
        this._currentSubMenu = aIndex;
        this._subList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : subFocusIndex,
            displayCount : dataList.length
        });
        this._$contextMenu.addClass('active');
        this._screenMainMenu.getItem(aIndex).addClass('his');
        this._$contextMenu.append(this._subList.$);
        this._subList.bind(dataList);
        this._subList.draw();
        this._$contextMenu.ani({
            style : this.aniConfig.dep2.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            },
            cb : () => {
                this.append(this._subList);
                this._subList.on();
            }
        });
    }
    screenMainMenu(aTitles: string[], aClassName: string) {
        var screenmainmenu = hx.al.component('settingsList', {
            itemAttr : {},
            attr : {
                'class' : aClassName
            },
            data : aTitles,
            count : aTitles.length,
            fx : function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>').html(""));
            },
            keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return screenmainmenu;
    }
    _setSubListFocusedIndex(aIndexMain: number, aIndexSub: number) {
        var $el = this._screenMainMenu.getItem(aIndexMain);
        var dataHandler = this._dataHandler;
        var option = dataHandler.getItemOptions(dataHandler.getItemTitles()[aIndexMain]);
//        aIndexSub = aIndexSub % option.length
        var stringID = option[aIndexSub];
        this.doSetSubListFocusedIndex(aIndexMain, aIndexSub);
        this._handlers[aIndexMain](aIndexSub);
        if(ENV.op === ENV.listOP.UKDTT) {
            if (aIndexMain === this.TMenuList.SUBTITLE_DISPLAY) {//Subtitle Display
                if (this._isSubtitleDisplay === 0) {
                    this._screenMainMenu.getItem(this.TMenuList.HARD_OF_HEARING).addClass('dis');
                    this._screenMainMenu.getItem(this.TMenuList.HARD_OF_HEARING).find('em').html(hx.l(stringID));
                } else {
                    this._screenMainMenu.getItem(this.TMenuList.HARD_OF_HEARING).removeClass('dis');
                }
            }
        }
        $el.find('em').html(hx.l(stringID));
        $el.removeClass('his');
        this._$contextMenu.removeClass('active rol');
        this._subList.$.remove();
        this._screenMainMenu.on();
        this._screenMainMenu.setItemFocus(aIndexMain);
    }
    doSetSubListFocusedIndex(aIndexMain: number, aIndexSub: number) {
//        throw 'do implement in inherited class';
    }
    //TODO: to cutting unused menu item
    setUnusedMenu(aMenuIndexes: number[]) {
        var cutter: number;
        var len = aMenuIndexes.length;
        if (len > this._handlers.length) {
            throw 'check _handlers.length';
        }
        aMenuIndexes.sort(function(a,b) {
            return b - a;
        });
        for (var i = 0; i < len; i++) {
            cutter = aMenuIndexes.shift();
            this._handlers.splice(cutter, 1);
            this._handlersGetOIPFValue.splice(cutter, 1);
            this._handlersGetRefValue.splice(cutter, 1);
            this._handlersSetRefValue.splice(cutter, 1);
        }
    }
}
export = cPgScreenDisplay;
