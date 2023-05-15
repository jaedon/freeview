var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var cCPgInstall08_1 = (function (_super) {
        __extends(cCPgInstall08_1, _super);
        function cCPgInstall08_1() {
            _super.call(this);
            this.TMenuItemValue_OffOn = {
                VALUE_OFF: 0,
                VALUE_ON: 1
            };
        }
        cCPgInstall08_1.prototype.create = function (aEntity, aPageUrl) {
            hx.logadd('CPgInstall08_1');
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(false);
            $.extend(this, {
                _containerEl: document.querySelector('#wiz'),
                _$article: $('<article class="cont">'),
                _pre_btn: this.__make_btn('LOC_PREVIOUS_ID'),
                _next_btn: this.__make_btn('LOC_NEXT_ID'),
                _$hArea: $('<h1>', {
                    'class': 'hArea'
                }),
                _$title: $('<span>', {
                    'class': 'tit',
                    'html': hx.l('LOC_POWER_SAVING_MODE_ID'),
                    'data-langID': 'LOC_POWER_SAVING_MODE_ID'
                }),
                _stepEl: document.createElement('span'),
                _descriptEl: document.createElement('p'),
                _descriptEl2: document.createElement('p'),
                _menuWrapEl: document.createElement('article'),
                _contextMenuEl: document.createElement('div'),
                _QBootMainMenu: hx.al.CPgInstall08_1.QBootMainMenu(hx.al.CPgInstall08_1.items.itemStr, 'notroll'),
                _btnWrapEl: document.createElement('div')
            });
            if (aPageUrl.query === 'prev') {
                this._$article.css('left', '-50px');
            }
            else {
                this._$article.css('left', '50px');
            }
            this._stepEl.classList.add(hx.al.CPgInstall08_1.items.step ? hx.al.CPgInstall08_1.items.step : 'step4');
            this._descriptEl.innerHTML = "Setting power saving mode to On will consume less power in standby (0.5W) but increase system start times.";
            this._descriptEl2.innerHTML = "If you wish to pair other devices or use loop-through in standby," + "<br>" + " please set the mode to Off now. Select NEXT and press OK to continue.";
            this._descriptEl2.classList.add('str2');
            this._menuWrapEl.classList.add('menu');
            this._menuWrapEl.classList.add('quickboot');
            this._menuWrapEl.classList.add('wrap');
            this._contextMenuEl.classList.add('dep2');
            this._btnWrapEl.classList.add('btnBo');
            this._currentOption = hx.svc.SETTING_UTIL.getStandbyPowerMode();
            this._drawCurrentOption();
            /////////////////append To DOM
            if (!this._containerEl) {
                this._containerEl = document.createElement('section');
                this._containerEl.id = 'wiz';
                document.querySelector('#wrap').appendChild(this._containerEl);
            }
            this._$hArea[0].appendChild(this._$title[0]);
            this._$hArea[0].appendChild(this._stepEl);
            this._menuWrapEl.appendChild(this._contextMenuEl);
            this._menuWrapEl.appendChild(this._QBootMainMenu.$[0]);
            this._btnWrapEl.appendChild(this._pre_btn.$[0]);
            this._btnWrapEl.appendChild(this._next_btn.$[0]);
            this._$article[0].appendChild(this._descriptEl);
            this._$article[0].appendChild(this._descriptEl2);
            this._$article[0].appendChild(this._menuWrapEl);
            this._$article[0].appendChild(this._btnWrapEl);
            this._containerEl.style.display = 'none';
            this._containerEl.appendChild(this._$hArea[0]);
            this._containerEl.appendChild(this._$article[0]);
            this._containerEl.style.display = 'block';
            hx.al.compose(this).append(this._QBootMainMenu).append(this._pre_btn).append(this._next_btn);
            this._QBootMainMenu.on();
            //        that._next_btn.$.addClass('on');
            this._isGoWizardPage = false;
            /////////////Do Initial Script
            hx.il.ani.action({
                targets: this._$hArea,
                cls: 'aniFadeIn'
            });
            hx.il.ani.action({
                targets: [this._$article],
                cls: 'aniFadeIn',
                style: 'left:0'
            });
        };
        cCPgInstall08_1.prototype.destroy = function () {
            if (this._containerEl) {
                if (!this._isGoWizardPage) {
                    if (this._msg) {
                        hx.msg.close('progress', this._msg);
                    }
                    this._containerEl.parentElement.removeChild(this._containerEl);
                }
                else {
                    this._containerEl.style.display = 'none';
                    this._containerEl.removeChild(this._$hArea[0]);
                    this._containerEl.removeChild(this._$article[0]);
                    this._containerEl.style.display = 'block';
                }
                this._containerEl = undefined;
            }
            this._$article = undefined;
            this._pre_btn = undefined;
            this._next_btn = undefined;
            this._$hArea = undefined;
            this._$title = undefined;
            this._stepEl = undefined;
            this._descriptEl = undefined;
            this._descriptEl2 = undefined;
            this._menuWrapEl = undefined;
            this._contextMenuEl = undefined;
            this._QBootMainMenu = undefined;
            this._btnWrapEl = undefined;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(true);
        };
        cCPgInstall08_1.prototype._on_alExit = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._QBootMainMenu:
                case this._QBootSubMenu:
                case this._pre_btn:
                case this._next_btn:
                    bConsumed = true;
                    this._go_pre_page();
                    break;
                default:
                    break;
            }
            return bConsumed;
        };
        cCPgInstall08_1.prototype._on_alScrollup = function (aParam) {
            var bConsumed = false;
            if (aParam.alData.me === this._pre_btn || aParam.alData.me === this._next_btn) {
                bConsumed = true;
                if (aParam.alData.me === this._pre_btn) {
                    this._pre_btn.$.removeClass('on');
                }
                else if (aParam.alData.me === this._next_btn) {
                    this._next_btn.$.removeClass('on');
                }
                this._QBootMainMenu.on();
            }
            else if (aParam.alData.me === this._QBootMainMenu) {
                bConsumed = true;
                this._next_btn.on();
                this._next_btn.$.addClass('on');
            }
            return bConsumed;
        };
        cCPgInstall08_1.prototype._on_alScrolldown = function (aParam) {
            var bConsumed = false;
            if (aParam.alData.me === this._pre_btn || aParam.alData.me === this._next_btn) {
                bConsumed = true;
                if (aParam.alData.me === this._pre_btn) {
                    this._pre_btn.$.removeClass('on');
                }
                else if (aParam.alData.me === this._next_btn) {
                    this._next_btn.$.removeClass('on');
                }
                this._QBootMainMenu.on();
            }
            else if (aParam.alData.me === this._QBootMainMenu) {
                bConsumed = true;
                this._next_btn.on();
                this._next_btn.$.addClass('on');
            }
            return bConsumed;
        };
        cCPgInstall08_1.prototype._on_alScrollLeft = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._pre_btn:
                    bConsumed = true;
                    this._pre_btn.$.removeClass('on');
                    this._next_btn.on();
                    this._next_btn.$.addClass('on');
                    break;
                case this._next_btn:
                    bConsumed = true;
                    this._next_btn.$.removeClass('on');
                    this._pre_btn.on();
                    this._pre_btn.$.addClass('on');
                    break;
            }
            return bConsumed;
        };
        cCPgInstall08_1.prototype._on_alScrollRight = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._pre_btn:
                    bConsumed = true;
                    this._pre_btn.$.removeClass('on');
                    this._next_btn.on();
                    this._next_btn.$.addClass('on');
                    break;
                case this._next_btn:
                    bConsumed = true;
                    this._next_btn.$.removeClass('on');
                    this._pre_btn.on();
                    this._pre_btn.$.addClass('on');
                    break;
            }
            return bConsumed;
        };
        cCPgInstall08_1.prototype._on_alClicked = function (aParam) {
            var bConsumed = false;
            var _key = aParam.alKey;
            var data = aParam.alData, target = data.me, index;
            if (target === this._pre_btn) {
                bConsumed = true;
                this._go_pre_page();
            }
            else if (target === this._next_btn) {
                bConsumed = true;
                // normal : 0,    saving : 1
                hx.svc.SETTING_UTIL.setStandbyPowerMode(this._currentOption);
                this._isGoWizardPage = true;
                hx.al.decompose(this);
                this._fadeOutAni(hx.al.CPgInstall08_1.items.nextPage, false);
            }
            else if (target === this._QBootMainMenu) {
                bConsumed = true;
                this._makeSubMenu(data.index);
                this._currentSubMenu = data.index;
            }
            else if (target === this._QBootSubMenu) {
                bConsumed = true;
                this._contextMenuEl.classList.remove('active');
                this._setOption(data.index);
            }
            return bConsumed;
        };
        cCPgInstall08_1.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_LEFT:
                    bConsumed = true;
                    if (aParam.alData.me === this._QBootSubMenu) {
                        this._removeSubMenu(this._currentSubMenu);
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgInstall08_1.prototype._go_pre_page = function () {
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(hx.al.CPgInstall08_1.items.prevPage + '?prev', true);
        };
        cCPgInstall08_1.prototype.__make_btn = function (aId) {
            return hx.al.component('button', {
                attr: {
                    'class': 'button',
                    'data-langID': aId
                },
                usePointerKey: true
            });
        };
        cCPgInstall08_1.prototype._setOption = function (aIndex) {
            this._currentOption = aIndex;
            this._drawCurrentOption();
            this._removeSubMenu(this._currentSubMenu);
        };
        cCPgInstall08_1.prototype._drawCurrentOption = function () {
            var $itemValue = this._QBootMainMenu.getItem(0).find('em');
            $itemValue.html(hx.l(hx.al.CPgInstall08_1.items.itemSubStr[this._currentOption]));
        };
        cCPgInstall08_1.prototype._fadeOutAni = function (aTargetPage, aIsGoPrev) {
            var _this = this;
            var left;
            if (aIsGoPrev) {
                left = '20px';
            }
            else {
                left = '-20px';
            }
            hx.il.ani.action({
                targets: this._$article,
                cls: 'aniFadeOut',
                style: 'left:' + left
            });
            hx.il.ani.action({
                targets: this._$title,
                cls: 'aniFadeOut',
                cb: function () {
                    _this.sendEventToEntity({
                        alEvent: 'alChangeInstallPage',
                        target: aTargetPage,
                        me: _this
                    });
                }
            });
        };
        cCPgInstall08_1.prototype._makeSubMenu = function (aParam) {
            var index = 0, subFocusIndex, dataList;
            dataList = hx.al.CPgInstall08_1.items.itemSubStr;
            subFocusIndex = this._currentOption;
            this._QBootSubMenu = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length
            });
            this._contextMenuEl.style.display = 'none';
            this._contextMenuEl.classList.add('active');
            this._QBootMainMenu.getItem(aParam).addClass('his');
            this.append(this._QBootSubMenu);
            this._contextMenuEl.appendChild(this._QBootSubMenu.$[0]);
            this._QBootSubMenu.on();
            this._QBootSubMenu.bind(dataList);
            this._QBootSubMenu.draw();
            this._contextMenuEl.style.display = 'block';
        };
        cCPgInstall08_1.prototype._removeSubMenu = function (aFocusIdx) {
            this._QBootSubMenu.$.remove();
            this._contextMenuEl.classList.remove('active');
            this._QBootMainMenu.getItem(aFocusIdx).removeClass('his');
            this._QBootMainMenu.setItemFocus(aFocusIdx);
            this._QBootMainMenu.on();
        };
        return cCPgInstall08_1;
    })(__PAGE_KERNEL__);
    hx.al.CPgInstall08_1 = cCPgInstall08_1;
    hx.al.CPgInstall08_1.QBootMainMenu = function (aParam, aParam2) {
        var QBootMainMenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': 'dep1'
            },
            data: aParam,
            roll: aParam2,
            count: aParam.length,
            fx: function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>'));
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return QBootMainMenu;
    };
    /********************      Static Variables     ********************/
    if (hx.config.wzd.page08_1) {
        hx.al.CPgInstall08_1.items = hx.config.wzd.page08_1;
    }
    else {
        hx.al.CPgInstall08_1.items = {
            itemStr: ['LOC_POWER_SAVING_MODE_MENU_ID'],
            itemSubStr: ['LOC_OFF_ID', 'LOC_ON_ID'],
            nextPage: 'CPgInstall09',
            prevPage: 'CPgInstall07',
            step: 'step4'
        };
    }
    return cCPgInstall08_1;
});