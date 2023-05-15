var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    var cPgLanguage = (function (_super) {
        __extends(cPgLanguage, _super);
        function cPgLanguage() {
            var _this = this;
            _super.call(this);
            this.TMenuList = {
                SYSTEMLANGUAGE: 0,
                AUDIOLANGUAGE: 1,
                SUBTITLELANGUAGE: 2
            };
            this.TLanguageList = {
                LANGUAGECODE: 0,
                LANGUAGESTRID: 1
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
                    }
                }
            };
            // All the language definition with structure format.[ISO language code,...][Language String ID,....]
            this._languageListMap = [
                ['eng', 'ger', 'tur', 'rus', 'fre', 'ara', 'per', 'gre', 'ita', 'spa', 'gla', 'wel', 'gle', 'slo', 'ces'],
                ['LOC_SET_ENGLISH_ID', 'LOC_SET_GERMAN_ID', 'LOC_SET_TURKISH_ID', 'LOC_SET_RUSSIAN_ID', 'LOC_SET_FRENCH_ID', 'LOC_SET_ARABIC_ID', 'LOC_SET_PERSIAN_ID', 'LOC_SET_GREEK_ID', 'LOC_SET_ITALIAN_ID', 'LOC_SET_SPANISH_ID', 'LOC_LANG_GARLIC_ID', 'LOC_LANG_WELSH_ID', 'LOC_LANG_IRISH_ID', 'LOC_SET_SLOVAK_ID', 'LOC_SET_CZECH_ID']
            ];
            this._handlers = [
                function (aIndex) {
                    _this._menuLang = _this._conv_idx_to_iso639(aIndex);
                    hx.svc.SETTING_UTIL.setLanguage({
                        LangMenu: _this._menuLang
                    });
                    hx.l.reload(_this._menuLang, function () {
                        _this.sendEventToEntity({
                            alEvent: 'alMessageToPage',
                            target: 'CPgHubProxy?updateLanguage'
                        });
                        _this._updateDateTime();
                    });
                },
                function (aIndex) {
                    _this._audioLang = _this._conv_idx_to_iso639(aIndex);
                    hx.svc.SETTING_UTIL.setLanguage({
                        LangAudio: _this._audioLang
                    });
                },
                function (aIndex) {
                    _this._subLang = _this._conv_subttlidx_to_iso639(aIndex);
                    hx.svc.SETTING_UTIL.setLanguage({
                        LangSubTtl: _this._subLang
                    });
                }
            ];
            this.setDataCutted(this.productData.general_settings.language);
            this._dataHandler = this.getHandlerSelectionMenu();
        }
        cPgLanguage.prototype.create = function () {
            var _this = this;
            _super.prototype.create.call(this);
            this._$menu = $('<article>', { 'class': 'menu' });
            this._$contextMenu = $('<div>', { 'class': 'dep2' });
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
            $('#set').append(this._$menu.append(this._$contextMenu.after(this._LangMainMenu.$)));
            hx.al.compose(this);
            $('#set .title').ani({
                style: this.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            this._$menu.ani({
                style: this.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                },
                cb: function () {
                    _this.append(_this._LangMainMenu);
                    _this._LangMainMenu.on();
                    _this._isChangePageInSetting = false;
                }
            });
        };
        cPgLanguage.prototype._createItem = function () {
            var LangObj = hx.svc.SETTING_UTIL.getLanguage();
            var titles = this._dataHandler.getItemTitles();
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
        };
        cPgLanguage.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            this._$menu.remove();
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
            this._$menu = undefined;
            this._$contextMenu = undefined;
            this._LangMainMenu = undefined;
        };
        cPgLanguage.prototype._conv_iso639_to_DispText = function (aIso639) {
            var languageListCode = this._languageListMap[this.TLanguageList.LANGUAGECODE];
            var languageListStrID = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
            var DispText = 'LOC_OFF_ID';
            var i;
            for (i = 0; i < languageListCode.length; i += 1) {
                if (aIso639 === languageListCode[i]) {
                    DispText = languageListStrID[i];
                    break;
                }
            }
            return DispText;
        };
        cPgLanguage.prototype._conv_DispText_to_iso639 = function (aDispText) {
            var languageListCode = this._languageListMap[this.TLanguageList.LANGUAGECODE];
            var languageListStrID = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
            var languageCode = 'off';
            var i;
            for (i = 0; i < languageListCode.length; i += 1) {
                if (aDispText === languageListStrID[i]) {
                    languageCode = languageListCode[i];
                    break;
                }
            }
            return languageCode;
        };
        cPgLanguage.prototype._conv_idx_to_iso639 = function (aIdx) {
            var languageListCode = this._languageListMap[this.TLanguageList.LANGUAGECODE];
            var languageListStrID = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
            var dataHandlers = this._dataHandler;
            var supportLanguageLists = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[this._currentSubMenu]);
            var i;
            var DispText = supportLanguageLists[aIdx];
            var languageCode = 'LOC_SET_ENGLISH_ID';
            for (i = 0; i < languageListCode.length; i += 1) {
                if (DispText === languageListStrID[i]) {
                    languageCode = languageListCode[i];
                    break;
                }
            }
            return languageCode;
        };
        cPgLanguage.prototype._conv_iso639_to_idx = function (aIdx) {
            var languageListCode = this._languageListMap[this.TLanguageList.LANGUAGECODE];
            var languageListStrID = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
            var dataHandlers = this._dataHandler;
            var supportLanguageLists = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[this._currentSubMenu]);
            var i;
            var DispText = 'LOC_SET_ENGLISH_ID';
            for (i = 0; i < languageListCode.length; i += 1) {
                if (aIdx === languageListCode[i]) {
                    DispText = languageListStrID[i];
                    break;
                }
            }
            var index = 0;
            for (i = 0; i < supportLanguageLists.length; i += 1) {
                if (DispText === supportLanguageLists[i]) {
                    index = i;
                    break;
                }
            }
            return index;
        };
        cPgLanguage.prototype._conv_subttlidx_to_iso639 = function (aIdx) {
            var languageListCode = this._languageListMap[this.TLanguageList.LANGUAGECODE];
            var languageListStrID = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
            var dataHandlers = this._dataHandler;
            var supportLanguageLists = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[this._currentSubMenu]);
            var DispText = supportLanguageLists[aIdx];
            var languageCode = 'off';
            var i;
            for (i = 0; i < languageListCode.length; i += 1) {
                if (DispText === languageListStrID[i]) {
                    languageCode = languageListCode[i];
                    break;
                }
            }
            return languageCode;
        };
        cPgLanguage.prototype._conv_iso639_to_subttlidx = function (aIdx) {
            var languageListCode = this._languageListMap[this.TLanguageList.LANGUAGECODE];
            var languageListStrID = this._languageListMap[this.TLanguageList.LANGUAGESTRID];
            var dataHandlers = this._dataHandler;
            var supportLanguageLists = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[this._currentSubMenu]);
            var DispText = 'LOC_OFF_ID';
            var i;
            for (i = 0; i < languageListCode.length; i += 1) {
                if (aIdx === languageListCode[i]) {
                    DispText = languageListStrID[i];
                    break;
                }
            }
            var index = 0;
            for (i = 0; i < supportLanguageLists.length; i += 1) {
                if (DispText === supportLanguageLists[i]) {
                    index = i;
                    break;
                }
            }
            return index;
        };
        cPgLanguage.prototype._updateDateTime = function () {
            var now = new Date();
            var $dateTime = $('#set .dateTime');
            $dateTime.children().eq(2).html(now.format('ddd HH:MM'));
        };
        cPgLanguage.prototype._on_alClicked = function (aParam) {
            var _this = this;
            var data = aParam.alData, target = data.me, bConsumed = false;
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
                        trans: { dur: 150, delay: 75 },
                        cb: function () {
                            var selectedMenuIndex = _this._currentSubMenu;
                            _this._setSubListFocusedIndex(selectedMenuIndex, data.index);
                            var $el = _this._LangMainMenu.getItem(selectedMenuIndex).find('em');
                            var text = _this._getEntryStringID(selectedMenuIndex);
                            $el.html(hx.l(text));
                            $el.attr('data-langid', text);
                        }
                    });
                    break;
            }
            return bConsumed;
        };
        cPgLanguage.prototype._on_alExit = function (aParam) {
            var _this = this;
            var bConsumed = false;
            if (aParam.alData) {
                if (aParam.alData.me === this._LangMainMenu) {
                    bConsumed = true;
                    this._isChangePageInSetting = true;
                    hx.al.decompose(this);
                    this._$menu.ani({
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
        cPgLanguage.prototype._on_alKey = function (aParam) {
            var _this = this;
            var bConsumed = false;
            switch (aParam.alKey) {
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
                                trans: { dur: 150, delay: 75 },
                                cb: function () {
                                    var menuFocusIndex = _this._currentSubMenu;
                                    _this._subList.$.remove();
                                    _this._$contextMenu.removeClass('active');
                                    _this._LangMainMenu.getItem(menuFocusIndex).removeClass('his');
                                    _this._LangMainMenu.setItemFocus(menuFocusIndex);
                                    _this._LangMainMenu.on();
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cPgLanguage.prototype._makesubList = function (aIndex) {
            var _this = this;
            var dataHandlers = this._dataHandler;
            var dataList = dataHandlers.getItemOptions(dataHandlers.getItemTitles()[aIndex]);
            this._currentSubMenu = aIndex;
            this._subList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: this._getSubListFocusedIndex(aIndex, dataHandlers.getItemOptions(dataHandlers.getItemTitles()[aIndex])),
                displayCount: dataList.length
            });
            this._$contextMenu.addClass('active');
            this._LangMainMenu.getItem(aIndex).addClass('his');
            this._$contextMenu.append(this._subList.$);
            this._subList.bind(dataList);
            this._subList.draw();
            this._$contextMenu.ani({
                style: this.aniConfig.dep2.fadeIn,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    _this.append(_this._subList);
                    _this._subList.on();
                }
            });
        };
        cPgLanguage.prototype._langMainMenu = function (aTitles, param2, param3) {
            var langmainmenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': param3
                },
                data: aTitles,
                count: aTitles.length,
                fx: function (i, o) {
                    o.attr('data-langid', this.data[i]).html(hx.l(this.data[i])).append($('<em>').attr('data-langid', '').html(''));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return langmainmenu;
        };
        cPgLanguage.prototype._getEntryStringID = function (aIndex) {
            return this.doGetEntryStringID(aIndex);
        };
        cPgLanguage.prototype._setSubListFocusedIndex = function (aIndexMain, aIndexSub) {
            this.doSetSubListFocusedIndex(aIndexMain, aIndexSub);
            this._handlers[aIndexMain](aIndexSub);
            this._LangMainMenu.getItem(aIndexMain).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._LangMainMenu.on();
            this._LangMainMenu.setItemFocus(aIndexMain);
        };
        cPgLanguage.prototype._getSubListFocusedIndex = function (aIndex, aArray) {
            return this.doGetSubListFocusedIndex(aIndex, aArray);
        };
        cPgLanguage.prototype.doGetEntryStringID = function (aIndex) {
            throw 'do implement in inherited class';
        };
        cPgLanguage.prototype.doSetSubListFocusedIndex = function (aIndexMain, aIndexSub) {
            throw 'do implement in inherited class';
        };
        cPgLanguage.prototype.doGetSubListFocusedIndex = function (aIndex, aArray) {
            throw 'do implement in inherited class';
        };
        return cPgLanguage;
    })(__KERNEL_SETTINGS__);
    return cPgLanguage;
});
