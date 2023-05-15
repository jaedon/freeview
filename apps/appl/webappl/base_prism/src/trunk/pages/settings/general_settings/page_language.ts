// file name : pages/settings/general_settings/language.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../../def/sugar.d.ts"/>
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx: ifSugar;

class cPgLanguage extends __KERNEL_SETTINGS__ {
    TMenuList = {
        SYSTEMLANGUAGE : 0,
        AUDIOLANGUAGE : 1,
        SUBTITLELANGUAGE : 2
    };
    TLanguageList = {
        LANGUAGECODE : 0,
        LANGUAGESTRID : 1
    };
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
    // All the language definition with structure format.[ISO language code,...][Language String ID,....]
    _languageListMap = [
        ['eng', 'ger', 'tur', 'rus', 'fre', 'ara', 'per', 'gre', 'ita', 'spa', 'gla', 'wel', 'gle', 'slo', 'ces'],
        ['LOC_SET_ENGLISH_ID', 'LOC_SET_GERMAN_ID', 'LOC_SET_TURKISH_ID', 'LOC_SET_RUSSIAN_ID', 'LOC_SET_FRENCH_ID', 'LOC_SET_ARABIC_ID', 'LOC_SET_PERSIAN_ID', 'LOC_SET_GREEK_ID', 'LOC_SET_ITALIAN_ID', 'LOC_SET_SPANISH_ID', 'LOC_LANG_GARLIC_ID', 'LOC_LANG_WELSH_ID', 'LOC_LANG_IRISH_ID', 'LOC_SET_SLOVAK_ID', 'LOC_SET_CZECH_ID']
    ];
    _subList: any;
    _$menu: JQuery;
    _$contextMenu: JQuery;
    _LangMainMenu: any;
    _menuLang: string;
    _audioLang: string;
    _subLang: string;
    _currentSubMenu: number;
    _isChangePageInSetting: boolean;
    _handlers = [
        (aIndex: number) => {
            this._menuLang = this._conv_idx_to_iso639(aIndex);
            hx.svc.SETTING_UTIL.setLanguage({
                LangMenu : this._menuLang
            });
            hx.l.reload(this._menuLang, () => {
                this.sendEventToEntity({
                    alEvent : 'alMessageToPage',
                    target : 'CPgHubProxy?updateLanguage'
                });
                this._updateDateTime();
            });
        },
        (aIndex: number) => {
            this._audioLang = this._conv_idx_to_iso639(aIndex);
            hx.svc.SETTING_UTIL.setLanguage({
                LangAudio : this._audioLang
            });
        },
        (aIndex: number) => {
            this._subLang = this._conv_subttlidx_to_iso639(aIndex);
            hx.svc.SETTING_UTIL.setLanguage({
                LangSubTtl : this._subLang
            });
        }
    ];
    private _dataHandler: ifDataHandlerSelectionMenu;
    constructor() {
        super();
        this.setDataCutted(this.productData.general_settings.language);
        this._dataHandler = this.getHandlerSelectionMenu();
    }
    create() {
        super.create();
        this._$menu = $('<article>', {'class' : 'menu'});
        this._$contextMenu = $('<div>', {'class' : 'dep2'});
        this._LangMainMenu = this._langMainMenu(this._dataHandler.getItemTitles(), null, 'dep1');
        this._menuLang = '';
        this._audioLang = '';
        this._subLang = '';
        this._currentSubMenu = 0;
        this._isChangePageInSetting = false;
        if ($('#set .title h1')) {
            $('#set .title h1').attr('data-langid', this._dataHandler.getTitle()).html(hx.l(this._dataHandler.getTitle()));
        }
        this._createItem();
        $('#set')
            .append(this._$menu
                .append(this._$contextMenu.after(this._LangMainMenu.$))
            );
        hx.al.compose(this);
        $('#set .title').ani({
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
                this.append(this._LangMainMenu);
                this._LangMainMenu.on();
                this._isChangePageInSetting = false;
            }
        });
    }
    _createItem() {
        var LangObj = hx.svc.SETTING_UTIL.getLanguage();
        var titles: string[] = this._dataHandler.getItemTitles();
        var i, len = titles.length;
        this._menuLang = LangObj.LangMenu;
        this._audioLang = LangObj.LangAudio;
        this._subLang = LangObj.LangSubTtl;

        for (i = 0; i < len; i++) {
            var $el = this._LangMainMenu.getItem(i).find('em');
            var text = this._getEntryStringID(i);
            $el.html(hx.l(text));
            $el.attr('data-langid', text);
        }
    }
    destroy() {
        super.destroy();
        this._$menu.remove();
        if(!this._isChangePageInSetting) {
            $('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }
        this._$menu = undefined;
        this._$contextMenu = undefined;
        this._LangMainMenu = undefined;
    }
    _conv_iso639_to_DispText(aIso639: string): string {
        var languageListCode = this._languageListMap[this.TLanguageList.LANGUAGECODE];
        var languageListStrID = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
        var DispText: string = 'LOC_OFF_ID';
        var i;
        for (i = 0; i < languageListCode.length; i+=1) {
            if (aIso639 === languageListCode[i]) {
                DispText = languageListStrID[i];
                break;
            }
        }
        return DispText;
    }
    _conv_DispText_to_iso639(aDispText: string): string {
        var languageListCode: string[] = this._languageListMap[this.TLanguageList.LANGUAGECODE];
        var languageListStrID: string[] = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
        var languageCode: string = 'off';
        var i;
        for (i = 0; i < languageListCode.length; i+=1) {
            if (aDispText === languageListStrID[i]) {
                languageCode = languageListCode[i];
                break;
            }
        }
        return languageCode;
    }
    _conv_idx_to_iso639(aIdx: number): string {
        var languageListCode: string[] = this._languageListMap[this.TLanguageList.LANGUAGECODE];
        var languageListStrID: string[] = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
        var dataHandlers = this._dataHandler;
        var supportLanguageLists = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[this._currentSubMenu]);
        var i;
        var DispText = supportLanguageLists[aIdx];
        var languageCode: string = 'LOC_SET_ENGLISH_ID';
        for (i = 0; i < languageListCode.length; i+=1) {
            if (DispText === languageListStrID[i]) {
                languageCode = languageListCode[i];
                break;
            }
        }
        return languageCode;
    }
    _conv_iso639_to_idx(aIdx: string): number {
        var languageListCode: string[] = this._languageListMap[this.TLanguageList.LANGUAGECODE];
        var languageListStrID: string[] = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
        var dataHandlers = this._dataHandler;
        var supportLanguageLists = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[this._currentSubMenu]);
        var i;
        var DispText: string = 'LOC_SET_ENGLISH_ID';
        for (i = 0; i < languageListCode.length; i+=1) {
            if (aIdx === languageListCode[i]) {
                DispText = languageListStrID[i];
                break;
            }
        }
        var index = 0;
        for (i = 0; i < supportLanguageLists.length; i+=1) {
            if (DispText === supportLanguageLists[i]) {
                index = i;
                break;
            }
        }
        return index;
    }
    _conv_subttlidx_to_iso639(aIdx: number): string {
        var languageListCode: string[] = this._languageListMap[this.TLanguageList.LANGUAGECODE];
        var languageListStrID: string[] = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
        var dataHandlers = this._dataHandler;
        var supportLanguageLists = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[this._currentSubMenu]);
        var DispText = supportLanguageLists[aIdx];
        var languageCode: string = 'off';
        var i;
        for (i = 0; i < languageListCode.length; i+=1) {
            if (DispText === languageListStrID[i]) {
                languageCode = languageListCode[i];
                break;
            }
        }
        return languageCode;
    }
    _conv_iso639_to_subttlidx(aIdx: string): number {
        var languageListCode: string[] = this._languageListMap[this.TLanguageList.LANGUAGECODE];
        var languageListStrID: string[] = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
        var dataHandlers = this._dataHandler;
        var supportLanguageLists = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[this._currentSubMenu]);
        var DispText: string = 'LOC_OFF_ID';
        var i;
        for (i = 0; i < languageListCode.length; i+=1) {
            if (aIdx === languageListCode[i]) {
                DispText = languageListStrID[i];
                break;
            }
        }
        var index: number = 0;
        for (i = 0; i < supportLanguageLists.length; i+=1) {
            if (DispText === supportLanguageLists[i]) {
                index = i;
                break;
            }
        }
        return index;
    }
    _updateDateTime() {
        var now = new Date();
        var $dateTime = $('#set .dateTime');

        $dateTime.children().eq(2).html(now.format('ddd HH:MM'));
    }
    _on_alClicked(aParam): boolean {
        var data = aParam.alData, target = data.me, bConsumed: boolean = false;
        switch (target) {
            case this._LangMainMenu:
                bConsumed = true;
                this._LangMainMenu.off();
                this._makesubList(data.index);
                break;
            case this._subList:
                bConsumed = true;
                this._subList.off();
                this._$contextMenu.ani({
                    style: this.aniConfig.dep2.fadeOut,
                    trans: {dur: 150, delay: 75},
                    cb: () => {
                        var selectedMenuIndex = this._currentSubMenu;
                        this._setSubListFocusedIndex(selectedMenuIndex, data.index);
                        var $el = this._LangMainMenu.getItem(selectedMenuIndex).find('em');
                        var text = this._getEntryStringID(selectedMenuIndex);
                        $el.html(hx.l(text));
                        $el.attr('data-langid', text);
                    }
                });
                break;
        }
        return bConsumed;
    }
    _on_alExit(aParam): boolean {
        var bConsumed: boolean = false;
        if (aParam.alData) {
            if (aParam.alData.me === this._LangMainMenu) {
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
    _on_alKey(aParam) {
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
                    if (aParam.alData.me === this._subList) {
                        this._subList.off();
                        this._$contextMenu.ani({
                            style: this.aniConfig.dep2.fadeOut,
                            trans: {dur: 150, delay: 75},
                            cb: () => {
                                var menuFocusIndex = this._currentSubMenu;
                                this._subList.$.remove();
                                this._$contextMenu.removeClass('active');
                                this._LangMainMenu.getItem(menuFocusIndex).removeClass('his');
                                this._LangMainMenu.setItemFocus(menuFocusIndex);
                                this._LangMainMenu.on();
                            }
                        });
                    }
                }
                break;
        }
        return bConsumed;
    }
    _makesubList(aIndex: number) {
        var dataHandlers = this._dataHandler;
        var dataList = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[aIndex]);
        this._currentSubMenu = aIndex;
        this._subList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : this._getSubListFocusedIndex(aIndex, dataHandlers.getItemOptions(dataHandlers.getItemTitles()[aIndex])),
            displayCount : dataList.length
        });
        this._$contextMenu.addClass('active');
        this._LangMainMenu.getItem(aIndex).addClass('his');
        this._$contextMenu.append(this._subList.$);
        this._subList.bind(dataList);
        this._subList.draw();
        this._$contextMenu.ani({
            style: this.aniConfig.dep2.fadeIn,
            trans: {dur: 200, delay: 100},
            cb: () => {
                this.append(this._subList);
                this._subList.on();
            }
        });
    }
    _langMainMenu(aTitles: string[], param2, param3) {
        var langmainmenu = hx.al.component('settingsList', {
            itemAttr : {},
            attr : {
                'class' : param3
            },
            data : aTitles,
            count : aTitles.length,
            fx : function (i, o) {
                o.attr('data-langid', this.data[i]).html(hx.l(this.data[i])).append($('<em>').attr('data-langid', '').html(''));
            },
            keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return langmainmenu;
    }
    _getEntryStringID(aIndex: number): string {
        return this.doGetEntryStringID(aIndex);
    }
    _setSubListFocusedIndex(aIndexMain: number, aIndexSub: number) {
        this.doSetSubListFocusedIndex(aIndexMain, aIndexSub);
        this._handlers[aIndexMain](aIndexSub);
        this._LangMainMenu.getItem(aIndexMain).removeClass('his');
        this._subList.$.remove();
        this._$contextMenu.removeClass('active');
        this._LangMainMenu.on();
        this._LangMainMenu.setItemFocus(aIndexMain);
    }
    _getSubListFocusedIndex(aIndex: number, aArray: any[]): number {
        return this.doGetSubListFocusedIndex(aIndex, aArray);
    }
    doGetEntryStringID(aIndex: number): string {
        throw 'do implement in inherited class';
    }
    doSetSubListFocusedIndex(aIndexMain: number, aIndexSub: number) {
        throw 'do implement in inherited class';
    }
    doGetSubListFocusedIndex(aIndex: number, aArray: any[]): number {
        throw 'do implement in inherited class';
    }
}
export = cPgLanguage;