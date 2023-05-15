var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var cCPgInstall06_1 = (function (_super) {
        __extends(cCPgInstall06_1, _super);
        function cCPgInstall06_1() {
            _super.call(this);
        }
        cCPgInstall06_1.prototype.create = function (aParam, aQuery) {
            hx.logadd('CPgInstall06_1');
            var i, _result;
            $.extend(this, {
                _containerEl: document.querySelector('#wiz'),
                _$article: $('<article>', {
                    'class': 'cont'
                }),
                _ok_btn: this.__make_btn('LOC_OK_ID'),
                _noThanks_btn: this.__make_btn('LOC_NO_THANKS_ID'),
                _show: aQuery.data.show,
                _$hArea: $('<h1>', {
                    'class': 'hArea'
                }),
                _$title: $('<span>', {
                    'class': 'tit',
                    'html': hx.l('LOC_PRIVACY_POLICY_LOWER_ID').toUpperCase()
                }),
                _resultEl: document.createElement('p'),
                _btnWrapEl: document.createElement('div')
            });
            //////////////////set Element attribute
            if (aQuery.query === 'prev') {
                this._$article.css('left', '-50px');
            }
            else {
                this._$article.css('left', '50px');
            }
            this._btnWrapEl.classList.add('btnBo');
            _result = hx.l('LOC_PRIVACY_POLICY_INFORM_01_ID') + hx.l('LOC_PRIVACY_POLICY_INFORM_02_ID') + hx.l('LOC_PRIVACY_POLICY_INFORM_03_ID') + '<br>' + '<br>' + hx.l('LOC_LEARN_HUMAX_PRIVACY_POLICY_ID');
            this._ok_btn.$[0].style.whiteSpace = 'nowrap';
            this._resultEl.innerHTML = _result;
            /////////////////append To DOM
            if (!this._containerEl) {
                this._containerEl = document.createElement('section');
                this._containerEl.id = 'wiz';
                document.querySelector('#wrap').appendChild(this._containerEl);
            }
            this._$hArea[0].appendChild(this._$title[0]);
            this._btnWrapEl.appendChild(this._ok_btn.$[0]);
            this._btnWrapEl.appendChild(this._noThanks_btn.$[0]);
            this._$article[0].appendChild(this._resultEl);
            this._$article[0].appendChild(this._btnWrapEl);
            this._containerEl.style.display = 'none';
            this._containerEl.appendChild(this._$hArea[0]);
            this._containerEl.appendChild(this._$article[0]);
            this._containerEl.style.display = 'block';
            /////////////Do Initial Script
            hx.al.compose(this).append(this._noThanks_btn).append(this._ok_btn);
            this._ok_btn.on();
            this._ok_btn.$.addClass('on');
            this._isGoWizardPage = false;
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
        cCPgInstall06_1.prototype.destroy = function () {
            clearInterval(this._timeset);
            if (this._containerEl) {
                if (!this._isGoWizardPage) {
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
            this._promoImgEl = undefined;
            this._noThanks_btn = undefined;
            this._ok_btn = undefined;
            this._show = undefined;
            this._$hArea = undefined;
            this._$title = undefined;
            this._stepEl = undefined;
            this._resultEl = undefined;
            this._graphicInfoEl = undefined;
            this._btnWrapEl = undefined;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(true);
        };
        cCPgInstall06_1.prototype.__make_btn = function (aId) {
            return hx.al.component('button', {
                attr: {
                    'class': 'button',
                    'data-langID': aId
                },
                usePointerKey: true
            });
        };
        cCPgInstall06_1.prototype._on_alScrollLeft = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._noThanks_btn:
                    bConsumed = true;
                    this._noThanks_btn.$.removeClass('on');
                    this._ok_btn.on();
                    this._ok_btn.$.addClass('on');
                    break;
                case this._ok_btn:
                    bConsumed = true;
                    this._ok_btn.$.removeClass('on');
                    this._noThanks_btn.on();
                    this._noThanks_btn.$.addClass('on');
                    break;
            }
            return bConsumed;
        };
        cCPgInstall06_1.prototype._on_alScrollRight = function (aPparam) {
            var data = aPparam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._noThanks_btn:
                    bConsumed = true;
                    this._noThanks_btn.$.removeClass('on');
                    this._ok_btn.on();
                    this._ok_btn.$.addClass('on');
                    break;
                case this._ok_btn:
                    bConsumed = true;
                    this._ok_btn.$.removeClass('on');
                    this._noThanks_btn.on();
                    this._noThanks_btn.$.addClass('on');
                    break;
            }
            return bConsumed;
        };
        cCPgInstall06_1.prototype._on_alExit = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._noThanks_btn:
                case this._ok_btn:
                    bConsumed = true;
                    this._go_pre_page();
                    break;
                default:
                    break;
            }
            return bConsumed;
        };
        cCPgInstall06_1.prototype._on_alClicked = function (aParam) {
            var bConsumed = false;
            var _key = aParam.alKey;
            if (aParam.alData.me === this._noThanks_btn && _key === hx.key.KEY_OK) {
                bConsumed = true;
                this._go_next_page(false);
            }
            else if (aParam.alData.me === this._ok_btn && _key === hx.key.KEY_OK) {
                bConsumed = true;
                this._go_next_page(true);
            }
            return bConsumed;
        };
        cCPgInstall06_1.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
            }
            return bConsumed;
        };
        cCPgInstall06_1.prototype._go_pre_page = function () {
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            if (this._show === 'success')
                this._fadeOutAni(hx.al.CPgInstall06_1.items.prevPage + '?prev,show=success', true);
            else
                this._fadeOutAni(hx.al.CPgInstall06_1.items.prevPage + '?prev,show=fail', true);
        };
        cCPgInstall06_1.prototype._go_next_page = function (aParam) {
            hx.svc.SETTING_UTIL.setPrivacyPolicy(aParam);
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(hx.al.CPgInstall06_1.items.nextPage, false);
        };
        cCPgInstall06_1.prototype._fadeOutAni = function (aTargetPage, aIsGoPrev) {
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
        return cCPgInstall06_1;
    })(__PAGE_KERNEL__);
    hx.al.CPgInstall06_1 = cCPgInstall06_1;
    /********************      Static Variables     ********************/
    if (hx.config.wzd.page06) {
        hx.al.CPgInstall06_1.items = hx.config.wzd.page06_1;
    }
    else {
        hx.al.CPgInstall06_1.items = {
            itemStr: [''],
            nextPage: 'CPgInstall06_2',
            prevPage: 'CPgInstall06'
        };
    }
    return cCPgInstall06_1;
});
