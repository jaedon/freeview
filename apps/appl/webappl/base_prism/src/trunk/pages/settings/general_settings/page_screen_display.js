var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    var cPgScreenDisplay = (function (_super) {
        __extends(cPgScreenDisplay, _super);
        function cPgScreenDisplay() {
            var _this = this;
            _super.call(this);
            this.TMenuList = {
                DISPLAY_TIME: 0,
                TRANSPARENCY: 1,
                SUBTITLE_DISPLAY: 2,
                SUBTITLE_FONT: 3,
                HARD_OF_HEARING: 4
            };
            this.ATransparencyValue = ['1', '0.8'];
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
            this._displayTimeout = 0;
            this._transparencyIdx = 1;
            this._isChangePageInSetting = false;
            this._handlers = [
                function (aIndex) {
                    //display time
                    _this._displayTimeout = aIndex;
                    hx.svc.SETTING_UTIL.setInfoDisplayTimeout({
                        'timeout': _this._displayTimeout
                    });
                },
                function (aIndex) {
                    //display time
                    _this._transparencyIdx = aIndex;
                    hx.svc.WEB_STORAGE.setBackgroundTransparency(_this.ATransparencyValue[aIndex]);
                    hx.il.updatePageBackground();
                },
                function (aIndex) {
                    //            subtitle display
                    if (ENV.op === ENV.listOP.UKDTT) {
                        _this._isSubtitleDisplay = aIndex;
                        hx.svc.SETTING_UTIL.setSubtitleEnabled(!!aIndex);
                        if (!aIndex) {
                            hx.svc.AV_CTRL.offSubtitleComponent();
                        }
                        else {
                            var subtitle = null;
                            var hardofHearingEnabled = hx.svc.SETTING_UTIL.getHardofHearing();
                            var p = hx.svc.AV_CTRL.getSubtitleComponent();
                            if (p !== undefined) {
                                subtitle = p.subtitle;
                            }
                            if (subtitle === null || subtitle.list === null || subtitle.list.length === 0) {
                                return;
                            }
                            else {
                                var target = null;
                                var len = subtitle.list.length;
                                var targetindex = subtitle.index;
                                var targetlang;
                                var i;
                                targetlang = hx.svc.SETTING_UTIL.getLanguage().LangSubTtl; //get lang
                                for (i = 0; i < len; i++) {
                                    console.log("  subtitle.list[i].language = " + subtitle.list[i].language);
                                    if (subtitle.list[i].language === targetlang) {
                                        if (subtitle.list[i].hearingImpaired === hardofHearingEnabled) {
                                            targetindex = i;
                                            break;
                                        }
                                    }
                                }
                                if (targetindex === null && hardofHearingEnabled === false) {
                                    for (i = 0; i < len; i++) {
                                        if (subtitle.list[i].language === targetlang) {
                                            targetindex = i;
                                            break;
                                        }
                                    }
                                }
                                if (targetindex !== null) {
                                    target = subtitle.list[targetindex];
                                }
                                else {
                                    target = subtitle.list[0];
                                }
                                hx.svc.AV_CTRL.setSubtitleComponent({
                                    'off': null,
                                    'on': target
                                });
                            }
                        }
                    }
                    else {
                        _this._isSubtitleDisplay = aIndex;
                        hx.svc.SETTING_UTIL.setSubtitleEnabled(!!aIndex);
                        if (!aIndex) {
                            hx.svc.AV_CTRL.offSubtitleComponent();
                        }
                    }
                },
                function (aIndex) {
                    //subtitle font
                    _this._subfont = aIndex;
                    hx.svc.SETTING_UTIL.setSubtitleFont(_this._subfont);
                },
                function (aIndex) {
                    //            hard of hearing display
                    _this._isHardofHearing = aIndex;
                    hx.svc.SETTING_UTIL.setHardofHearing(!!aIndex);
                }
            ];
            this._handlersGetOIPFValue = [
                function () {
                    return hx.svc.SETTING_UTIL.getInfoDisplayTimeout();
                },
                function () {
                    return _this.ATransparencyValue.indexOf(String(hx.svc.WEB_STORAGE.getBackgroundTransparency()));
                },
                function () {
                    return hx.svc.SETTING_UTIL.getSubtitleEnabled() ? 1 : 0;
                },
                function () {
                    return hx.svc.SETTING_UTIL.getSubtitleFont();
                },
                function () {
                    return hx.svc.SETTING_UTIL.getHardofHearing() ? 1 : 0;
                }
            ];
            this._handlersSetRefValue = [
                function (aValue) {
                    _this._displayTimeout = aValue;
                },
                function (aValue) {
                    _this._transparencyIdx = aValue;
                },
                function (aValue) {
                    _this._isSubtitleDisplay = aValue;
                },
                function (aValue) {
                    _this._subfont = aValue;
                },
                function (aValue) {
                    _this._isHardofHearing = aValue;
                }
            ];
            this._handlersGetRefValue = [
                function () {
                    return _this._displayTimeout;
                },
                function () {
                    return _this._transparencyIdx;
                },
                function () {
                    return _this._isSubtitleDisplay;
                },
                function () {
                    return _this._subfont;
                },
                function () {
                    return _this._isHardofHearing;
                }
            ];
            this.setDataCutted(this.productData.general_settings.on_screen_display);
            this._dataHandler = this.getHandlerSelectionMenu();
        }
        cPgScreenDisplay.prototype.create = function () {
            var _this = this;
            _super.prototype.create.call(this);
            var dataHandler = this._dataHandler;
            var titles = dataHandler.getItemTitles();
            this._$menu = $('<article>', {
                'class': 'menu'
            });
            this._$contextMenu = $('<div>', {
                'class': 'dep2'
            });
            this._screenMainMenu = this.screenMainMenu(titles, "dep1");
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l(dataHandler.getTitle());
            }
            var title;
            var option;
            for (var i = 0; i < titles.length; i++) {
                title = titles[i];
                option = dataHandler.getItemOptions(title);
                this._handlersSetRefValue[i](this._handlersGetOIPFValue[i]());
                this._screenMainMenu.getItem(i).find('em').html(hx.l(option[this._handlersGetRefValue[i]()]));
            }
            if (ENV.op === ENV.listOP.UKDTT) {
                if (this._isSubtitleDisplay === 0) {
                    this._screenMainMenu.getItem(this.TMenuList.HARD_OF_HEARING).addClass('dis');
                }
            }
            $('#set').append(this._$menu.append(this._$contextMenu.after(this._screenMainMenu.$)));
            hx.al.compose(this);
            $title.ani({
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
                    _this.append(_this._screenMainMenu);
                    _this._screenMainMenu.on();
                    _this._isChangePageInSetting = false;
                }
            });
        };
        cPgScreenDisplay.prototype.destroy = function () {
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
            this._screenMainMenu = undefined;
        };
        cPgScreenDisplay.prototype._on_alClicked = function (aParam) {
            var _this = this;
            var data = aParam.alData, target = data.me, bConsumed = false;
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
                        style: this.aniConfig.dep2.fadeOut,
                        trans: {
                            dur: 150,
                            delay: 75
                        },
                        cb: function () {
                            var selectedMenuIndex = _this._currentSubMenu;
                            //                        this._setSubListFocusedIndex(selectedMenuIndex, data.index + data.me.currentIndex);
                            _this._setSubListFocusedIndex(selectedMenuIndex, data.absoluteIndex);
                        }
                    });
                    break;
            }
            return bConsumed;
        };
        cPgScreenDisplay.prototype._on_alExit = function (aParam) {
            var _this = this;
            var bConsumed = false;
            if (aParam.alData) {
                if (aParam.alData.me === this._screenMainMenu) {
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
        cPgScreenDisplay.prototype._on_alKey = function (aParam) {
            var _this = this;
            var bConsumed = false;
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
                            text: ''
                        });
                        this.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: this
                        });
                    }
                    break;
                case hx.key.KEY_BACK:
                    if (aParam.alData) {
                        bConsumed = true;
                        if (aParam.alData.me === this._subList) {
                            this._subList.off();
                            this._$contextMenu.ani({
                                style: this.aniConfig.dep2.fadeOut,
                                trans: {
                                    dur: 150,
                                    delay: 75
                                },
                                cb: function () {
                                    var menuFocusIndex = _this._currentSubMenu;
                                    _this._subList.$.remove();
                                    _this._screenMainMenu.getItem(menuFocusIndex).removeClass('his');
                                    _this._$contextMenu.removeClass('active');
                                    _this._screenMainMenu.setItemFocus(menuFocusIndex);
                                    _this._screenMainMenu.on();
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cPgScreenDisplay.prototype._on_alScrollLeft = function (aParam) {
            var _this = this;
            var bConsumed = false;
            if (aParam.alData.me === this._subList) {
                bConsumed = true;
                this._subList.off();
                this._$contextMenu.ani({
                    style: this.aniConfig.dep2.fadeOut,
                    trans: {
                        dur: 150,
                        delay: 75
                    },
                    cb: function () {
                        var menuFocusIndex = _this._currentSubMenu;
                        _this._subList.$.remove();
                        _this._screenMainMenu.getItem(menuFocusIndex).removeClass('his');
                        _this._$contextMenu.removeClass('active');
                        _this._screenMainMenu.setItemFocus(menuFocusIndex);
                        _this._screenMainMenu.on();
                    }
                });
            }
            return bConsumed;
        };
        cPgScreenDisplay.prototype._make_editSubMenu = function (aIndex) {
            var _this = this;
            var subFocusIndex = this._handlersGetRefValue[aIndex]();
            var dataHandler = this._dataHandler;
            var dataList = dataHandler.getItemOptions(dataHandler.getItemTitles()[aIndex]);
            this._currentSubMenu = aIndex;
            this._subList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length
            });
            this._$contextMenu.addClass('active');
            this._screenMainMenu.getItem(aIndex).addClass('his');
            this._$contextMenu.append(this._subList.$);
            this._subList.bind(dataList);
            this._subList.draw();
            this._$contextMenu.ani({
                style: this.aniConfig.dep2.fadeIn,
                trans: {
                    dur: 200,
                    delay: 100
                },
                cb: function () {
                    _this.append(_this._subList);
                    _this._subList.on();
                }
            });
        };
        cPgScreenDisplay.prototype.screenMainMenu = function (aTitles, aClassName) {
            var screenmainmenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': aClassName
                },
                data: aTitles,
                count: aTitles.length,
                fx: function (i, o) {
                    o.html(hx.l(this.data[i])).append($('<em>').html(""));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return screenmainmenu;
        };
        cPgScreenDisplay.prototype._setSubListFocusedIndex = function (aIndexMain, aIndexSub) {
            var $el = this._screenMainMenu.getItem(aIndexMain);
            var dataHandler = this._dataHandler;
            var option = dataHandler.getItemOptions(dataHandler.getItemTitles()[aIndexMain]);
            //        aIndexSub = aIndexSub % option.length
            var stringID = option[aIndexSub];
            this.doSetSubListFocusedIndex(aIndexMain, aIndexSub);
            this._handlers[aIndexMain](aIndexSub);
            if (ENV.op === ENV.listOP.UKDTT) {
                if (aIndexMain === this.TMenuList.SUBTITLE_DISPLAY) {
                    if (this._isSubtitleDisplay === 0) {
                        this._screenMainMenu.getItem(this.TMenuList.HARD_OF_HEARING).addClass('dis');
                        this._screenMainMenu.getItem(this.TMenuList.HARD_OF_HEARING).find('em').html(hx.l(stringID));
                    }
                    else {
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
        };
        cPgScreenDisplay.prototype.doSetSubListFocusedIndex = function (aIndexMain, aIndexSub) {
            //        throw 'do implement in inherited class';
        };
        //TODO: to cutting unused menu item
        cPgScreenDisplay.prototype.setUnusedMenu = function (aMenuIndexes) {
            var cutter;
            var len = aMenuIndexes.length;
            if (len > this._handlers.length) {
                throw 'check _handlers.length';
            }
            aMenuIndexes.sort(function (a, b) {
                return b - a;
            });
            for (var i = 0; i < len; i++) {
                cutter = aMenuIndexes.shift();
                this._handlers.splice(cutter, 1);
                this._handlersGetOIPFValue.splice(cutter, 1);
                this._handlersGetRefValue.splice(cutter, 1);
                this._handlersSetRefValue.splice(cutter, 1);
            }
        };
        return cPgScreenDisplay;
    })(__KERNEL_SETTINGS__);
    return cPgScreenDisplay;
});
